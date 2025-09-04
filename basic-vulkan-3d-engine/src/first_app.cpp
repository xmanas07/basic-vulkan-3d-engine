#include "first_app.hpp"

#include "keyboard_movement_controller.hpp"
#include "bve_camera.hpp"
#include "simple_render_system.hpp"
#include "point_light_system.hpp"
#include "bve_buffer.hpp"
#include "model_shapes.hpp"
#include "bve_texture.hpp"

// libs
#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>
#include <glm/gtc/constants.hpp>

//std
#include <array>
#include <chrono>
#include <cassert>
#include <stdexcept>
#include <cmath>

namespace bve {
	

	FirstApp::FirstApp()
	{
		globalPool = BveDescriptorPool::Builder(bveDevice)
			.setMaxSets(BveSwapChain::MAX_FRAMES_IN_FLIGHT)
			.addPoolSize(VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, BveSwapChain::MAX_FRAMES_IN_FLIGHT)
			.addPoolSize(VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, BveSwapChain::MAX_FRAMES_IN_FLIGHT)
			.build();
		loadGameObjects();
	}
	FirstApp::~FirstApp()
	{
	}

	void FirstApp::run() {

		std::vector<std::unique_ptr<BveBuffer>> uboBuffers(BveSwapChain::MAX_FRAMES_IN_FLIGHT);
		for (int i = 0; i < uboBuffers.size(); i++) {
			uboBuffers[i] = std::make_unique<BveBuffer>(
				bveDevice,
				sizeof(GlobalUbo),
				1,
				VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
				VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT);
			uboBuffers[i]->map();
		}

		auto globalSetLayout = BveDescriptorSetLayout::Builder(bveDevice)
			.addBinding(0, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, VK_SHADER_STAGE_ALL_GRAPHICS)
			.addBinding(1, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, VK_SHADER_STAGE_FRAGMENT_BIT)
			.build();

		BveTexture texture = BveTexture(bveDevice, "src/textures/RohlikSpaceX.png");

		VkDescriptorImageInfo imageInfo{};
		imageInfo.sampler = texture.getSampler();
		imageInfo.imageView = texture.getImageView();
		imageInfo.imageLayout = texture.getImageLayout();


		std::vector<VkDescriptorSet> globalDescriptorSets(BveSwapChain::MAX_FRAMES_IN_FLIGHT);
		for (int i = 0; i < globalDescriptorSets.size(); i++) {
			auto bufferInfo = uboBuffers[i]->descriptorInfo();
			BveDescriptorWriter(*globalSetLayout, *globalPool)
				.writeBuffer(0, &bufferInfo)
				.writeImage(1, &imageInfo)
				.build(globalDescriptorSets[i]);
		}

		SimpleRenderSystem simpleRenderSystem{ bveDevice, bveRenderer.getSwapchainRenderPass(), globalSetLayout->getDescriptorSetLayout()};
		PointLightSystem pointLightSystem{ bveDevice, bveRenderer.getSwapchainRenderPass(), globalSetLayout->getDescriptorSetLayout()};
		BveCamera camera{};
		camera.setViewTarget(glm::vec3(-1.f, -2.f, 2.f), glm::vec3(0.f, 0.f, 2.5f));

		auto viewerObject = BveGameObject::createGameObject();
		//viewerObject.model = createSierpPyramidModel(bveDevice, 1.f, 1); // uncoment to demonstrate having a model in a viewer object

		int viewerId = viewerObject.getId();
		addGameObject(std::move(viewerObject));
		BveGameObject& viewer = gameObjects.at(viewerId);
		KeyboardMovementController cameraController{};

		auto currentTime = std::chrono::high_resolution_clock::now();

		while (!bveWindow.shouldClose()) {
			glfwPollEvents();

			auto newTime = std::chrono::high_resolution_clock::now();
			float frameTime = std::chrono::duration<float, std::chrono::seconds::period>(newTime - currentTime).count();
			currentTime = newTime;
			

			cameraController.moveInPlaneXZ(bveWindow.getGLFWwindow(), frameTime, viewer);
			camera.setViewYXZ(viewer.transform.translation, viewer.transform.rotation);

			float aspect = bveRenderer.getAspectRatio();
			camera.setPerspectiveProjection(glm::radians(50.f), aspect, .1f, 10.f);

			if (auto commandBuffer = bveRenderer.beginFrame()) {
				int frameIndex = bveRenderer.getCurrentFrameIndex();
				FrameInfo frameInfo{
					frameIndex,
					frameTime,
					commandBuffer,
					camera,
					globalDescriptorSets[frameIndex],
					gameObjects
				};

				// update
				GlobalUbo ubo{};
				ubo.projection = camera.getProjection();
				ubo.view = camera.getView();
				ubo.inverseView = camera.getInverseView();
				pointLightSystem.update(frameInfo, ubo);
				uboBuffers[frameIndex]->writeToBuffer(&ubo);
				uboBuffers[frameIndex]->flush();
				
				// renderer
				bveRenderer.beginSwapChainRenderPass(commandBuffer);

				// make sure to render solid objects before transparent objects
				simpleRenderSystem.renderGameObjects(frameInfo);
				pointLightSystem.render(frameInfo);

				bveRenderer.endSwapChainRenderPass(commandBuffer);
				bveRenderer.endFrame();
			}
		}

		vkDeviceWaitIdle(bveDevice.device());
	}
	
	
	void FirstApp::loadGameObjects() {
		std::shared_ptr<BveModel> bveModel = BveModel::createModelFromFile(bveDevice, "src/models/smooth_vase.obj");
		auto gameObject = BveGameObject::createGameObject();
		gameObject.model = bveModel;
		gameObject.transform.translation = { .0f,0.5f,2.5f };
		gameObject.transform.scale = { 1.f,.2f,1.f };
		gameObjects.emplace(gameObject.getId(), std::move(gameObject));

		bveModel = BveModel::createModelFromFile(bveDevice, "src/models/smooth_brown_rat.obj");
		auto rat = BveGameObject::createGameObject();
		rat.model = bveModel;
		rat.transform.translation = { .0f,-0.0f,2.5f };
		rat.transform.scale = { .5f,.5f,.5f };
		rat.transform.rotation = {0.f, 0.f, -glm::half_pi<float>()};
		gameObjects.emplace(rat.getId(), std::move(rat));

		std::array<BveModel::Vertex, 4> pyramidVertices;
		pyramidVertices[0] = { {.0f,-.5f,.5f},{1.f,1.f,1.f} };
		pyramidVertices[1] = { {.0f,.5f,-.5f},{1.f,0.f,0.f} };
		pyramidVertices[2] = { {.5f,.5f,.5f},{0.f,1.f,0.f} };
		pyramidVertices[3] = { {-.5f,.5f,.5f},{0.f,0.f,1.f} };
		
		auto sierpPyramid = BveGameObject::createGameObject();
		sierpPyramid.model = createSierpPyramidModel(bveDevice, 1.f, 3);
		sierpPyramid.transform.translation = { 1.5f,-.5f,2.5f };
		sierpPyramid.transform.scale = { 1.f,1.f,1.f };
		gameObjects.emplace(sierpPyramid.getId(), std::move(sierpPyramid));
		
		bveModel = BveModel::createModelFromFile(bveDevice, "src/models/quad.obj");
		auto floor = BveGameObject::createGameObject();
		floor.model = bveModel;
		floor.transform.translation = { .0f,.5f,2.5f };
		floor.transform.scale = { 3.f,1.f,3.f };
		gameObjects.emplace(floor.getId(), std::move(floor));


		std::vector<glm::vec3> lightColors{
		  {1.f, .1f, .1f},
		  {.1f, .1f, 1.f},
		  {.1f, 1.f, .1f},
		  {1.f, 1.f, .1f},
		  {.1f, 1.f, 1.f},
		  {1.f, 1.f, 1.f}  //
		};

		for (int i = 0; i < lightColors.size(); i++)
		{
			auto pointLight = BveGameObject::makePointLight(.5f);
			pointLight.color = lightColors[i];
			auto rotateLight = glm::rotate(glm::mat4(1.f), (i * glm::two_pi<float>()) / lightColors.size(), { 0.f, -1.f,0.f });
			pointLight.transform.translation = glm::vec3(rotateLight * glm::vec4(-1.f, -1.f, -1.f, -1.f));
			gameObjects.emplace(pointLight.getId(), std::move(pointLight));
		}
	}

	void FirstApp::addGameObject(bve::BveGameObject gameObject)
	{
		gameObjects.emplace(gameObject.getId(), std::move(gameObject));
	}

}