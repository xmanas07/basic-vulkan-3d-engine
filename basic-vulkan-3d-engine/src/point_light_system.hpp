#pragma  once

#include "bve_camera.hpp"
#include "bve_device.hpp"
#include "bve_game_object.hpp"
#include "bve_pipeline.hpp"
#include "bve_frame_info.hpp"

//std
#include <memory>

namespace bve {
	class PointLightSystem {

	public:
		PointLightSystem(BveDevice& device, VkRenderPass renderPass, VkDescriptorSetLayout globalSetLayout);
		~PointLightSystem();
		PointLightSystem(const PointLightSystem&) = delete;
		PointLightSystem& operator=(const PointLightSystem&) = delete;

		void update(FrameInfo& frameInfo, GlobalUbo& ubo);
		void render(FrameInfo& frameInfo);
	private:
		void createPipelineLayout(VkDescriptorSetLayout globalSetLayout);
		void createPipeline(VkRenderPass renderPass);

		BveDevice& bveDevice;

		std::unique_ptr<BvePipeline> bvePipeline;
		VkPipelineLayout pipelineLayout;
	};
} //namespace bve