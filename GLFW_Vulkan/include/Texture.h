#pragma once


#include <vulkan/vulkan.h>
#include <vector>

#include "Commands_Wrapper.h"

struct Texture
{
	VkImage textureImage;
	VkDeviceMemory textureImageMemory;
};


class Texture_Wrapper
{
private:
	VkDevice					logicalDevice;
	VkPhysicalDevice			physicalDevice;

	VkQueue						graphicsQueue;

	Command						*graphicsCommand;

	VkImage						textureImage;
	VkDeviceMemory				textureImageMemory;
	VkImageView					textureImageView;
	VkSampler					textureSampler;


public:
	Texture_Wrapper();

	~Texture_Wrapper();

	void Texture_WrapperInit(VkPhysicalDevice physDevice, VkDevice logDevice, VkQueue gQueue, Command *cmd);

	void CreateTextureImage();

	static void CreateImage(uint32_t width, uint32_t height, VkFormat format, VkImageTiling tiling, VkImageUsageFlags usage, VkMemoryPropertyFlags properties, VkImage& image, VkDeviceMemory& imageMemory, VkDevice logicalDevice, VkPhysicalDevice physicalDevice);

	static void TransitionImageLayout(VkImage image, VkFormat format, VkImageLayout oldLayout, VkImageLayout newLayout, VkDevice logicalDevice, Command *graphicsCommand, VkQueue graphicsQueue);

	static void CopyBufferToImage(VkBuffer buffer, VkImage image, uint32_t width, uint32_t height, VkDevice logicalDevice, Command *graphicsCommand, VkQueue graphicsQueue);

	static VkImageView CreateImageView(VkImage image, VkFormat format, VkDevice logDevice, VkImageAspectFlags aspectFlags);

	void CreateTextureImageView();

	void CreateTextureSampler();

	VkImageView GetTextureImageView(){ return textureImageView; }

	VkSampler GetTextureSampler(){ return textureSampler;}
};