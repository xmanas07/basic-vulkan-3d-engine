#pragma  once

#include "bve_camera.hpp"
#include "bve_device.hpp"
#include "bve_game_object.hpp"
#include "bve_pipeline.hpp"
#include "bve_frame_info.hpp"

//std
#include <memory>
#include <vector>

namespace bve {
	class SimpleRenderSystem {

	public:
		SimpleRenderSystem(BveDevice& device, VkRenderPass renderPass, VkDescriptorSetLayout globalSetLayout);
		~SimpleRenderSystem();
		SimpleRenderSystem(const SimpleRenderSystem&) = delete;
		SimpleRenderSystem& operator=(const SimpleRenderSystem&) = delete;
		void renderGameObjects(FrameInfo& frameInfo);

	private:
		void createPipelineLayout(VkDescriptorSetLayout globalSetLayout);
		void createPipeline(VkRenderPass renderPass);

		BveDevice& bveDevice;

		std::unique_ptr<BvePipeline> bvePipeline;
		VkPipelineLayout pipelineLayout;
	};
} //namespace bve