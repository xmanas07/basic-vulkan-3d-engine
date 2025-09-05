#pragma  once

#include "bve_window.hpp"
#include "bve_device.hpp"
#include "bve_swap_chain.hpp"
#include "bve_model.hpp"

//std
#include <memory>
#include <vector>
#include <cassert>

namespace bve {
	class BveRenderer {

	public:

		BveRenderer(BveWindow& window, BveDevice& device);
		~BveRenderer();
		BveRenderer(const BveRenderer&) = delete;
		BveRenderer& operator=(const BveRenderer&) = delete;

		VkRenderPass getSwapchainRenderPass() const { return bveSwapChain->getRenderPass(); }
		float getAspectRatio() const { return bveSwapChain->extentAspectRatio(); }
		VkSampler getSampler() const { return sampler; }
		bool isFrameInProgress() const { return isFrameStarted; }

		VkCommandBuffer getCurrentCommandBuffer() const {
			assert(isFrameStarted && "Cannot get command buffer when frame not in progress");
			return commandBuffers[currentFrameIndex];
		}
		int getCurrentFrameIndex() const {
			assert(isFrameStarted && "Cannot get frame index when frame not in progress");
			return currentFrameIndex;
		}

		VkCommandBuffer beginFrame();
		void endFrame();
		void beginSwapChainRenderPass(VkCommandBuffer commandBuffer);
		void endSwapChainRenderPass(VkCommandBuffer commandBuffer);

	private:
		void createCommandBuffers();
		void freeCommandBuffers();
		void recreateSwapChain();
		void createSampler();

		BveWindow& bveWindow;
		BveDevice& bveDevice;
		VkSampler sampler;
		std::unique_ptr<BveSwapChain> bveSwapChain;
		std::vector<VkCommandBuffer> commandBuffers;

		uint32_t currentImageIndex;
		int currentFrameIndex = 0;
		bool isFrameStarted = false;
	};
} //namespace bve