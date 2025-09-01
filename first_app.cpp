#include "first_app.hpp"

#include "keyboard_movement_controller.hpp"
#include "lve_camera.hpp"
#include "simple_render_system.hpp"
#include "point_light_system.hpp"
#include "lve_buffer.hpp"

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

namespace lve {
	

	FirstApp::FirstApp()
	{
		globalPool = LveDescriptorPool::Builder(lveDevice)
			.setMaxSets(LveSwapChain::MAX_FRAMES_IN_FLIGHT)
			.addPoolSize(VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, LveSwapChain::MAX_FRAMES_IN_FLIGHT)
			.build();
		loadGameObjects();
	}
	FirstApp::~FirstApp()
	{
	}

	void FirstApp::run() {

		std::vector<std::unique_ptr<LveBuffer>> uboBuffers(LveSwapChain::MAX_FRAMES_IN_FLIGHT);
		for (int i = 0; i < uboBuffers.size(); i++) {
			uboBuffers[i] = std::make_unique<LveBuffer>(
				lveDevice,
				sizeof(GlobalUbo),
				1,
				VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
				VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT);
			uboBuffers[i]->map();
		}

		auto globalSetLayout = LveDescriptorSetLayout::Builder(lveDevice)
			.addBinding(0, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, VK_SHADER_STAGE_ALL_GRAPHICS)
			.build();

		std::vector<VkDescriptorSet> globalDescriptorSets(LveSwapChain::MAX_FRAMES_IN_FLIGHT);
		for (int i = 0; i < globalDescriptorSets.size(); i++) {
			auto bufferInfo = uboBuffers[i]->descriptorInfo();
			LveDescriptorWriter(*globalSetLayout, *globalPool)
				.writeBuffer(0, &bufferInfo)
				.build(globalDescriptorSets[i]);
		}

		

		SimpleRenderSystem simpleRenderSystem{ lveDevice, lveRenderer.getSwapchainRenderPass(), globalSetLayout->getDescriptorSetLayout()};
		PointLightSystem pointLightSystem{ lveDevice, lveRenderer.getSwapchainRenderPass(), globalSetLayout->getDescriptorSetLayout()};
		LveCamera camera{};
		camera.setViewTarget(glm::vec3(-1.f, -2.f, 2.f), glm::vec3(0.f, 0.f, 2.5f));

		auto viewerObject = LveGameObject::createGameObject();
		KeyboardMovementController cameraController{};

		auto currentTime = std::chrono::high_resolution_clock::now();

		while (!lveWindow.shouldClose()) {
			glfwPollEvents();

			auto newTime = std::chrono::high_resolution_clock::now();
			float frameTime = std::chrono::duration<float, std::chrono::seconds::period>(newTime - currentTime).count();
			currentTime = newTime;

			frameTime = glm::min(frameTime, FirstApp::MAX_ALOWABLE_FRAMETIME);

			cameraController.moveInPlaneXZ(lveWindow.getGLFWwindow(), frameTime, viewerObject);
			camera.setViewYXZ(viewerObject.transform.translation, viewerObject.transform.rotation);

			float aspect = lveRenderer.getAspectRatio();
			camera.setPerspectiveProjection(glm::radians(50.f), aspect, .1f, 10.f);

			if (auto commandBuffer = lveRenderer.beginFrame()) {
				int frameIndex = lveRenderer.getCurrentFrameIndex();
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
				lveRenderer.beginSwapChainRenderPass(commandBuffer);
				simpleRenderSystem.renderGameObjects(frameInfo);
				pointLightSystem.render(frameInfo);
				lveRenderer.endSwapChainRenderPass(commandBuffer);
				lveRenderer.endFrame();
			}
		}

		vkDeviceWaitIdle(lveDevice.device());
	}
	
	std::unique_ptr<LveModel> createSierpPyramidModel(LveDevice& device, const std::array<LveModel::Vertex, 4>& peaks, int depth) {
		//calculate normal for each face - clockwise
		std::array<glm::vec3, 4> faceNormals;
		faceNormals[0] = glm::normalize(glm::cross(peaks[1].position - peaks[0].position, peaks[2].position - peaks[0].position));
		faceNormals[1] = glm::normalize(glm::cross(peaks[2].position - peaks[0].position, peaks[3].position - peaks[0].position));
		faceNormals[2] = glm::normalize(glm::cross(peaks[3].position - peaks[0].position, peaks[1].position - peaks[0].position));
		faceNormals[3] = glm::normalize(glm::cross(peaks[3].position - peaks[1].position, peaks[2].position - peaks[1].position));
		
		
		
		int nPyramids = static_cast<uint32_t>(std::pow(4, depth));
		std::vector<std::array<LveModel::Vertex, 4>> outPyramidVector;
		outPyramidVector.reserve(nPyramids);
		outPyramidVector.emplace_back(peaks);

		std::vector<std::array<LveModel::Vertex, 4>> helpPyramidVector;
		helpPyramidVector.reserve(nPyramids/3);

		// subdivide into 4 smaller pyramids depth times
		for (size_t i = 0; i < depth; i++)
		{
			// divide each pyramid in outPyramidVector into 4 smaller
			for (const std::array<LveModel::Vertex, 4>&pyramidVertexes : outPyramidVector) {
				// pyramid 1
				helpPyramidVector.push_back({
					pyramidVertexes[0],
					(pyramidVertexes[0] + pyramidVertexes[1]) / 2.f,
					(pyramidVertexes[0] + pyramidVertexes[2]) / 2.f,
					(pyramidVertexes[0] + pyramidVertexes[3]) / 2.f 
					});
				// pyramid 2
				helpPyramidVector.push_back({
					(pyramidVertexes[1] + pyramidVertexes[0]) / 2.f,
					pyramidVertexes[1],
					(pyramidVertexes[1] + pyramidVertexes[2]) / 2.f,
					(pyramidVertexes[1] + pyramidVertexes[3]) / 2.f 
					});
				// pyramid 3
				helpPyramidVector.push_back({
					(pyramidVertexes[2] + pyramidVertexes[0]) / 2.f,
					(pyramidVertexes[2] + pyramidVertexes[1]) / 2.f,
					pyramidVertexes[2],
					(pyramidVertexes[2] + pyramidVertexes[3]) / 2.f 
					});
				// pyramid 4
				helpPyramidVector.push_back({
					(pyramidVertexes[3] + pyramidVertexes[0]) / 2.f,
					(pyramidVertexes[3] + pyramidVertexes[1]) / 2.f,
					(pyramidVertexes[3] + pyramidVertexes[2]) / 2.f, 
					pyramidVertexes[3]
					});			
			}
			outPyramidVector = helpPyramidVector;
			helpPyramidVector.clear();
		}

		LveModel::Builder modelBuilder{};
		modelBuilder.vertices.reserve(nPyramids*4);
		modelBuilder.indices.reserve(nPyramids*4*3);

		int pyramidIdx = 0;
		for (const std::array<LveModel::Vertex, 4>& pyramidVertexes : outPyramidVector) {
			
			// create triangles indices
			// triangle face 1
			modelBuilder.vertices.emplace_back(pyramidVertexes[0]);
			modelBuilder.vertices.back().normal = faceNormals[0];
			modelBuilder.vertices.emplace_back(pyramidVertexes[1]);
			modelBuilder.vertices.back().normal = faceNormals[0];
			modelBuilder.vertices.emplace_back(pyramidVertexes[2]);
			modelBuilder.vertices.back().normal = faceNormals[0];
			
			// triangle face 2
			modelBuilder.vertices.emplace_back(pyramidVertexes[0]);
			modelBuilder.vertices.back().normal = faceNormals[1];
			modelBuilder.vertices.emplace_back(pyramidVertexes[2]);
			modelBuilder.vertices.back().normal = faceNormals[1];
			modelBuilder.vertices.emplace_back(pyramidVertexes[3]);
			modelBuilder.vertices.back().normal = faceNormals[1];
			
			// triangle face 3
			modelBuilder.vertices.emplace_back(pyramidVertexes[0]);
			modelBuilder.vertices.back().normal = faceNormals[2];
			modelBuilder.vertices.emplace_back(pyramidVertexes[3]);
			modelBuilder.vertices.back().normal = faceNormals[2];
			modelBuilder.vertices.emplace_back(pyramidVertexes[1]);
			modelBuilder.vertices.back().normal = faceNormals[2];
			
			// triangle face 4
			modelBuilder.vertices.emplace_back(pyramidVertexes[3]);
			modelBuilder.vertices.back().normal = faceNormals[3];
			modelBuilder.vertices.emplace_back(pyramidVertexes[2]);
			modelBuilder.vertices.back().normal = faceNormals[3];
			modelBuilder.vertices.emplace_back(pyramidVertexes[1]);
			modelBuilder.vertices.back().normal = faceNormals[3];
		}

		return std::make_unique<LveModel>(device, modelBuilder);
	}
	void FirstApp::loadGameObjects() {
		std::shared_ptr<LveModel> lveModel = LveModel::createModelFromFile(lveDevice, "models/smooth_vase.obj");
		auto gameObject = LveGameObject::createGameObject();
		gameObject.model = lveModel;
		gameObject.transform.translation = { .0f,0.5f,2.5f };
		gameObject.transform.scale = { 1.f,.2f,1.f };
		gameObjects.emplace(gameObject.getId(), std::move(gameObject));

		lveModel = LveModel::createModelFromFile(lveDevice, "models/smooth_brown_rat.obj");
		auto rat = LveGameObject::createGameObject();
		rat.model = lveModel;
		rat.transform.translation = { .0f,-0.0f,2.5f };
		rat.transform.scale = { .5f,.5f,.5f };
		rat.transform.rotation = {0.f, 0.f, -glm::half_pi<float>()};
		gameObjects.emplace(rat.getId(), std::move(rat));

		std::array<LveModel::Vertex, 4> pyramidVertices;
		pyramidVertices[0] = { {.0f,-.5f,.5f},{1.f,1.f,1.f} };
		pyramidVertices[1] = { {.0f,.5f,-.5f},{1.f,0.f,0.f} };
		pyramidVertices[2] = { {.5f,.5f,.5f},{0.f,1.f,0.f} };
		pyramidVertices[3] = { {-.5f,.5f,.5f},{0.f,0.f,1.f} };
		lveModel = createSierpPyramidModel(lveDevice, pyramidVertices, 3);
		auto sierpPyramid = LveGameObject::createGameObject();
		sierpPyramid.model = lveModel;
		sierpPyramid.transform.translation = { 1.5f,-.5f,2.5f };
		sierpPyramid.transform.scale = { .5f,.5f,.5f };
		gameObjects.emplace(sierpPyramid.getId(), std::move(sierpPyramid));
		
		lveModel = LveModel::createModelFromFile(lveDevice, "models/quad.obj");
		auto floor = LveGameObject::createGameObject();
		floor.model = lveModel;
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
			auto pointLight = LveGameObject::makePointLight(0.2f);
			pointLight.color = lightColors[i];
			auto rotateLight = glm::rotate(glm::mat4(1.f), (i * glm::two_pi<float>()) / lightColors.size(), { 0.f, -1.f,0.f });
			pointLight.transform.translation = glm::vec3(rotateLight * glm::vec4(-1.f, -1.f, -1.f, -1.f));
			gameObjects.emplace(pointLight.getId(), std::move(pointLight));
		}
	}

}