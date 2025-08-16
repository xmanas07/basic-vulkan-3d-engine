#include "first_app.hpp"


//std
#include <stdexcept>
#include <array>
#include <glm/vec2.hpp>
#include <glm/vec3.hpp>



namespace lve {
	FirstApp::FirstApp()
	{
		loadModels();
		createPipelineLayout();
		recreateSwapChain();
		createCommandBuffers();
	}
	FirstApp::~FirstApp()
	{
		vkDestroyPipelineLayout(lveDevice.device(), pipelineLayout, nullptr);
	}

	void FirstApp::run() {
		while (!lveWindow.shouldClose()) {
			glfwPollEvents();
			drawFrame();
		}

		vkDeviceWaitIdle(lveDevice.device());
	}

	 void drawSierpTriangle(const std::vector<LveModel::Vertex> baseTriangle, int depth,std::vector<LveModel::Vertex>& vertices) {
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

	void FirstApp::loadModels() {
		std::vector<LveModel::Vertex> baseTriangle{
			{{0.0f, -0.5f}, {1.0f,0.0f,0.0f}},
			{{0.5f,  0.5f}, {0.0f,1.0f,0.0f}},
			{{-0.5f, 0.5f}, {0.0f,0.0f,1.0f}}
		};
		std::vector<LveModel::Vertex> vertices;
		//drawSierpTriangle(baseTriangle, 4, vertices);

		lveModel = std::make_unique<LveModel>(lveDevice, baseTriangle);
	}

	void FirstApp::createPipelineLayout()
	{
		VkPipelineLayoutCreateInfo pipelineLayoutInfo{};
		pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
		pipelineLayoutInfo.setLayoutCount = 0;
		pipelineLayoutInfo.pSetLayouts = nullptr;
		pipelineLayoutInfo.pushConstantRangeCount = 0;
		pipelineLayoutInfo.pPushConstantRanges = nullptr;
		if (vkCreatePipelineLayout(lveDevice.device(), &pipelineLayoutInfo, nullptr, &pipelineLayout) != VK_SUCCESS)
		{
			throw std::runtime_error("failed to create pipeline layout!");
		}
	}
	void FirstApp::createPipeline()
	{

		PipelineConfigInfo pipelineConfig{};
		LvePipeline::defaultPipelineConfigInfo(pipelineConfig, lveSwapChain->width(), lveSwapChain->height());
		pipelineConfig.renderPass = lveSwapChain->getRenderPass();
		pipelineConfig.pipelineLayout = pipelineLayout;
		lvePipeline = std::make_unique<LvePipeline>(lveDevice, "shaders/simple_shader.vert.spv", "shaders/simple_shader.frag.spv", pipelineConfig);
	}
	
	void FirstApp::recreateSwapChain()
	{
		auto extent = lveWindow.getExtent();
		while (extent.width || extent.height == 0) {
			extent = lveWindow.getExtent();
			glfwWaitEvents();
		}

		vkDeviceWaitIdle(lveDevice.device());
		lveSwapChain = std::make_unique<LveSwapChain>(lveDevice, extent);
		createPipeline();
	}
	void FirstApp::createCommandBuffers()
	{
		commandBuffers.resize(lveSwapChain->imageCount());

		VkCommandBufferAllocateInfo allocInfo{};
		allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
		allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
		allocInfo.commandPool = lveDevice.getCommandPool();
		allocInfo.commandBufferCount = static_cast<uint32_t>(commandBuffers.size());

		if (vkAllocateCommandBuffers(lveDevice.device(), &allocInfo, commandBuffers.data()))
		{
			throw std::runtime_error("failed to allocate command buffers!");
		}
	}
	void FirstApp::recordCommandBuffer(int imageIndex) {
		VkCommandBufferBeginInfo beginInfo{};
		beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;

		if (vkBeginCommandBuffer(commandBuffers[imageIndex], &beginInfo) != VK_SUCCESS) {
			throw std::runtime_error("failed to begin recording the buffer!");
		}

		VkRenderPassBeginInfo renderPassInfo{};
		renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
		renderPassInfo.renderPass = lveSwapChain->getRenderPass();
		renderPassInfo.framebuffer = lveSwapChain->getFrameBuffer(imageIndex);

		renderPassInfo.renderArea.offset = { 0, 0 };
		renderPassInfo.renderArea.extent = lveSwapChain->getSwapChainExtent();

		std::array<VkClearValue, 2> clearValues{};
		clearValues[0].color = { 0.1f, 0.1f, 0.1f, 1.0f };
		clearValues[1].depthStencil = { 1.0f, 0 };
		renderPassInfo.clearValueCount = static_cast<uint32_t>(clearValues.size());
		renderPassInfo.pClearValues = clearValues.data();

		vkCmdBeginRenderPass(commandBuffers[imageIndex], &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);

		lvePipeline->bind(commandBuffers[imageIndex]);
		lveModel->bind(commandBuffers[imageIndex]);
		lveModel->draw(commandBuffers[imageIndex]);

		vkCmdEndRenderPass(commandBuffers[imageIndex]);
		if (vkEndCommandBuffer(commandBuffers[imageIndex]) != VK_SUCCESS) {
			throw std::runtime_error("failed to record command buffer!");
		}
	}

	void FirstApp::drawFrame()
	{
		uint32_t imageIndex;
		auto result = lveSwapChain->acquireNextImage(&imageIndex);
		if (result == VK_ERROR_OUT_OF_DATE_KHR) {
			recreateSwapChain();
			return;
		}

		if (result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR) {
			throw std::runtime_error("failed to acquire swap chain image!");
		}
		
		recordCommandBuffer(imageIndex);
		result = lveSwapChain->submitCommandBuffers(&commandBuffers[imageIndex], &imageIndex);
		if (result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR || lveWindow.wasWindowResized()) {
			lveWindow.resetWindowResizedFlag();
			recreateSwapChain();
			return;
		}
		if (result != VK_SUCCESS) {
			throw std::runtime_error("failed to present swap chain image!");
		}
	}
}