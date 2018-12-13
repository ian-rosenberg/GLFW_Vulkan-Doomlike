#include <vector>

#include "Extensions_Manager.h"
#include "simple_logger.h"
#include "gf3d_types.h"

Extensions_Manager::Extensions_Manager()
{

}

Extensions_Manager::~Extensions_Manager()
{
}

std::vector<const char*> Extensions_Manager::InstanceExtensionsInit(bool validation)
{
	std::vector<const char*> glfwExtensions = {};
	uint32_t glfwCount = 0;
	const char** glfwNames = glfwGetRequiredInstanceExtensions(&glfwCount);
	std::vector<const char*> emptyExts = {};
	std::vector<const char*> allInstanceExtensions = {};

	for (int i = 0; i < glfwCount; ++i)
	{
		slog("GLFW Extensions to use: %s", glfwNames[i]);

		glfwExtensions.push_back(glfwNames[i]);
	}

	vkEnumerateInstanceExtensionProperties(NULL, &instanceExtensions.availableExtensionCount, NULL);

	slog("Total available instance extensions: %i",instanceExtensions.availableExtensionCount);

	if (!instanceExtensions.availableExtensionCount)
	{
		return emptyExts;
	}

	std::vector<VkExtensionProperties> extensions(instanceExtensions.availableExtensionCount + glfwCount);

	slog("Instance Extension return code: %i", vkEnumerateInstanceExtensionProperties(NULL, &instanceExtensions.availableExtensionCount, extensions.data()));

	instanceExtensions.availableExtensions = extensions;

	if (validation)
	{
		instanceExtensions.enabledExtensionNames.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
		++instanceExtensions.enabledExtensionCount;
	}

	for (int i = 0; i < instanceExtensions.availableExtensionCount; i++)
	{
		slog("Instance extension to use: %s", instanceExtensions.availableExtensions[i].extensionName);

		instanceExtensions.enabledExtensionNames.push_back(instanceExtensions.availableExtensions[i].extensionName);

		++instanceExtensions.enabledExtensionCount;
	}

	for (const char* instanceExt : instanceExtensions.enabledExtensionNames)
	{
		allInstanceExtensions.push_back(instanceExt);

		slog("Enabled instance extension: %s", instanceExt);

		++instanceExtensions.enabledExtensionCount;
	}

	return allInstanceExtensions;
}

void Extensions_Manager::DeviceExtensionsInit(VkPhysicalDevice device, std::vector<const char*> *deviceExtNames)
{
	vkEnumerateDeviceExtensionProperties(device, NULL, &deviceExtensions.availableExtensionCount, NULL);

	slog("Total available device extensions: %i", deviceExtensions.availableExtensionCount);

	if (deviceExtensions.availableExtensionCount < 1)
	{
		return;
	}	

	std::vector<VkExtensionProperties> localDeviceExtensions(deviceExtensions.availableExtensionCount);

	vkEnumerateDeviceExtensionProperties(device, NULL, &deviceExtensions.availableExtensionCount, localDeviceExtensions.data());

	deviceExtensions.availableExtensions = localDeviceExtensions;

	for (int i = 0; i < deviceExtensions.availableExtensions.size(); ++i)
	{
		slog("available device extension: %s", deviceExtensions.availableExtensions[i].extensionName);

		if (deviceExtensions.availableExtensions[i].extensionName == "VK_KHR_shader_atomic_int64"
			|| deviceExtensions.availableExtensions[i].extensionName == "VK_KHR_driver_properties")
		{
			continue;
		}
		else
		{
			deviceExtensions.enabledExtensionNames.push_back(deviceExtensions.availableExtensions[i].extensionName);
			deviceExtNames->push_back(deviceExtensions.availableExtensions[i].extensionName);
		}
	}

	deviceExtensions.enabledExtensionCount = deviceExtensions.enabledExtensionNames.size();
}