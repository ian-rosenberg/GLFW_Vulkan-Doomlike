#pragma once

#include <glm/mat4x4.hpp>
#include <array>

#include "Commands_Wrapper.h"

struct Vertex {
	glm::vec3 pos;
	glm::vec3 color;
	glm::vec2 texCoord;

	bool operator==(const Vertex& other) const {
		return pos == other.pos && color == other.color && texCoord == other.texCoord;
	}
};


/*const std::vector<Vertex> vertices = {
	{ { -0.5f, -0.5f, 0.0f }, { 1.0f, 0.0f, 0.0f }, { 0.0f, 0.0f } },
	{ { 0.5f, -0.5f, 0.0f }, { 0.0f, 1.0f, 0.0f }, { 1.0f, 0.0f } },
	{ { 0.5f, 0.5f, 0.0f }, { 0.0f, 0.0f, 1.0f }, { 1.0f, 1.0f } },
	{ { -0.5f, 0.5f, 0.0f }, { 1.0f, 1.0f, 1.0f }, { 0.0f, 1.0f } },

	{ { -0.5f, -0.5f, -0.5f }, { 1.0f, 0.0f, 0.0f }, { 0.0f, 0.0f } },
	{ { 0.5f, -0.5f, -0.5f }, { 0.0f, 1.0f, 0.0f }, { 1.0f, 0.0f } },
	{ { 0.5f, 0.5f, -0.5f }, { 0.0f, 0.0f, 1.0f }, { 1.0f, 1.0f } },
	{ { -0.5f, 0.5f, -0.5f }, { 1.0f, 1.0f, 1.0f }, { 0.0f, 1.0f } }
};

const std::vector<uint16_t> indices = {
	0, 1, 2, 2, 3, 0,
	4, 5, 6, 6, 7, 4
};*/

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

static std::array<VkVertexInputAttributeDescription, 3> GetAttributeDescriptions() 
{
	std::array<VkVertexInputAttributeDescription, 3> attributeDescriptions = {};

	attributeDescriptions[0].binding = 0;
	attributeDescriptions[0].location = 0;
	attributeDescriptions[0].format = VK_FORMAT_R32G32B32_SFLOAT;
	attributeDescriptions[0].offset = offsetof(Vertex, pos);

	attributeDescriptions[1].binding = 0;
	attributeDescriptions[1].location = 1;
	attributeDescriptions[1].format = VK_FORMAT_R32G32B32_SFLOAT;
	attributeDescriptions[1].offset = offsetof(Vertex, color);

	attributeDescriptions[2].binding = 0;
	attributeDescriptions[2].location = 2;
	attributeDescriptions[2].format = VK_FORMAT_R32G32_SFLOAT;
	attributeDescriptions[2].offset = offsetof(Vertex, texCoord);

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

	VkImageView								textureImageView;
	VkSampler								textureSampler;

	VkImage									depthImage;
	VkDeviceMemory							depthImageMemory;
	VkImageView								depthImageView;

public:
	Buffer_Wrapper();
	~Buffer_Wrapper();
	
	void BufferInit(VkDevice logDevice, VkPhysicalDevice physDevice, VkQueue gQueue, std::vector<VkImage> swapChainImages, Command *cmd);

	void CreateDescriptorSetLayout();

	void CreateDescriptorPool();

	void CreateDescriptorSets();

	void CreateVertexBuffers(Command *cmd, const std::vector<Vertex> vertices);

	void CreateIndexBuffers(Command *cmd, const std::vector<uint32_t> indices);

	void CreateUniformBuffers();

	void CreateDepthResources(VkExtent2D extents, Command *graphicsCommand);

	static void CreateBuffer(VkDeviceSize size, VkBufferUsageFlags usage, VkMemoryPropertyFlags properties, VkBuffer& buffer, VkDeviceMemory& bufferMemory, VkDevice logicalDevice, VkQueue graphicsQueue, VkPhysicalDevice physicalDevice);

	static void CopyBuffer(VkBuffer srcBuffer, VkBuffer dstBuffer, VkDeviceSize size, VkDevice logicalDevice, VkQueue graphicsQueue, Command* graphicsCmd);

	void SetTextureInfo(VkImageView texImgView, VkSampler texSampler);

	VkBuffer GetVertexBuffer(){ return vertexBuffer; }
	VkDeviceMemory GetVertexBufferMemory() { return vertexBufferMemory; }
	VkBuffer GetIndexBuffer(){ return indexBuffer; }
	std::vector<VkBuffer> GetUniformBuffers() { return uniformBuffers; }
	std::vector<VkDeviceMemory> GetUniformBuffersMemory() { return uniformBuffersMemory; }
	std::vector<VkDescriptorSet> GetDescriptorSets() { return descriptorSets; }
	VkDescriptorSetLayout GetDescriptorSetLayout(){ return descriptorSetLayout; }
	VkImageView GetDepthImageView(){ return depthImageView; }

	VkFormat FindSupportedFormat(const std::vector<VkFormat>& candidates, VkImageTiling tiling, VkFormatFeatureFlags features);
	
	VkFormat FindDepthFormat();

	static bool HasStencilComponent(VkFormat format) { return format == VK_FORMAT_D32_SFLOAT_S8_UINT || format == VK_FORMAT_D24_UNORM_S8_UINT;}

	static uint32_t FindMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags properties, VkPhysicalDevice physicalDevice);
};

