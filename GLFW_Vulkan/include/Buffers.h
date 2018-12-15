#pragma once

#include <glm/mat4x4.hpp>
#include <array>

#include "Commands_Wrapper.h"

struct Vertex {
	glm::vec2 pos;
	glm::vec3 color;
};

const std::vector<Vertex> vertices = {
	{ { -0.5f, -0.5f }, { 1.0f, 0.0f, 0.0f } },
	{ { 0.5f, -0.5f }, { 0.0f, 1.0f, 0.0f } },
	{ { 0.5f, 0.5f }, { 0.0f, 0.0f, 1.0f } },
	{ { -0.5f, 0.5f }, { 1.0f, 1.0f, 1.0f } }
};

const std::vector<uint16_t> indices = {
	0, 1, 2, 2, 3, 0
};

struct UniformBufferObject {
	glm::mat4 model;
	glm::mat4 view;
	glm::mat4 proj;
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
	VkDevice								logicalDevice;
	VkPhysicalDevice						physicalDevice;

	VkBuffer								vertexBuffer;
	VkDeviceMemory							vertexBufferMemory;
	VkBuffer								indexBuffer;
	VkDeviceMemory							indexBufferMemory;

	VkQueue									graphicsQueue;

	VkDescriptorSetLayout					descriptorSetLayout;
	VkDescriptorPool						descriptorPool;
	std::vector<VkDescriptorSet>			descriptorSets;

	std::vector<VkBuffer>					uniformBuffers;
	std::vector<VkDeviceMemory>				uniformBuffersMemory;
	std::vector<VkImage>					swapImages;

public:
	Buffer_Wrapper();
	~Buffer_Wrapper();
	
	void BufferInit(VkDevice logDevice, VkPhysicalDevice physDevice, VkQueue gQueue, std::vector<VkImage> swapChainImages);

	void CreateDescriptorSetLayout();

	void CreateDescriptorPool();

	void CreateDescriptorSets();

	void CreateVertexBuffers(VkCommandPool cmd);

	void CreateIndexBuffers(VkCommandPool cmd);

	void CreateUniformBuffers();

	void CreateBuffer(VkDeviceSize size, VkBufferUsageFlags usage, VkMemoryPropertyFlags properties, VkBuffer& buffer, VkDeviceMemory& bufferMemory);

	void CopyBuffer(VkBuffer srcBuffer, VkBuffer dstBuffer, VkDeviceSize size, VkCommandPool pool);

	VkBuffer GetVertexBuffer(){ return vertexBuffer; }
	VkDeviceMemory GetVertexBufferMemory() { return vertexBufferMemory; }
	VkBuffer GetIndexBuffer(){ return indexBuffer; }
	std::vector<VkBuffer> GetUniformBuffers() { return uniformBuffers; }
	std::vector<VkDeviceMemory> GetUniformBuffersMemory() { return uniformBuffersMemory; }
	std::vector<VkDescriptorSet> GetDescriptorSets() { return descriptorSets; }
	VkDescriptorSetLayout GetDescriptorSetLayout(){ return descriptorSetLayout; }

	uint32_t FindMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags properties);
};

