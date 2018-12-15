#include <stdio.h>
#include <iostream>
#include <vulkan/vulkan.h>

#include "GLFW_Wrapper.h"
#include "Vulkan_Graphics.h"
#include "simple_logger.h"

using namespace std;

int main(int agrc, char *argv[])
{
	//uint32_t bufferFrame = 0;
	//VkCommandBuffer commandBuffer;
	
	GLFW_Wrapper *glfwWrapper = new GLFW_Wrapper("Doomlike", 1280, 720, false);
	Vulkan_Graphics vGraphics = Vulkan_Graphics(glfwWrapper, true);

	init_logger("logFile.txt");

	while (!glfwWindowShouldClose(glfwWrapper->GetWindow()))
	{
		glfwPollEvents();

		vGraphics.DrawFrame();
	}

	slog("Ending program...");

	slog_sync();

	vkDeviceWaitIdle(vGraphics.GetLogicalDevice());

	return 0;
}