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
			gameObjects[2].transform.rotation.y += 1.f * frameTime;
			gameObjects[2].transform.rotation.x += 1.f * frameTime;
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
		gameObject.transform.translation = { .0f,.0f,2.5f };
		gameObject.transform.scale = { 1.f,.2f,1.f };
		gameObjects.push_back(std::move(gameObject));

		std::shared_ptr<LveModel> lveModel2 = LveModel::createModelFromFile(lveDevice, "models/rat.obj");
		auto gameObject2 = LveGameObject::createGameObject();
		gameObject2.model = lveModel2;
		gameObject2.transform.translation = { .0f,-2.0f,2.5f };
		gameObject2.transform.scale = { 1.f,1.f,1.f };
		gameObjects.push_back(std::move(gameObject2));

		std::array<LveModel::Vertex, 4> pyramidVertices;
		pyramidVertices[0] = { {.0f,-.5f,.5f},{1.f,1.f,1.f} };
		pyramidVertices[1] = { {.0f,.5f,-.5f},{1.f,0.f,0.f} };
		pyramidVertices[2] = { {.5f,.5f,.5f},{0.f,1.f,0.f} };
		pyramidVertices[3] = { {-.5f,.5f,.5f},{0.f,0.f,1.f} };
		std::shared_ptr<LveModel> lvePyramidModel = createSierpPyramidModel(lveDevice, pyramidVertices, 3);
		auto sierpPyramid = LveGameObject::createGameObject();
		sierpPyramid.model = lvePyramidModel;
		sierpPyramid.transform.translation = { .0f,.50f,2.5f };
		sierpPyramid.transform.scale = { .5f,.5f,.5f };
		gameObjects.push_back(std::move(sierpPyramid));
	}

}