#include <stdio.h>

#include "Vulkan_Graphics.h"
#include "GLFW_Wrapper.h"
#include "simple_logger.h"

GLFW_Wrapper::GLFW_Wrapper(char* appName, uint32_t width, uint32_t height, bool fullscreen)
{
	CreateWindow(appName, width, height, fullscreen);
}

void GLFW_Wrapper::CreateWindow(char* appName, uint32_t renderWidth, uint32_t renderHeight, bool fullscreen)
{
	if (!glfwInit())
	{
		slog("GLFW failed to init");
	}
	else
	{
		slog("GLFW init!");
	}
		
	if (glfwVulkanSupported())
	{
		slog("Vulkan & GLFW are available");
	}
	else
	{
		slog("Vulkan not supported!");
	}
		
	glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);

	window = glfwCreateWindow(renderWidth, renderHeight, appName, NULL, NULL);
	glfwSetWindowUserPointer(window, this);
	glfwSetFramebufferSizeCallback(window, framebufferResizeCallback);

	if (!window)
	{
		slog("Window creation failed");
	}
	else
	{
		slog("Window created");
	}
}

VkSurfaceKHR GLFW_Wrapper::CreateGLFWWindowSurface(VkInstance instance)
{
	glfwCreateWindowSurface(instance, window, NULL, &surface);

	return surface;
}

GLFW_Wrapper::~GLFW_Wrapper()
{
	glfwDestroyWindow(window);
	
	glfwTerminate();
}

void GLFW_Wrapper::framebufferResizeCallback(GLFWwindow* window, int width, int height) {
	auto app = reinterpret_cast<Vulkan_Graphics*>(glfwGetWindowUserPointer(window));
	app->framebufferResized = true;
}
