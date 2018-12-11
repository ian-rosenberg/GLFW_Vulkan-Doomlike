#pragma once

#include "Shader_Wrapper.h"


struct Pipeline
{
	bool                inUse;
	VkPipeline          graphicsPipeline;
	VkRenderPass        renderPass;
	VkPipelineLayout    pipelineLayout;
	size_t              vertSize;
	char               *vertShader;
	VkShaderModule      vertModule;
	size_t              fragSize;
	char               *fragShader;
	VkShaderModule      fragModule;
	VkDevice            device;

	Pipeline()
	{
		this->inUse = false;
		this->vertModule = VK_NULL_HANDLE;
		this->fragModule = VK_NULL_HANDLE;
	}
};


class Pipeline_Wrapper
{
private:
	Shader_Wrapper			*shaderWrapper;
	
	uint32_t				maxPipes;
	std::vector<Pipeline>	pipelineList;

	VkDevice				logicalDevice;

	uint32_t				graphicsPipelineIndex;

public:
	Pipeline_Wrapper();
	~Pipeline_Wrapper();

	void Pipeline_WrapperInit(uint32_t maxPipelines);

	void PipelineLoad(VkDevice device, char* vertFile, char* fragFile, VkFormat format, VkPhysicalDevice physDevice, VkExtent2D extents );

	Pipeline* NewPipe();

	void RenderPassSetup(VkFormat format, VkPhysicalDevice physDevice, VkDevice lDevice);

	VkFormat FindDepthFormat(VkPhysicalDevice physDevice);

	VkFormat FindSupportedFormat(VkFormat* candidates, uint32_t candidateCount, VkImageTiling tiling, VkFormatFeatureFlags features, VkPhysicalDevice physDevice);

	Pipeline GetCurrentPipe() { return pipelineList[graphicsPipelineIndex]; }

	VkPipelineLayout GetPipelineLayout() { return pipelineList[graphicsPipelineIndex].pipelineLayout; }

	static std::vector<char> ReadShaderFile(const std::string& filename);
};