#include "first_app.hpp"

#include "simple_render_system.hpp"

// libs
#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>
#include <glm/gtc/constants.hpp>

//std
#include <array>
#include <cassert>
#include <stdexcept>



namespace lve {

	FirstApp::FirstApp()
	{
		loadGameObjects();
	}
	FirstApp::~FirstApp()
	{
	}

	void drawSierpTriangle(const std::vector<LveModel::Vertex>& baseTriangle, int depth, std::vector<LveModel::Vertex>& vertices) {
		if (depth < 1) {
			vertices.insert(vertices.end(), baseTriangle.begin(), baseTriangle.end());
			return;
		}
		std::vector<LveModel::Vertex> triangles[4];

		glm::vec2 vert0 = baseTriangle.at(0).position;
		glm::vec2 vert1 = baseTriangle.at(1).position;
		glm::vec2 vert2 = baseTriangle.at(2).position;

		glm::vec3 cvert0 = baseTriangle.at(0).color;
		glm::vec3 cvert1 = baseTriangle.at(1).color;
		glm::vec3 cvert2 = baseTriangle.at(2).color;

		LveModel::Vertex vert01 = { { 0.5f * (vert0 + vert1)}, {0.5f * (cvert0 + cvert1) } };
		LveModel::Vertex vert12 = { { 0.5f * (vert1 + vert2)}, {0.5f * (cvert1 + cvert2) } };
		LveModel::Vertex vert02 = { { 0.5f * (vert0 + vert2)}, {0.5f * (cvert0 + cvert2) } };

		std::vector<LveModel::Vertex> t1{ baseTriangle[0], vert01, vert02 };
		std::vector<LveModel::Vertex> t2{ baseTriangle[1], vert12, vert01 };
		std::vector<LveModel::Vertex> t3{ baseTriangle[2], vert02, vert12 };


		drawSierpTriangle(t1, depth - 1, vertices);
		drawSierpTriangle(t2, depth - 1, vertices);
		drawSierpTriangle(t3, depth - 1, vertices);
	}
	void FirstApp::run() {
		SimpleRenderSystem simpleRenderSystem{ lveDevice, lveRenderer.getSwapchainRenderPass() };

		while (!lveWindow.shouldClose()) {
			glfwPollEvents();

			if (auto commandBuffer = lveRenderer.beginFrame()) {
				
				// begin offscreen shadow pass
				// render shadow casting objects
				// end offscreen shadow pass

				lveRenderer.beginSwapChainRenderPass(commandBuffer);
				simpleRenderSystem.renderGameObjects(commandBuffer, gameObjects);
				lveRenderer.endSwapChainRenderPass(commandBuffer);
				lveRenderer.endFrame();
			}
		}

		vkDeviceWaitIdle(lveDevice.device());
	}
	void FirstApp::loadGameObjects() {
		std::vector<LveModel::Vertex> baseTriangle{
			{{0.0f, 0.0f}, {1.0f,0.0f,0.0f}},
			{{.4f,  .8f}, {0.0f,1.0f,0.0f}},
			{{-.4f, .8f}, {0.0f,0.0f,1.0f}}
		};
		std::vector<LveModel::Vertex> vertices;
		drawSierpTriangle(baseTriangle, 0, vertices);

		auto lveModel = std::make_shared<LveModel>(lveDevice, vertices);
		auto triangle = LveGameObject::createGameObject();
		triangle.model = lveModel;
		triangle.color = { .1f,.8f,.1f };
		triangle.transform2d.translation.x = .0f;
		triangle.transform2d.scale = { 0.5f,.5f };
		triangle.transform2d.rotation = .25f * glm::two_pi<float>();

		gameObjects.push_back(std::move(triangle));
	}

}