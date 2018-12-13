#pragma once

#include "Pipeline_Wrapper.h"
#include "Queue_Wrapper.h"

struct Command
{
	uint8_t									_inuse;
	VkCommandPool							commandPool;
	std::vector<VkCommandBuffer>			commandBuffers;
	uint32_t								commandBufferCount;
	uint32_t								commandBufferNext;

	Command()
	{
		_inuse = false;
		commandPool = VK_NULL_HANDLE;
		commandBuffers = {};
		commandBufferCount = 0;
		commandBufferNext = 0;
	}
};

class Commands_Wrapper
{
private:
	std::vector<Command>		commandList;
	uint32_t					max_commands;
	VkDevice					device;
	int							graphicsCommandIndex;
	int							stagingCommandBufferIndex;
public:
	Commands_Wrapper();
	~Commands_Wrapper();

	void CommandsWrapperInit(uint32_t count, VkDevice defaultDevice);

	Command* NewCommandPool();

	//Command* GraphicsCommandPoolSetup(uint32_t count, Pipeline *pipe, uint32_t graphicsFamily);

	Command* CreateCommandPool(uint32_t graphicsFamily, VkCommandPoolCreateFlags flags);

	void CreateCommandBuffers(Command *cmd, uint32_t swpchnFbs, std::vector<VkFramebuffer> fBuffers, Pipeline* pipe, VkExtent2D extents, VkBuffer vertexBuffer);

	void ResetCommandPool(Command *com);

	std::vector<VkCommandBuffer> GetGraphicsBuffer(){ return commandList[graphicsCommandIndex].commandBuffers; }
	std::vector<VkCommandBuffer> GetVertexBuffer(){ return commandList[stagingCommandBufferIndex].commandBuffers; }
	VkCommandBuffer* GetCommandBuffers(uint32_t index){ return commandList[index].commandBuffers.data(); }
	uint32_t GetUsedBufferCount(Command *com){ return !com ? com->commandBufferNext : 0; }
	VkCommandBuffer* GetUsedCommandBuffers(Command *com);


	void ConfigureRenderPass(VkCommandBuffer commandBuffer, VkRenderPass renderPass, VkFramebuffer framebuffer, VkPipeline graphicsPipeline, VkExtent2D extent);

	void ConfigureRenderPassEnd(VkCommandBuffer cmdBuffer);

	VkCommandBuffer CmdRenderBegin(uint32_t index, Pipeline *thePipe, VkFramebuffer fBuffer, VkPipeline pipeline, VkExtent2D extent, Command *graphicsPool);
	void CmdRenderEnd(VkCommandBuffer cmdBuffer, Command *graphicsPool, VkQueue graphicsQueue);


	void CommandEndSingleTime(Command *com, VkCommandBuffer commandBuffer, VkQueue graphicsQueue, VkDevice lDevice);

	VkCommandBuffer CommandBeginSingleTime(Command *cmd);
};