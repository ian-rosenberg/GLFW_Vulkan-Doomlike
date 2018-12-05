#pragma once

#include <vulkan/vulkan.h>
#include <vector>

using namespace std;

class Queue_Wrapper
{
private:
	uint32_t								graphicsQueueFamily;
	uint32_t								presentQueueFamily;
	uint32_t								transferQueueFamily;
	uint32_t								queueFamilyCount;
	VkQueueFamilyProperties					*queueProperties;
	float									graphicsQueuePriority;
	float									presentQueuePriority;
	float									transferQueuePriority;
	uint32_t								workQueueCount;
	std::vector<VkDeviceQueueCreateInfo>	queueCreateInfo;

	VkQueue									graphicsQueue;
	VkQueue									presentQueue;
	VkQueue									transferQueue;
public:
	Queue_Wrapper();
	~Queue_Wrapper();

	VkDeviceQueueCreateInfo* Queue_WrapperInit(VkPhysicalDevice* device, VkSurfaceKHR surface);

	void SetupDeviceQueues(VkDevice device);

	uint32_t GetQueueFamilyCount() { return queueFamilyCount; }

	uint32_t GetGraphicsQueueFamily(){ return graphicsQueueFamily; }
	uint32_t GetPresentQueueFamily(){ return presentQueueFamily; }
	uint32_t GetTransferQueueFamily(){ return transferQueueFamily; }

	VkQueue	GetGraphicsQueue(){ return graphicsQueue; }
	VkQueue GetPresentQueue(){ return presentQueue; }
	VkQueue GetTransferQueue(){ return transferQueue; }

	VkDeviceQueueCreateInfo GetGraphicsQueueInfo();
	VkDeviceQueueCreateInfo GetPresentQueueInfo();
	VkDeviceQueueCreateInfo GetTransferQueueInfo();

	uint32_t GetWorkQueueCount() { return workQueueCount; }

	bool IsComplete() { return graphicsQueueFamily != -1 && presentQueueFamily != -1 && transferQueueFamily != -1; }
};