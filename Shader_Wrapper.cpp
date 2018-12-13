#include <fstream>

#include "Shader_Wrapper.h"
#include "gf3d_types.h"
#include "simple_logger.h"

Shader_Wrapper::Shader_Wrapper()
{
}

Shader_Wrapper::Shader_Wrapper(char** fileNames)
{

}

Shader_Wrapper::~Shader_Wrapper()
{

}

char* Shader_Wrapper::LoadShaderData(const char* filename, size_t *rsize)
{

	char* buffer = {};
	FILE *file;
	size_t size;
	file = fopen(filename, "rb");
	if (!file)
	{
		slog("failed to open shader file %s", filename);
		return NULL;
	}
	fseek(file, 0, SEEK_END);
	size = ftell(file);
	if (!size)
	{
		slog("file %s contained no data", filename);
		fclose(file);
		return NULL;
	}
	rewind(file);
	buffer = new char[size];
	if (!buffer)
	{
		slog("failed to allocate memory for shader file %s", filename);
		fclose(file);
		return NULL;
	}
	fread(buffer, size, 1, file);
	fclose(file);
	if (rsize)*rsize = size;
	return buffer;
}

VkShaderModule Shader_Wrapper::CreateShaderModule(const std::vector<char>& shader, VkDevice device)
{
	VkShaderModule module = {};
	VkShaderModuleCreateInfo createInfo = {};

	createInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
	createInfo.codeSize = shader.size();
	createInfo.pCode = reinterpret_cast<const uint32_t*>(shader.data());

	if (vkCreateShaderModule(device, &createInfo, NULL, &module) != VK_SUCCESS)
	{
		slog("failed to create shader module");
	}
	return module;
}
