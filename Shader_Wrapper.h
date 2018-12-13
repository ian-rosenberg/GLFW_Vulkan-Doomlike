#pragma once

#include <vulkan/vulkan.h>
#include <vector>


class Shader_Wrapper
{
private:
	std::vector<VkShaderModule>			shaders;
	const char**						shaderNames;
	uint32_t							shaderNameNum;
	VkDevice							device;

public:
	Shader_Wrapper();
	Shader_Wrapper(char** fileNames);
	~Shader_Wrapper();

	char* LoadShaderData(const char* filename, size_t *rsize);

	VkShaderModule CreateShaderModule(const std::vector<char>& shader, VkDevice device);
};