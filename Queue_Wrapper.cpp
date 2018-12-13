#include "Queue_Wrapper.h"
#include "simple_logger.h"



Queue_Wrapper::Queue_Wrapper()
{
	queueFamilyCount = 0;
	queueProperties = {};

	graphicsQueue = {};
	presentQueue = {};
	transferQueue = {};
	
	graphicsQueuePriority = 0;
	presentQueuePriority = 0;
	transferQueuePriority = 0;

	workQueueCount = 0;
}


VkDeviceQueueCreateInfo* Queue_Wrapper::Queue_WrapperInit(VkPhysicalDevice* device, VkSurfaceKHR surface)
{
	VkBool32 supported;

	graphicsQueueFamily = -1;
	presentQueueFamily = -1; 
	transferQueueFamily = -1;

	vkGetPhysicalDeviceQueueFamilyProperties(*device, &queueFamilyCount, NULL);

	queueCreateInfo = {};

	if (queueFamilyCount < 1)
	{
		slog("Failed to get any queue properties, quiting...");
		
		return NULL;
	}

	std::vector<VkQueueFamilyProperties> localQueueFamilyProps(queueFamilyCount);

	vkGetPhysicalDeviceQueueFamilyProperties(*device, &queueFamilyCount, localQueueFamilyProps.data());

	slog("discoverd %i queue family properties", queueFamilyCount);

	for (int i = 0; i < queueFamilyCount; ++i)
	{
		slog("Queue family %i:", i);
		slog("queue flag bits %i", localQueueFamilyProps[i].queueFlags);
		slog("queue count %i", localQueueFamilyProps[i].queueCount);
		slog("queue timestamp valid bits %i", localQueueFamilyProps[i].timestampValidBits);
		slog("queue min image transfer granularity %iw %ih %id",
			localQueueFamilyProps[i].minImageTransferGranularity.width,
			localQueueFamilyProps[i].minImageTransferGranularity.height,
			localQueueFamilyProps[i].minImageTransferGranularity.depth);

		vkGetPhysicalDeviceSurfaceSupportKHR(*device, i, surface, &supported);

		if (localQueueFamilyProps[i].queueFlags & VK_QUEUE_GRAPHICS_BIT)
		{
			graphicsQueueFamily = i;
			graphicsQueuePriority = 1.0f;

			slog("Queue %i handles graphics calls", i);
		}

		/*if (localQueueFamilyProps[i].queueFlags & VK_QUEUE_TRANSFER_BIT)
		{
			transferQueueFamily = i;
			transferQueuePriority = 1.0f;

			slog("Queue %i handles transfer calls", i);
		}*/

		if (supported)
		{
			presentQueueFamily = i;
			presentQueuePriority = 1.0f;
			
			slog("Queue %i handles present calls", i);
		}
	}

	queueProperties = localQueueFamilyProps.data();

	slog("using queue family %i for graphics commands",graphicsQueueFamily);
	slog("using queue family %i for rendering pipeline", presentQueueFamily);
	slog("using queue family %i for transfer pipeline", transferQueueFamily);

	if (graphicsQueueFamily != -1)
	{
		workQueueCount++;
	}
	
	if ((presentQueueFamily != -1) && (presentQueueFamily != graphicsQueueFamily))
	{ 
		workQueueCount++; 
	}

	if (transferQueueFamily != -1 && (transferQueueFamily != graphicsQueueFamily && transferQueueFamily != presentQueueFamily))
	{
		workQueueCount++;
	}

	if (workQueueCount < 1)
	{
		slog("No suitable queues for graphics calls or presentation");
	}
	else
	{
		int i = 0;
		if (graphicsQueueFamily != -1)
		{
			queueCreateInfo.push_back(GetGraphicsQueueInfo());

			++i;
		}
		if (presentQueueFamily != -1)
		{
			queueCreateInfo.push_back(GetPresentQueueInfo());

			++i;
		}
		if (transferQueueFamily != -1)
		{
			queueCreateInfo.push_back(GetTransferQueueInfo());

			++i;
		}
	}

	return queueCreateInfo.data();
}

void Queue_Wrapper::SetupDeviceQueues(VkDevice device)
{
	if (graphicsQueueFamily != -1)
	{
		vkGetDeviceQueue(device, graphicsQueueFamily, 0, &graphicsQueue);
	}
	if (presentQueueFamily != -1)
	{
		vkGetDeviceQueue(device, presentQueueFamily, 0, &presentQueue);
	}
	/*if (transferQueueFamily != -1)
	{
		vkGetDeviceQueue(device, transferQueueFamily, 0, &transferQueue);
	}*/
}

VkDeviceQueueCreateInfo Queue_Wrapper::GetGraphicsQueueInfo()
{
	VkDeviceQueueCreateInfo queueCreateInfo = {};
	queueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
	queueCreateInfo.queueFamilyIndex = graphicsQueueFamily;
	queueCreateInfo.queueCount = 1;
	queueCreateInfo.pQueuePriorities = &graphicsQueuePriority;
	return queueCreateInfo;
}

VkDeviceQueueCreateInfo Queue_Wrapper::GetPresentQueueInfo()
{
	VkDeviceQueueCreateInfo queueCreateInfo = {};
	queueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
	queueCreateInfo.queueFamilyIndex = presentQueueFamily;
	queueCreateInfo.queueCount = 1;
	queueCreateInfo.pQueuePriorities = &presentQueuePriority;
	return queueCreateInfo;
}

VkDeviceQueueCreateInfo Queue_Wrapper::GetTransferQueueInfo()
{
	VkDeviceQueueCreateInfo queueCreateInfo = {};
	queueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
	queueCreateInfo.queueFamilyIndex = transferQueueFamily;
	queueCreateInfo.queueCount = 1;
	queueCreateInfo.pQueuePriorities = &transferQueuePriority;
	return queueCreateInfo;
}