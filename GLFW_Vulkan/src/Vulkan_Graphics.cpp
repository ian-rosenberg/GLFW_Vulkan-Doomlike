#include "Vulkan_Graphics.h"
#include "simple_logger.h"

static bool enableValidationLayers;

const static std::vector<const char*> validationLayers = {
	"VK_LAYER_LUNARG_standard_validation"
};

VkResult CreateDebugUtilsMessengerEXT(VkInstance instance, const VkDebugUtilsMessengerCreateInfoEXT* pCreateInfo, const VkAllocationCallbacks* pAllocator, VkDebugUtilsMessengerEXT* pCallback) 
{
	auto func = (PFN_vkCreateDebugUtilsMessengerEXT)vkGetInstanceProcAddr(instance, "vkCreateDebugUtilsMessengerEXT");
	if (func != nullptr) {
		return func(instance, pCreateInfo, pAllocator, pCallback);
	}
	else {
		return VK_ERROR_EXTENSION_NOT_PRESENT;
	}
}

void DestroyDebugUtilsMessengerEXT(VkInstance instance, VkDebugUtilsMessengerEXT callback, const VkAllocationCallbacks* pAllocator) 
{
	auto func = (PFN_vkDestroyDebugUtilsMessengerEXT) vkGetInstanceProcAddr(instance, "vkDestroyDebugUtilsMessengerEXT");
    if (func != nullptr)
	{
        func(instance, callback, pAllocator);
    }
}

static VKAPI_ATTR VkBool32 VKAPI_CALL debugCallback(
	VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
	VkDebugUtilsMessageTypeFlagsEXT messageType,
	const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,
	void* pUserData) 
{

	std::cerr << "validation layer: " << pCallbackData->pMessage << std::endl;

	return VK_FALSE;
}

static std::vector<const char*> instanceExtensionNames = {};

Vulkan_Graphics::Vulkan_Graphics(GLFW_Wrapper *gWrapper, bool enableValidation)
{
	vkInstance = VK_NULL_HANDLE;
	extManager = new Extensions_Manager();
	queueWrapper = new Queue_Wrapper();
	swapchainWrapper = new Swapchain_Wrapper();
	pipeWrapper = new Pipeline_Wrapper();
	cmdWrapper = new Commands_Wrapper();
	glfwWrapper = gWrapper;
	enableValidationLayers = enableValidation;
	validationDeviceLayerNames = {};
	graphicsCommandPool = VK_NULL_HANDLE;
	graphicsCommandBuffers = {};
	
	CreateVulkanInstance();
	surface = glfwWrapper->CreateGLFWWindowSurface(vkInstance);
	SetupDebugCallback();
	PickPhysicalDevice();
	CreateLogicalDevice(); 
	queueWrapper->SetupDeviceQueues(logicalDevice);
	swapchainWrapper->SwapchainInit(physicalDevice, logicalDevice, surface, glfwWrapper->GetWindowWidth(), glfwWrapper->GetWindowHeight(), queueWrapper);
	
	//following tutorial/DJ but using tutorial as basis for now, will add model stuff later

	pipeWrapper->Pipeline_WrapperInit(4);

	//pipeWrapper->RenderPassSetup(swapchainWrapper->GetFormat(), physicalDevice, logicalDevice);

	pipeWrapper->PipelineLoad(logicalDevice, "shaders/vert.spv", "shaders/frag.spv", swapchainWrapper->GetFormat(), physicalDevice, swapchainWrapper->GetExtent());

	//swapchainWrapper->SetupFramebuffers(pipeWrapper->GetPipe());
	swapchainWrapper->CreateFrameBuffers(&pipeWrapper->GetCurrentPipe());
	
	cmdWrapper->CommandsWrapperInit(8, logicalDevice);

	//graphicsCommands = cmdWrapper->GraphicsCommandPoolSetup(swapchainWrapper->GetFrameBuffers().size(), currentPipe, queueWrapper->GetGraphicsQueueFamily());
	cmdWrapper->CreateCommandPool(queueWrapper->GetGraphicsQueueFamily(), &graphicsCommandPool);
	cmdWrapper->CreateCommandBuffers(&graphicsCommandBuffers, swapchainWrapper->GetFrameBuffers().size(), swapchainWrapper->GetFrameBuffers(), &pipeWrapper->GetCurrentPipe(), swapchainWrapper->GetExtent(), graphicsCommandPool);

	CreateSemaphores();
}

void Vulkan_Graphics::SetupDebugCallback() 
{
	if (!enableValidationLayers)
	{
		return;
	}

	VkDebugUtilsMessengerCreateInfoEXT createInfo = {};


	createInfo.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;

	createInfo.messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;

	createInfo.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;

	createInfo.pfnUserCallback = debugCallback;
	createInfo.pUserData = NULL; // Optional

	if (CreateDebugUtilsMessengerEXT(vkInstance, &createInfo, NULL, &callback) != VK_SUCCESS)
	{
		throw std::runtime_error("failed to set up debug callback!");
	}
	else
	{
		slog("Set up debug callback");
	}
}

Vulkan_Graphics::~Vulkan_Graphics()
{
	if (enableValidationLayers) {
		DestroyDebugUtilsMessengerEXT(vkInstance, callback, nullptr);
	}

	if (cmdWrapper)
	{
		cmdWrapper->~Commands_Wrapper();
	}

	if (pipeWrapper)
	{
		pipeWrapper->~Pipeline_Wrapper();
	}

	if (swapchainWrapper)
	{
		swapchainWrapper->~Swapchain_Wrapper();
	}

	vkDestroySemaphore(logicalDevice, renderFinishedSemaphore, nullptr);
	vkDestroySemaphore(logicalDevice, imageAvailableSemaphore, nullptr);
	
	if (vkInstance)
	{
		vkDestroyInstance(vkInstance, nullptr); 
	}
}

void Vulkan_Graphics::CreateVulkanInstance()
{
	VkInstanceCreateInfo createInfo = {};
	VkApplicationInfo appInfo = {};
	std::vector<const char*> vkExtensions = {};
	validationInstanceLayerNames = {};
	validationAvailableLayers = {};


	if (enableValidationLayers && !CheckValidationLayerSupport())
	{
		slog("Validation layers requested, but not available");
		return;
	}

	vkExtensions = extManager->InstanceExtensionsInit(enableValidationLayers);

	vkInstance = VK_NULL_HANDLE;

	appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
	appInfo.pApplicationName = "Hello Triangle";
	appInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
	appInfo.pEngineName = "No Engine";
	appInfo.engineVersion = VK_MAKE_VERSION(1, 0, 0);
	appInfo.apiVersion = VK_API_VERSION_1_0;

	createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
	createInfo.pApplicationInfo = &appInfo;

	if (enableValidationLayers)
	{
		slog("Adding validation layers to vkinstance create info");

		createInfo.enabledLayerCount = static_cast<uint32_t>(validationLayers.size());
		createInfo.ppEnabledLayerNames = validationLayers.data();
	}

	createInfo.enabledExtensionCount = static_cast<uint32_t>(vkExtensions.size());
	createInfo.ppEnabledExtensionNames = vkExtensions.data();

	if (vkCreateInstance(&createInfo, nullptr, &vkInstance) != VK_SUCCESS)
	{
		slog("Failed to create vulkan instance.");
	}
	else
	{
		slog("Created vulkan instance!");
	}
}

void Vulkan_Graphics::CreateLogicalDevice()
{
	VkDeviceCreateInfo deviceCreateInfo = {};
	std::vector<const char*> deviceExts = {};
	validationDeviceLayerNames = {};

	extManager->DeviceExtensionsInit(physicalDevice, &deviceExts);

	deviceExts.push_back("VK_KHR_swapchain");

	deviceCreateInfo = GetDeviceInfo(enableValidationLayers);

	deviceCreateInfo.enabledExtensionCount = static_cast<uint32_t>(deviceExts.size());
	deviceCreateInfo.ppEnabledExtensionNames = deviceExts.data();

	if (vkCreateDevice(physicalDevice, &deviceCreateInfo, NULL, &logicalDevice) != VK_SUCCESS)
	{
		slog("Failed to create logical device!");

		return;
	}
	else
	{
		slog("Created logical device!");
	}

	logicalDeviceCreated = true;
}

bool Vulkan_Graphics::CheckValidationLayerSupport() 
{
	uint32_t layerCount;

	vkEnumerateInstanceLayerProperties(&layerCount, nullptr);

	std::vector<VkLayerProperties> availableLayers(layerCount);

	vkEnumerateInstanceLayerProperties(&layerCount, availableLayers.data());


	for (const char* layerName : validationLayers) {
		bool layerFound = false;

		for (const auto& layerProperties : availableLayers) {
			if (strcmp(layerName, layerProperties.layerName) == 0) {
				layerFound = true;
				break;
			}
		}

		if (!layerFound) {
			return false;
		}
	}

	validationAvailableLayers = availableLayers;

	for (int i = 0; i < validationAvailableLayers.size(); ++i)
	{
		validationInstanceLayerNames.push_back(validationAvailableLayers[i].layerName);
	}

	return true;
}

void Vulkan_Graphics::PickPhysicalDevice()
{
	VkPhysicalDevice physDevice = VK_NULL_HANDLE;
	uint32_t deviceCount = 0;

	vkEnumeratePhysicalDevices(vkInstance, &deviceCount, nullptr);

	if (deviceCount == 0)
	{
		slog("failed to find GPUs with Vulkan support!");
	}


	std::vector<VkPhysicalDevice> devices(deviceCount);

	vkEnumeratePhysicalDevices(vkInstance, &deviceCount, devices.data());

	for (const VkPhysicalDevice device : devices)
	{
		if (IsDeviceSuitable(device))
		{
			physDevice = device;
			break;
		}
	}

	if (physDevice == VK_NULL_HANDLE)
	{
		slog("failed to find a suitable GPU!");
	}
	else
	{
		physicalDevice = physDevice;
	}
}


bool Vulkan_Graphics::IsDeviceSuitable(VkPhysicalDevice device) {
	VkPhysicalDeviceProperties deviceProperties;

	vkGetPhysicalDeviceFeatures(device, &deviceFeatures);
	vkGetPhysicalDeviceProperties(device, &deviceProperties);

	slog("Device Name: %s", deviceProperties.deviceName);
	slog("Dedicated GPU: %i", (deviceProperties.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU) ? 1 : 0);
	slog("apiVersion: %i", deviceProperties.apiVersion);
	slog("driverVersion: %i", deviceProperties.driverVersion);
	slog("supports Geometry Shader: %i", deviceFeatures.geometryShader);

	return deviceFeatures.geometryShader;
}

VkDeviceCreateInfo Vulkan_Graphics::GetDeviceInfo(bool validation)
{
	VkDeviceCreateInfo createInfo = {};

	createInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;

	createInfo.pQueueCreateInfos = queueWrapper->Queue_WrapperInit(&physicalDevice, surface);
	createInfo.queueCreateInfoCount = queueWrapper->GetWorkQueueCount();


	
	deviceFeatures.samplerAnisotropy = VK_TRUE;

	createInfo.pEnabledFeatures = &deviceFeatures;

	if (validation)
	{		
		uint32_t layerCount;

		std::vector<const char*> validation = {};

		vkEnumerateDeviceLayerProperties(physicalDevice, &layerCount, NULL);

		std::vector<VkLayerProperties> availableDeviceLayers(layerCount);

		vkEnumerateDeviceLayerProperties(physicalDevice, &layerCount, availableDeviceLayers.data());

		for (VkLayerProperties prop : availableDeviceLayers)
		{
			validationDeviceLayerNames.push_back(prop.layerName);
		}

		createInfo.enabledLayerCount = validationDeviceLayerNames.size();
		createInfo.ppEnabledLayerNames = validationDeviceLayerNames.data();
	}
	else
	{
		createInfo.enabledLayerCount = 0;
	}

	return createInfo;
}

void Vulkan_Graphics::CreateSemaphores()
{
	VkSemaphoreCreateInfo semaphoreInfo = {};

	imageAvailableSemaphore = VK_NULL_HANDLE;
	renderFinishedSemaphore = VK_NULL_HANDLE;

	semaphoreInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

	if ((vkCreateSemaphore(logicalDevice, &semaphoreInfo, NULL, &imageAvailableSemaphore) != VK_SUCCESS) ||
		(vkCreateSemaphore(logicalDevice, &semaphoreInfo, NULL, &renderFinishedSemaphore) != VK_SUCCESS))
	{
		slog("failed to create semaphores!");
	}
	else{
		slog("created semaphores!");
	}
}

uint32_t Vulkan_Graphics::BeginDrawFrame()
{
	uint32_t imageIndex;
	VkSwapchainKHR swapChains[1] = {};

	swapChains[0] = swapchainWrapper->GetSwapchain();

	vkAcquireNextImageKHR(
		logicalDevice,
		swapChains[0],
		UINT_MAX,
		imageAvailableSemaphore,
		VK_NULL_HANDLE,
		&imageIndex);

	return imageIndex;
}

void Vulkan_Graphics::EndDrawFrame(uint32_t imageIndex)
{

	VkPresentInfoKHR presentInfo = {};
	VkSubmitInfo submitInfo = {};
	VkSwapchainKHR swapChains[1] = {};
	VkSemaphore waitSemaphores[] = { imageAvailableSemaphore };
	VkSemaphore signalSemaphores[] = { renderFinishedSemaphore };
	VkPipelineStageFlags waitStages[] = { VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT };
	swapChains[0] = swapchainWrapper->GetSwapchain();

	//gf3d_vgraphics_update_uniform_buffer(imageIndex);

	submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;

	submitInfo.waitSemaphoreCount = 1;
	submitInfo.pWaitSemaphores = waitSemaphores;
	submitInfo.pWaitDstStageMask = waitStages;
	//get count of configured command buffers
	//get the list of command buffers

	submitInfo.commandBufferCount = graphicsCommands->commandBufferCount;
	submitInfo.pCommandBuffers = graphicsCommands->commandBuffers.data();

	submitInfo.signalSemaphoreCount = 1;
	submitInfo.pSignalSemaphores = signalSemaphores;

	if (vkQueueSubmit(queueWrapper->GetPresentQueue(), 1, &submitInfo, VK_NULL_HANDLE) != VK_SUCCESS)
	{
		slog("failed to submit draw command buffer!");
	}

	presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;

	presentInfo.waitSemaphoreCount = 1;
	presentInfo.pWaitSemaphores = signalSemaphores;

	presentInfo.swapchainCount = 1;
	presentInfo.pSwapchains = swapChains;
	presentInfo.pImageIndices = &imageIndex;
	presentInfo.pResults = NULL; // Optional

	vkQueuePresentKHR(queueWrapper->GetPresentQueue(), &presentInfo);
}




//testing
/*void Vulkan_Graphics::DrawFrame() {
	uint32_t imageIndex;
	vkAcquireNextImageKHR(logicalDevice, swapchainWrapper->GetSwapchain(), std::numeric_limits<uint64_t>::max(), imageAvailableSemaphore, VK_NULL_HANDLE, &imageIndex);
	
	VkRenderPassBeginInfo renderPassInfo = {};
	renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
	renderPassInfo.renderPass = currentPipe.renderPass;
	renderPassInfo.framebuffer = swapchainFramebuffers[imageIndex];

	renderPassInfo.renderArea.offset = { 0, 0 };
	renderPassInfo.renderArea.extent = swapchainWrapper->GetExtent();

	VkClearValue clearColor = { 0.0f, 1.0f, 0.0f, 1.0f };
	renderPassInfo.clearValueCount = 1;
	renderPassInfo.pClearValues = &clearColor;

	VkSubmitInfo submitInfo = {};
	submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;

	VkSemaphore waitSemaphores[] = {imageAvailableSemaphore};
	VkPipelineStageFlags waitStages[] = {VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT};
	submitInfo.waitSemaphoreCount = 1;
	submitInfo.pWaitSemaphores = waitSemaphores;
	submitInfo.pWaitDstStageMask = waitStages;

	submitInfo.commandBufferCount = 1;
	submitInfo.pCommandBuffers = graphicsCommands->commandBuffers.data();

	VkSemaphore signalSemaphores[] = { renderFinishedSemaphore };
	submitInfo.signalSemaphoreCount = 1;
	submitInfo.pSignalSemaphores = signalSemaphores;


	if (vkQueueSubmit(queueWrapper->GetGraphicsQueue(), 1, &submitInfo, VK_NULL_HANDLE) != VK_SUCCESS)
	{
		throw std::runtime_error("failed to submit draw command buffer!");
	}

	VkPresentInfoKHR presentInfo = {};
	presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;

	presentInfo.waitSemaphoreCount = 1;
	presentInfo.pWaitSemaphores = signalSemaphores;

	VkSwapchainKHR swapChains[] = {swapchainWrapper->GetSwapchain()};
	presentInfo.swapchainCount = 1;
	presentInfo.pSwapchains = swapChains;
	presentInfo.pImageIndices = &imageIndex;

	presentInfo.pResults = nullptr; // Optional

	vkCmdBindPipeline(graphicsCommands->commandBuffers[imageIndex], VK_PIPELINE_BIND_POINT_GRAPHICS, pipeWrapper->GetCurrentPipe().graphicsPipeline);

	vkQueuePresentKHR(queueWrapper->GetPresentQueue(), &presentInfo);

	vkQueueWaitIdle(queueWrapper->GetPresentQueue());

	vkCmdDraw(graphicsCommands->commandBuffers[imageIndex], 3, 1, 0, 0);
}

void Vulkan_Graphics::CreateFramebuffers()
{
	swapchainFramebuffers.resize(swapchainWrapper->GetImageViews().size());

	for (size_t i = 0; i < swapchainWrapper->GetImageViews().size(); i++) {
		VkImageView attachments[] = {
			swapchainWrapper->GetImageViews()[i]
		};

		VkFramebufferCreateInfo framebufferInfo = {};
		framebufferInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
		framebufferInfo.renderPass = currentPipe.renderPass;
		framebufferInfo.attachmentCount = 1;
		framebufferInfo.pAttachments = attachments;
		framebufferInfo.width = swapchainWrapper->GetExtent().width;
		framebufferInfo.height = swapchainWrapper->GetExtent().height;
		framebufferInfo.layers = 1;

		if (vkCreateFramebuffer(logicalDevice, &framebufferInfo, nullptr, &swapchainFramebuffers[i]) != VK_SUCCESS) {
			throw std::runtime_error("failed to create framebuffer!");
		}
	}
}
*/

void Vulkan_Graphics::DrawFrame()
{

	//begin drawing frame
	uint32_t imageIndex;
	VkSwapchainKHR swapchain = swapchainWrapper->GetSwapchain();
	VkCommandBuffer cmdBuffer;
	Pipeline *pipe = &pipeWrapper->GetCurrentPipe();
	VkPresentInfoKHR presentInfo = {};
	VkSubmitInfo submitInfo = {};
	VkSemaphore waitSemaphores[] = { imageAvailableSemaphore };
	VkSemaphore signalSemaphores[] = { renderFinishedSemaphore };
	VkPipelineStageFlags waitStages[] = { VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT };

	vkAcquireNextImageKHR(logicalDevice,
		swapchain,
		std::numeric_limits<uint32_t>::max(),
		imageAvailableSemaphore,
		VK_NULL_HANDLE,
		&imageIndex);

	//cmdBuffer = cmdWrapper->CommandBeginSingleTime(graphicsCommands);

	//cmdWrapper->ConfigureRenderPass(cmdBuffer, 
		//pipe->renderPass, 
		//swapchainWrapper->GetFrameBuffers()[imageIndex], 
		//pipe->graphicsPipeline,
		//swapchainWrapper->GetExtent());



	//end drawing frame
	//cmdWrapper->ConfigureRenderPassEnd(cmdBuffer);
	//cmdWrapper->CommandEndSingleTime(graphicsCommands, cmdBuffer, queueWrapper->GetGraphicsQueue(), logicalDevice);


	submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;

	submitInfo.waitSemaphoreCount = 1;
	submitInfo.pWaitSemaphores = waitSemaphores;
	submitInfo.pWaitDstStageMask = waitStages;

	submitInfo.commandBufferCount = 1;// cmdWrapper->GetUsedBufferCount(graphicsCommands);
	submitInfo.pCommandBuffers = &graphicsCommandBuffers[imageIndex];

	submitInfo.signalSemaphoreCount = 1;
	submitInfo.pSignalSemaphores = signalSemaphores;

	if (vkQueueSubmit(queueWrapper->GetGraphicsQueue(), 1, &submitInfo, VK_NULL_HANDLE) != VK_SUCCESS)
	{
		slog("failed to submit draw command buffer!");
	}

	presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;

	presentInfo.waitSemaphoreCount = 1;
	presentInfo.pWaitSemaphores = signalSemaphores;

	presentInfo.swapchainCount = 1;
	presentInfo.pSwapchains = &swapchain;
	presentInfo.pImageIndices = &imageIndex;
	presentInfo.pResults = NULL; // Optional

	vkQueuePresentKHR(queueWrapper->GetPresentQueue(), &presentInfo);
}