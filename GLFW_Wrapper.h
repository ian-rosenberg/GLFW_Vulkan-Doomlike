#pragma once

#define GLFW_INCLUDE_VULKAN

#include <GLFW/glfw3.h>

class GLFW_Wrapper
{
private:
	GLFWwindow						*window;
	VkSurfaceKHR					surface;
	const char						*applicationName;
	int								screenWidth;
	int								screenHeight;

public:
	GLFW_Wrapper(char* appName, uint32_t width, uint32_t height, bool fullscreen);
	~GLFW_Wrapper();

	void CreateWindow(char* appName, uint32_t renderWidth, uint32_t renderHeight, bool fullscreen);

	const char* GetWindowName() { return applicationName; }

	int GetWindowWidth() { return screenWidth; }
	int GetWindowHeight() { return screenHeight; }

	GLFWwindow* GetWindow() { return window; }

	VkSurfaceKHR CreateGLFWWindowSurface(VkInstance instance);

	static void framebufferResizeCallback(GLFWwindow* window, int width, int height);
};