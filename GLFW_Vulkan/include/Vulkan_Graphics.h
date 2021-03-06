#pragma once

#include <vulkan/vulkan.h>
#include <vector>
#include <iostream>
#include <array>

#include "Pipeline_Wrapper.h"
#include "GLFW_Wrapper.h"
#include "Extensions_Manager.h"
#include "Swapchain_Wrapper.h"
#include "Queue_Wrapper.h"
#include "Commands_Wrapper.h"
#include "Buffers.h"
#include "Texture.h"
#include "Model.h"

class Vulkan_Graphics
{
private:
	VkInstance						vkInstance;

	Command             			*graphicsCommands;

	VkDebugUtilsMessengerEXT		callback;
	VkSurfaceKHR					surface;
	
	VkPhysicalDevice				physicalDevice;
	VkDevice						logicalDevice;
	uint32_t						deviceCount;
	VkPhysicalDevice				*devices;
	bool							logicalDeviceCreated;

	VkQueue							graphicsQueue;
	VkQueue							presentQueue;
	VkQueue							computeQueue;

	VkDeviceQueueCreateInfo			*queueCreateInfo;
	VkPhysicalDeviceFeatures		deviceFeatures;

	std::vector<VkSemaphore>		imageAvailableSemaphores;
	std::vector<VkSemaphore>		renderFinishedSemaphores;
	std::vector<VkFence>			inFlightFences;
	size_t							currentFrame;

	std::vector<VkLayerProperties>	validationAvailableLayers;
	std::vector<const char*>		validationInstanceLayerNames;
	std::vector<const char*>		validationDeviceLayerNames;

	Command 						stagingCommand;

	Model							*testModel;


	void CreateVulkanInstance();
	void CreateLogicalDevice();

	void CreateSemaphores();
	
	void SetupDebugCallback();

	bool CheckValidationLayerSupport();

	void PickPhysicalDevice();

	void CreateFramebuffers();

	VkDeviceCreateInfo GetDeviceInfo(bool validation);

	VkPhysicalDevice GetPhysicalDevice(){ return physicalDevice; }
	
	bool IsDeviceSuitable(VkPhysicalDevice device);

public:
	GLFW_Wrapper					*glfwWrapper;
	Commands_Wrapper				*cmdWrapper;
	Extensions_Manager				*extManager;
	Queue_Wrapper					*queueWrapper;
	Swapchain_Wrapper				*swapchainWrapper;
	Pipeline_Wrapper				*pipeWrapper;
	Buffer_Wrapper					*bufferWrapper;
	Texture_Wrapper					*textureWrapper;
	Model_Manager					*modelManager;
	
	Vulkan_Graphics(GLFW_Wrapper *glfwWrapper, bool enableValidation);
	~Vulkan_Graphics();

	Command* GetGraphicsPool(){ return graphicsCommands; }
	VkDevice GetLogicalDevice(){ return logicalDevice; }

	//testing
	void DrawFrame();

	void UpdateUniformBuffer(uint32_t imageIndex);
};