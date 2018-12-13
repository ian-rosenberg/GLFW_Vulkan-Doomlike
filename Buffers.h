#pragma once

#include <glm/glm.hpp>
#include <array>
#include <vector>
#include <vulkan/vulkan.h>

#include "Commands_Wrapper.h"

struct Vertex {
	glm::vec2 pos;
	glm::vec3 color;
};

const std::vector<Vertex> vertices = {
	{ { 0.0f, -0.5f }, { 1.0f, 0.0f, 0.0f } },
	{ { 0.5f, 0.5f }, { 0.0f, 1.0f, 0.0f } },
	{ { -0.75f, 0.5f }, { 1.0f, 1.0f, 1.0f } }
};

static VkVertexInputBindingDescription GetBindingDescription()
{
	VkVertexInputBindingDescription bindingDescription = {};

	bindingDescription.binding = 0;
	bindingDescription.stride = sizeof(Vertex);
	bindingDescription.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;

	return bindingDescription;
}

static std::array<VkVertexInputAttributeDescription, 2> GetAttributeDescriptions()
{
	std::array<VkVertexInputAttributeDescription, 2> attributeDescriptions = {};

	attributeDescriptions[0].binding = 0;
	attributeDescriptions[0].location = 0;
	attributeDescriptions[0].format = VK_FORMAT_R32G32_SFLOAT;
	attributeDescriptions[0].offset = offsetof(Vertex, pos);
	attributeDescriptions[1].binding = 0;
	attributeDescriptions[1].location = 1;
	attributeDescriptions[1].format = VK_FORMAT_R32G32B32_SFLOAT;
	attributeDescriptions[1].offset = offsetof(Vertex, color);

	return attributeDescriptions;
}

class Buffer_Wrapper
{
private:
	VkDevice				logicalDevice;
	VkPhysicalDevice		physicalDevice;

	VkBuffer				vertexBuffer;
	VkDeviceMemory			vertexBufferMemory;

	VkQueue					graphicsQueue;

public:
	void BufferInit(VkDevice logDevice, VkPhysicalDevice physDevice, VkQueue gQueue);

	void CreateVertexBuffers(Command* cmd);

	void CreateBuffer(VkDeviceSize size, VkBufferUsageFlags usage, VkMemoryPropertyFlags properties, VkBuffer& buffer, VkDeviceMemory& bufferMemory);

	void CopyBuffer(VkBuffer srcBuffer, VkBuffer dstBuffer, VkDeviceSize size, VkCommandPool pool);

	VkBuffer GetVertexBuffer(){ return vertexBuffer; }
	VkDeviceMemory GetVertexBufferMemory() { return vertexBufferMemory; }

	uint32_t FindMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags properties);
};

