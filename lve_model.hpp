#pragma once


#include "lve_device.hpp"

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>

#include <vector>


namespace lve {
	class LveModel {
	public:

		struct Vertex {
			glm::vec3 position;
			glm::vec3 color;
            // Add two vertices
            Vertex operator+(const Vertex& other) const {
                return { position + other.position, color + other.color };
            }

            // Subtract two vertices
            Vertex operator-(const Vertex& other) const {
                return { position - other.position, color - other.color };
            }

            // Multiply by scalar
            Vertex operator*(float scalar) const {
                return { position * scalar, color * scalar };
            }

            // Divide by scalar
            Vertex operator/(float scalar) const {
                return { position / scalar, color / scalar };
            }

            // Compound operators (optional, for +=, *=, etc.)
            Vertex& operator+=(const Vertex& other) {
                position += other.position;
                color += other.color;
                return *this;
            }

            Vertex& operator*=(float scalar) {
                position *= scalar;
                color *= scalar;
                return *this;
            }

		};
        struct Builder {
            std::vector<Vertex> vertices{};
            std::vector<uint32_t> indices{};
        };

		static std::vector<VkVertexInputBindingDescription> getBindingDescriptions();
		static std::vector<VkVertexInputAttributeDescription> getAttributeDescriptions();
		LveModel(LveDevice& device, const LveModel::Builder& builder);
		~LveModel();
		
		LveModel(const LveModel&) = delete;
		LveModel& operator=(const LveModel&) = delete;

		void bind(VkCommandBuffer commandBuffer);
		void draw(VkCommandBuffer commandBuffer);

	private:
		void createVertexBuffers(const std::vector<Vertex>& vertices);
		void createIndexBuffers(const std::vector<uint32_t>& indices);

		LveDevice& lveDevice;

		VkBuffer vertexBuffer;
		VkDeviceMemory vertexBufferMemory;
		uint32_t vertexCount;

        bool hasIndexBuffer = false;
		VkBuffer indexBuffer;
		VkDeviceMemory indexBufferMemory;
		uint32_t indexCount;
	};

}