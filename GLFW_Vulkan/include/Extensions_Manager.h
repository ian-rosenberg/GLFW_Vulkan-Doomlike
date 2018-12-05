#pragma once

#include <vulkan/vulkan.h>
#include <GLFW/glfw3.h>
#include <vector>


typedef enum
{
	ET_Instance,
	ET_Device
}ExtensionType;

typedef struct
{
	uint32_t							availableExtensionCount;
	std::vector<VkExtensionProperties>	availableExtensions;
	std::vector<const char*>			enabledExtensionNames;
	uint32_t							enabledExtensionCount;
}Extensions;

class Extensions_Manager
{
private:
    Extensions deviceExtensions;
	Extensions instanceExtensions;

public:
	Extensions_Manager();
	~Extensions_Manager();

	std::vector<const char*> InstanceExtensionsInit(bool validation);
	void DeviceExtensionsInit(VkPhysicalDevice device, std::vector<const char*> *deviceExtNames);
};
