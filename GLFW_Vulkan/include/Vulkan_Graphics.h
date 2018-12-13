#pragma once

#include <vulkan/vulkan.h>
#include <iostream>
#include <array>

#include "Pipeline_Wrapper.h"
#include "GLFW_Wrapper.h"
#include "Extensions_Manager.h"
#include "Swapchain_Wrapper.h"
#include "Queue_Wrapper.h"
#include "Commands_Wrapper.h"
#include "Buffers.h"

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

	bool							framebufferResized;
	
	GLFW_Wrapper					*glfwWrapper;
	Commands_Wrapper				*cmdWrapper;
	Extensions_Manager				*extManager;
	Queue_Wrapper					*queueWrapper;
	Swapchain_Wrapper				*swapchainWrapper;
	Pipeline_Wrapper				*pipeWrapper;
	Buffer_Wrapper					*bufferWrapper;
	
	Vulkan_Graphics(GLFW_Wrapper *glfwWrapper, bool enableValidation);
	~Vulkan_Graphics();

	Command* GetGraphicsPool(){ return graphicsCommands; }
	VkDevice GetLogicalDevice(){ return logicalDevice; }

	//testing
	void DrawFrame();
};