#pragma once

#include <vector>
#include <string>
#include <vulkan/vulkan.h>

#include "Buffers.h"
struct Model
{
	std::vector<Vertex>		vertices;
	std::vector<uint32_t>	indices;
	VkBuffer				vertexBuffer;
	VkDeviceMemory			vertexBufferMemory;
};

class Model_Manager
{
private:
	std::vector<Model>	modelList;

	Model				*testModel;

public:
	Model* LoadModel(const char* ModelName);
	Model* NewModel();
};