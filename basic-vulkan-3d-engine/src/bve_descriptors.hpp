#pragma once

#include "bve_device.hpp"

// std
#include <memory>
#include <unordered_map>
#include <vector>

namespace bve {

    class BveDescriptorSetLayout {
    public:
        class Builder {
        public:
            Builder(BveDevice& bveDevice) : bveDevice{ bveDevice } {}

            Builder& addBinding(
                uint32_t binding,
                VkDescriptorType descriptorType,
                VkShaderStageFlags stageFlags,
                uint32_t count = 1);
            std::unique_ptr<BveDescriptorSetLayout> build() const;

        private:
            BveDevice& bveDevice;
            std::unordered_map<uint32_t, VkDescriptorSetLayoutBinding> bindings{};
        };

        BveDescriptorSetLayout(
            BveDevice& bveDevice, std::unordered_map<uint32_t, VkDescriptorSetLayoutBinding> bindings);
        ~BveDescriptorSetLayout();
        BveDescriptorSetLayout(const BveDescriptorSetLayout&) = delete;
        BveDescriptorSetLayout& operator=(const BveDescriptorSetLayout&) = delete;

        VkDescriptorSetLayout getDescriptorSetLayout() const { return descriptorSetLayout; }

    private:
        BveDevice& bveDevice;
        VkDescriptorSetLayout descriptorSetLayout;
        std::unordered_map<uint32_t, VkDescriptorSetLayoutBinding> bindings;

        friend class BveDescriptorWriter;
    };

    class BveDescriptorPool {
    public:
        class Builder {
        public:
            Builder(BveDevice& bveDevice) : bveDevice{ bveDevice } {}

            Builder& addPoolSize(VkDescriptorType descriptorType, uint32_t count);
            Builder& setPoolFlags(VkDescriptorPoolCreateFlags flags);
            Builder& setMaxSets(uint32_t count);
            std::unique_ptr<BveDescriptorPool> build() const;

        private:
            BveDevice& bveDevice;
            std::vector<VkDescriptorPoolSize> poolSizes{};
            uint32_t maxSets = 1000;
            VkDescriptorPoolCreateFlags poolFlags = 0;
        };

        BveDescriptorPool(
            BveDevice& bveDevice,
            uint32_t maxSets,
            VkDescriptorPoolCreateFlags poolFlags,
            const std::vector<VkDescriptorPoolSize>& poolSizes);
        ~BveDescriptorPool();
        BveDescriptorPool(const BveDescriptorPool&) = delete;
        BveDescriptorPool& operator=(const BveDescriptorPool&) = delete;

        bool allocateDescriptor(
            const VkDescriptorSetLayout descriptorSetLayout, VkDescriptorSet& descriptor) const;

        void freeDescriptors(std::vector<VkDescriptorSet>& descriptors) const;

        void resetPool();

    private:
        BveDevice& bveDevice;
        VkDescriptorPool descriptorPool;

        friend class BveDescriptorWriter;
    };

    class BveDescriptorWriter {
    public:
        BveDescriptorWriter(BveDescriptorSetLayout& setLayout, BveDescriptorPool& pool);

        BveDescriptorWriter& writeBuffer(uint32_t binding, VkDescriptorBufferInfo* bufferInfo);
        BveDescriptorWriter& writeImage(uint32_t binding, VkDescriptorImageInfo* imageInfo);

        bool build(VkDescriptorSet& set);
        void overwrite(VkDescriptorSet& set);

    private:
        BveDescriptorSetLayout& setLayout;
        BveDescriptorPool& pool;
        std::vector<VkWriteDescriptorSet> writes;
    };

}  // namespace bve