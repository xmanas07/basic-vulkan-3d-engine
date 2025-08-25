#include "first_app.hpp"

#include "keyboard_movement_controller.hpp"
#include "lve_camera.hpp"
#include "simple_render_system.hpp"

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
		loadGameObjects();
	}
	FirstApp::~FirstApp()
	{
	}

	void FirstApp::run() {
		SimpleRenderSystem simpleRenderSystem{ lveDevice, lveRenderer.getSwapchainRenderPass() };
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
				
				// begin offscreen shadow pass
				// render shadow casting objects
				// end offscreen shadow pass

				lveRenderer.beginSwapChainRenderPass(commandBuffer);
				simpleRenderSystem.renderGameObjects(commandBuffer, gameObjects, camera);
				lveRenderer.endSwapChainRenderPass(commandBuffer);
				lveRenderer.endFrame();
			}
		}

		vkDeviceWaitIdle(lveDevice.device());
	}
	
	std::unique_ptr<LveModel> createCubeModel(LveDevice& device, glm::vec3 offset) {
		std::vector<LveModel::Vertex> vertices{

			// left face (white)
			{{-.5f, -.5f, -.5f}, {.9f, .9f, .9f}},
			{{-.5f, .5f, .5f}, {.9f, .9f, .9f}},
			{{-.5f, -.5f, .5f}, {.9f, .9f, .9f}},
			{{-.5f, -.5f, -.5f}, {.9f, .9f, .9f}},
			{{-.5f, .5f, -.5f}, {.9f, .9f, .9f}},
			{{-.5f, .5f, .5f}, {.9f, .9f, .9f}},

			// right face (yellow)
			{{.5f, -.5f, -.5f}, {.8f, .8f, .1f}},
			{{.5f, .5f, .5f}, {.8f, .8f, .1f}},
			{{.5f, -.5f, .5f}, {.8f, .8f, .1f}},
			{{.5f, -.5f, -.5f}, {.8f, .8f, .1f}},
			{{.5f, .5f, -.5f}, {.8f, .8f, .1f}},
			{{.5f, .5f, .5f}, {.8f, .8f, .1f}},

			// top face (orange, remember y axis points down)
			{{-.5f, -.5f, -.5f}, {.9f, .6f, .1f}},
			{{.5f, -.5f, .5f}, {.9f, .6f, .1f}},
			{{-.5f, -.5f, .5f}, {.9f, .6f, .1f}},
			{{-.5f, -.5f, -.5f}, {.9f, .6f, .1f}},
			{{.5f, -.5f, -.5f}, {.9f, .6f, .1f}},
			{{.5f, -.5f, .5f}, {.9f, .6f, .1f}},

			// bottom face (red)
			{{-.5f, .5f, -.5f}, {.8f, .1f, .1f}},
			{{.5f, .5f, .5f}, {.8f, .1f, .1f}},
			{{-.5f, .5f, .5f}, {.8f, .1f, .1f}},
			{{-.5f, .5f, -.5f}, {.8f, .1f, .1f}},
			{{.5f, .5f, -.5f}, {.8f, .1f, .1f}},
			{{.5f, .5f, .5f}, {.8f, .1f, .1f}},

			// nose face (blue)
			{{-.5f, -.5f, 0.5f}, {.1f, .1f, .8f}},
			{{.5f, .5f, 0.5f}, {.1f, .1f, .8f}},
			{{-.5f, .5f, 0.5f}, {.1f, .1f, .8f}},
			{{-.5f, -.5f, 0.5f}, {.1f, .1f, .8f}},
			{{.5f, -.5f, 0.5f}, {.1f, .1f, .8f}},
			{{.5f, .5f, 0.5f}, {.1f, .1f, .8f}},

			// tail face (green)
			{{-.5f, -.5f, -0.5f}, {.1f, .8f, .1f}},
			{{.5f, .5f, -0.5f}, {.1f, .8f, .1f}},
			{{-.5f, .5f, -0.5f}, {.1f, .8f, .1f}},
			{{-.5f, -.5f, -0.5f}, {.1f, .8f, .1f}},
			{{.5f, -.5f, -0.5f}, {.1f, .8f, .1f}},
			{{.5f, .5f, -0.5f}, {.1f, .8f, .1f}},

		};
		for (auto& v : vertices) {
			v.position += offset;
		}
		return std::make_unique<LveModel>(device, vertices);
	}
	std::unique_ptr<LveModel> createSierpPyramidModel(LveDevice& device, const std::array<LveModel::Vertex, 4>& peaks, int depth) {
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
				helpPyramidVector.push_back({pyramidVertexes[0],
				(pyramidVertexes[0] + pyramidVertexes[1]) / 2.f,
				(pyramidVertexes[0] + pyramidVertexes[2]) / 2.f,
				(pyramidVertexes[0] + pyramidVertexes[3]) / 2.f });
				// pyramid 2
				helpPyramidVector.push_back({pyramidVertexes[1],
				(pyramidVertexes[1] + pyramidVertexes[0]) / 2.f,
				(pyramidVertexes[1] + pyramidVertexes[2]) / 2.f,
				(pyramidVertexes[1] + pyramidVertexes[3]) / 2.f });
				// pyramid 3
				helpPyramidVector.push_back({pyramidVertexes[2],
				(pyramidVertexes[2] + pyramidVertexes[0]) / 2.f,
				(pyramidVertexes[2] + pyramidVertexes[1]) / 2.f,
				(pyramidVertexes[2] + pyramidVertexes[3]) / 2.f });
				// pyramid 4
				helpPyramidVector.push_back({pyramidVertexes[3],
				(pyramidVertexes[3] + pyramidVertexes[0]) / 2.f,
				(pyramidVertexes[3] + pyramidVertexes[1]) / 2.f,
				(pyramidVertexes[3] + pyramidVertexes[2]) / 2.f });			
			}
			outPyramidVector = helpPyramidVector;
			helpPyramidVector.clear();
		}

		std::vector<LveModel::Vertex> vertices;
		vertices.reserve(nPyramids*4*3);
		for (const std::array<LveModel::Vertex, 4>& pyramidVertexes : outPyramidVector) {
			// create triangles vertices
			// triangle face 1
			vertices.emplace_back(pyramidVertexes[0]);
			vertices.emplace_back(pyramidVertexes[1]);
			vertices.emplace_back(pyramidVertexes[2]);
			// triangle face 2
			vertices.emplace_back(pyramidVertexes[0]);
			vertices.emplace_back(pyramidVertexes[2]);
			vertices.emplace_back(pyramidVertexes[3]);
			// triangle face 3
			vertices.emplace_back(pyramidVertexes[0]);
			vertices.emplace_back(pyramidVertexes[3]);
			vertices.emplace_back(pyramidVertexes[1]);
			// triangle face 4
			vertices.emplace_back(pyramidVertexes[1]);
			vertices.emplace_back(pyramidVertexes[2]);
			vertices.emplace_back(pyramidVertexes[3]);
		}

		return std::make_unique<LveModel>(device, vertices);
	}
	void FirstApp::loadGameObjects() {
		std::shared_ptr<LveModel> lveModel = createCubeModel(lveDevice, { .0f,.0f,.0f });
		auto cube = LveGameObject::createGameObject();
		cube.model = lveModel;
		cube.transform.translation = { .0f,.0f,2.5f };
		cube.transform.scale = { .1f,.1f,.1f };
		gameObjects.push_back(std::move(cube));

		std::array<LveModel::Vertex, 4> pyramidVertices;
		pyramidVertices[0] = { {.0f,-.5f,.5f},{1.f,1.f,1.f} };
		pyramidVertices[1] = { {.0f,.5f,-.5f},{1.f,0.f,0.f} };
		pyramidVertices[2] = { {.5f,.5f,.5f},{0.f,1.f,0.f} };
		pyramidVertices[3] = { {-.5f,.5f,.5f},{0.f,0.f,1.f} };
		std::shared_ptr<LveModel> lvePyramidModel = createSierpPyramidModel(lveDevice, pyramidVertices, 3);
		auto sierpPyramid = LveGameObject::createGameObject();
		sierpPyramid.model = lvePyramidModel;
		sierpPyramid.transform.translation = { .0f,.0f,2.5f };
		sierpPyramid.transform.scale = { .5f,.5f,.5f };
		gameObjects.push_back(std::move(sierpPyramid));
	}

}