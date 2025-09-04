#pragma once

#include <vulkan/vulkan_core.h>
#include "bve_device.hpp"
#include <string>



namespace bve {
    class BveTexture {
    public:
        BveTexture(BveDevice& device, const std::string& filepath);

        BveTexture(const BveTexture&) = delete;
        BveTexture &operator= (const BveTexture&) = delete;
        BveTexture(BveTexture&&) = delete;
        BveTexture &operator= (BveTexture&&) = delete;
        ~BveTexture();

        VkSampler getSampler() { return sampler; }
        VkImageView getImageView() { return imageView; }
        VkImageLayout getImageLayout() { return imageLayout; }
    private:
        void transitionImageLayout(VkImageLayout oldLayout, VkImageLayout newLayout);
        void generateMipmaps();

        uint32_t width, height, mipLevels;

        BveDevice& bveDevice;
        VkImage image;
        VkDeviceMemory imageMemory;
        VkImageView imageView;
        VkSampler sampler;
        VkFormat imageFormat;
        VkImageLayout imageLayout;
    };

}