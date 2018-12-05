#include <algorithm>

#include "Swapchain_Wrapper.h"
#include "simple_logger.h"

Swapchain_Wrapper::Swapchain_Wrapper()
{
	presentModes = {};
	swapImages = {};
	imageViews = {}; 

	chosenFormat = 0;
	chosenPresentMode = 0;
	swapchainImageCount = 0;
}

Swapchain_Wrapper::~Swapchain_Wrapper()
{	
	if (imageViews.size())
	{
		for (int i = 0; i < swapchainImageCount; i++)
		{
			vkDestroyImageView(logDevice, imageViews[i], NULL);
			slog("imageview destroyed");
		}
	}

	if (frameBuffers.size())
	{
		for (int i = 0; i < frameBuffers.size(); ++i)
		{
			vkDestroyFramebuffer(logDevice, frameBuffers[i], NULL);
		}
	}

	if (swapchain)
	{
		vkDestroySwapchainKHR(logDevice, swapchain, NULL);
	}

}

void Swapchain_Wrapper::SwapchainInit(VkPhysicalDevice device, VkDevice logicalDevice, VkSurfaceKHR surface, uint32_t width, uint32_t height, Queue_Wrapper *qWrapper)
{	
	uint32_t formatCount = 0;
	uint32_t presentModeCount = 0;
	
	vkGetPhysicalDeviceSurfaceCapabilitiesKHR(device, surface, &capabilities);

	vkGetPhysicalDeviceSurfaceFormatsKHR(device, surface, &formatCount, NULL);

	slog("Device supports %i surface formats", formatCount);

	if (formatCount != 0)
	{
		formats.resize(formatCount);

		vkGetPhysicalDeviceSurfaceFormatsKHR(device, surface, &formatCount, formats.data());

		for (int i = 0; i < formatCount; ++i)
		{
			slog("surface format %i", i);
			slog("format: %i", formats[i].format);
			slog("colorspace: %i", formats[i].colorSpace);
		}
	}

	vkGetPhysicalDeviceSurfacePresentModesKHR(device, surface, &presentModeCount, NULL);

	slog("Device supports %i presentation modes", presentModeCount);

	if (presentModeCount != 0)
	{
		presentModes.resize(presentModeCount);

		vkGetPhysicalDeviceSurfacePresentModesKHR(device, surface, &presentModeCount, presentModes.data());

		for (int i = 0; i < presentModeCount; ++i)
		{
			slog("presentation mode: %i is %i", i, presentModes[i]);
		}
	}

	chosenFormat = ChooseFormat();

	swapImageFormat = formats[chosenFormat].format;

	slog("choosing surface format %i", chosenFormat);

	chosenPresentMode = GetPresentationMode();
	slog("choosing presentation mode %i", chosenPresentMode);

	extent = ConfigureExtent(width, height);
	slog("choosing swap chain extent of (%i,%i)", extent.width, extent.height);

	CreateSwapchain(logicalDevice, surface, qWrapper);

	logDevice = logicalDevice;
}

int Swapchain_Wrapper::ChooseFormat()
{
	int chosen = -1;

	for (int i = 0; i < formats.size(); ++i)
	{
		if (formats[i].format == VK_FORMAT_B8G8R8A8_UNORM &&
			formats[i].colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR)
		{
			return i;
		}
		chosen = i;
	}

	return chosen;
}

int Swapchain_Wrapper::GetPresentationMode()
{
	int chosen = -1;

	for (int i = 0; i < formats.size(); ++i)
	{
		if (presentModes[i] == VK_PRESENT_MODE_MAILBOX_KHR)
		{
			return i;
		}

		chosen = i;
	}

	return chosen;
}

VkExtent2D Swapchain_Wrapper::ConfigureExtent(int width, int height)
{
	VkExtent2D actual;

	slog("Requested resolution: %i x %i", width, height);
	slog("Minimum resolution: %i x %i", capabilities.minImageExtent.width, capabilities.minImageExtent.height);
	slog("Maximum resolution: %i x %i", capabilities.maxImageExtent.width, capabilities.maxImageExtent.height);

	actual.width = std::max(capabilities.minImageExtent.width, std::min(capabilities.maxImageExtent.width, actual.width));
	actual.height = std::max(capabilities.minImageExtent.height, std::min(capabilities.maxImageExtent.height, actual.height));

	return actual;
}

void Swapchain_Wrapper::CreateSwapchain(VkDevice lDevice, VkSurfaceKHR surface, Queue_Wrapper *qWrapper)
{
	int graphicsFamily;
    int presentFamily;
    int transferFamily;
    VkSwapchainCreateInfoKHR createInfo = {};
    uint32_t queueFamilyIndices[2];

	slog("minimum images needed for swap chain: %i", capabilities.minImageCount);
	slog("maximum images needed for swap chain : %i", capabilities.maxImageCount);

	swapchainCount = capabilities.minImageCount + 1;
	if (capabilities.maxImageCount != 0)
	{
		swapchainCount = std::min(swapchainCount, capabilities.maxImageCount);
	}
	slog("using %i images for the swapchain", swapchainCount);

	createInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
	createInfo.surface = surface;
	createInfo.minImageCount = swapchainCount;
	createInfo.imageFormat = swapImageFormat;
	createInfo.imageColorSpace = formats[chosenFormat].colorSpace;
	createInfo.imageExtent = extent;
	createInfo.imageArrayLayers = 1;
	createInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;

	graphicsFamily = qWrapper->GetGraphicsQueueFamily();
	presentFamily = qWrapper->GetPresentQueueFamily();
	//transferFamily = qWrapper->GetTransferQueueFamily();

	queueFamilyIndices[0] = graphicsFamily;
	queueFamilyIndices[1] = presentFamily;
	//queueFamilyIndices[2] = transferFamily;

	if (graphicsFamily != presentFamily)
	{
		createInfo.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
		createInfo.queueFamilyIndexCount = 2;
		createInfo.pQueueFamilyIndices = queueFamilyIndices;
	}
	else
	{
		createInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
	}

	createInfo.preTransform = capabilities.currentTransform;
	createInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;

	createInfo.presentMode = presentModes[chosenPresentMode];
	createInfo.clipped = VK_TRUE;

	createInfo.oldSwapchain = VK_NULL_HANDLE;

	if (vkCreateSwapchainKHR(lDevice, &createInfo, NULL, &swapchain) != VK_SUCCESS)
	{
		slog("failed to create swapchain!");
		return;
	}
	slog("created a swapchain with length %i", swapchainCount);

	vkGetSwapchainImagesKHR(lDevice, swapchain, &swapchainImageCount, NULL);
	if (swapchainImageCount == 0)
	{
		slog("failed to create any swap images!");
		return;
	}

	swapImages.resize(swapchainImageCount);

	imageViews = {};

	vkGetSwapchainImagesKHR(lDevice, swapchain, &swapchainImageCount, swapImages.data());
	slog("created swapchain with %i images", swapchainImageCount);

	for (int i = 0; i < swapImages.size(); ++i)
	{
		imageViews.push_back(CreateImageView(swapImages[i], formats[chosenFormat].format, lDevice));
	}
	
	slog("created image views");
}

VkImageView Swapchain_Wrapper::CreateImageView(VkImage image, VkFormat format, VkDevice lDevice)
{
	VkImageView imageView;
	VkImageViewCreateInfo viewInfo = {};

	viewInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
	viewInfo.image = image;
	viewInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
	viewInfo.format = format;
	viewInfo.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
	viewInfo.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
	viewInfo.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
	viewInfo.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;
	viewInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
	viewInfo.subresourceRange.baseMipLevel = 0;
	viewInfo.subresourceRange.levelCount = 1;
	viewInfo.subresourceRange.baseArrayLayer = 0;
	viewInfo.subresourceRange.layerCount = 1;

	if (vkCreateImageView(lDevice, &viewInfo, NULL, &imageView) != VK_SUCCESS)
	{
		slog("failed to create texture image view!");
		return VK_NULL_HANDLE;
	}

	return imageView;
}

void Swapchain_Wrapper::CreateFrameBuffers(Pipeline* pipe)
{
	frameBuffers.resize(imageViews.size());

	for (size_t i = 0; i < imageViews.size(); i++) 
	{
		VkImageView attachments[] = { imageViews[i] };

		VkFramebufferCreateInfo framebufferInfo = {};
		framebufferInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
		framebufferInfo.renderPass = pipe->renderPass;
		framebufferInfo.attachmentCount = 1;
		framebufferInfo.pAttachments = attachments;
		framebufferInfo.width = extent.width;
		framebufferInfo.height = extent.height;
		framebufferInfo.layers = 1;

		if (vkCreateFramebuffer(logDevice, &framebufferInfo, nullptr, &frameBuffers[i]) != VK_SUCCESS) {
			throw std::runtime_error("failed to create framebuffer!");
		}
	}
}