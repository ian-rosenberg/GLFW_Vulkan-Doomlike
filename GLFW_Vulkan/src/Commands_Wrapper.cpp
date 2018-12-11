#include "Commands_Wrapper.h"
#include "simple_logger.h"

Commands_Wrapper::Commands_Wrapper()
{
	for (Command command : commandList)
	{ 
		command._inuse = 0;
		command.commandBufferCount = 0;
		command.commandBufferNext = 0;
	}

	max_commands = 0;

	device = VK_NULL_HANDLE;
}

Commands_Wrapper::~Commands_Wrapper()
{
	if (max_commands)
	{
		for (int i = 0; i < max_commands; ++i)
		{
			if (commandList[i]._inuse)
			{				
				vkDestroyCommandPool(device, commandList[i].commandPool, NULL);
			}
		}
	}
}

void Commands_Wrapper::CommandsWrapperInit(uint32_t count, VkDevice defaultDevice)
{
	slog("command pool system init");
	if (!count)
	{
		slog("cannot initliaze 0 command pools");
		return;
	}
	device = defaultDevice;
	max_commands = count;
	graphicsCommandIndex = -1;
	commandList.resize(max_commands);
}

/*Command* Commands_Wrapper::GraphicsCommandPoolSetup(uint32_t count, Pipeline *pipe, uint32_t graphicsFamily)
{
	Command *com;
	VkCommandPoolCreateInfo poolInfo = {};
	VkCommandBufferAllocateInfo allocInfo = {};

	com = NewCommandPool();

	poolInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
	poolInfo.queueFamilyIndex = graphicsFamily;
	poolInfo.flags = 0; // Optional    

	if (vkCreateCommandPool(device, &poolInfo, NULL, &com->commandPool) != VK_SUCCESS)
	{
		slog("failed to create command pool!");
		return {};
	}

	com->commandBuffers = std::vector<VkCommandBuffer>(count);

	allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
	allocInfo.commandPool = com->commandPool;
	allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
	allocInfo.commandBufferCount = count;
	com->commandBufferCount = count;

	if (vkAllocateCommandBuffers(device, &allocInfo, com->commandBuffers.data()) != VK_SUCCESS)
	{
		slog("failed to allocate command buffers!");
		return{};
	}

	slog("created command buffer pool");

	return com;
}*/

void Commands_Wrapper::CreateCommandBuffers(std::vector<VkCommandBuffer> *theCmdBuffers, uint32_t swpchnFbs, std::vector<VkFramebuffer> fBuffers, Pipeline* pipe, VkExtent2D extents, VkCommandPool cmdPool, VkBuffer vertexBuffer)
{	
	/*theCmdBuffers->commandBuffers.resize(swpchnFbs);
	
	VkCommandBufferAllocateInfo allocInfo = {};
	allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
	allocInfo.commandPool = theCmdBuffers->commandPool;
	allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
	allocInfo.commandBufferCount = (uint32_t) theCmdBuffers->commandBuffers.size();
	theCmdBuffers->commandBufferCount = (uint32_t)theCmdBuffers->commandBuffers.size();

	if (vkAllocateCommandBuffers(device, &allocInfo, theCmdBuffers->commandBuffers.data()) != VK_SUCCESS) {
		throw std::runtime_error("failed to allocate command buffers!");
	}

	for (size_t i = 0; i < theCmdBuffers->commandBuffers.size(); i++) 
	{
		VkCommandBufferBeginInfo beginInfo = {};
		beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
		beginInfo.flags = VK_COMMAND_BUFFER_USAGE_SIMULTANEOUS_USE_BIT;
		beginInfo.pInheritanceInfo = nullptr; // Optional

		if (vkBeginCommandBuffer(theCmdBuffers->commandBuffers[i], &beginInfo) != VK_SUCCESS)
		{
			throw std::runtime_error("failed to begin recording command buffer!");
		}

		VkRenderPassBeginInfo renderPassInfo = {};
		renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
		renderPassInfo.renderPass = pipe->renderPass;
		renderPassInfo.framebuffer = fBuffers[i];
		renderPassInfo.renderArea.offset = { 0, 0 };
		renderPassInfo.renderArea.extent = extents;

		VkClearValue clearColor = { 0.0f, 0.0f, 0.0f, 1.0f };
		renderPassInfo.clearValueCount = 1;
		renderPassInfo.pClearValues = &clearColor;

		vkCmdBeginRenderPass(theCmdBuffers->commandBuffers[i], &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);

		vkCmdBindPipeline(theCmdBuffers->commandBuffers[i], VK_PIPELINE_BIND_POINT_GRAPHICS, pipe->graphicsPipeline);

		vkCmdDraw(theCmdBuffers->commandBuffers[i], 3, 1, 0, 0);

		vkCmdEndRenderPass(theCmdBuffers->commandBuffers[i]);

		if (vkEndCommandBuffer(theCmdBuffers->commandBuffers[i]) != VK_SUCCESS)
		{
			throw std::runtime_error("failed to record command buffer!");
		}
	}*/

	theCmdBuffers->resize(swpchnFbs);

	VkCommandBufferAllocateInfo allocInfo = {};
	allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
	allocInfo.commandPool = cmdPool;
	allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
	allocInfo.commandBufferCount = (uint32_t)theCmdBuffers->size();

	if (vkAllocateCommandBuffers(device, &allocInfo, theCmdBuffers->data()) != VK_SUCCESS) {
		throw std::runtime_error("failed to allocate command buffers!");
	}

	for (size_t i = 0; i < theCmdBuffers->size(); i++) {
		VkCommandBufferBeginInfo beginInfo = {};
		beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
		beginInfo.flags = VK_COMMAND_BUFFER_USAGE_SIMULTANEOUS_USE_BIT;

		if (vkBeginCommandBuffer((*theCmdBuffers)[i], &beginInfo) != VK_SUCCESS) {
			throw std::runtime_error("failed to begin recording command buffer!");
		}

		VkRenderPassBeginInfo renderPassInfo = {};
		renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
		renderPassInfo.renderPass = pipe->renderPass;
		renderPassInfo.framebuffer = fBuffers[i];
		renderPassInfo.renderArea.offset = { 0, 0 };
		renderPassInfo.renderArea.extent = extents;

		VkClearValue clearColor = { 0.7f, .5f, 0.125f, 1.0f };
		renderPassInfo.clearValueCount = 1;
		renderPassInfo.pClearValues = &clearColor;

		vkCmdBeginRenderPass((*theCmdBuffers)[i], &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);

		vkCmdBindPipeline((*theCmdBuffers)[i], VK_PIPELINE_BIND_POINT_GRAPHICS, pipe->graphicsPipeline);

		VkBuffer vertexBuffers[] = { vertexBuffer };
		VkDeviceSize offsets[] = { 0 };
		vkCmdBindVertexBuffers((*theCmdBuffers)[i], 0, 1, vertexBuffers, offsets);

		vkCmdDraw((*theCmdBuffers)[i], 3, 1, 0, 0);

		vkCmdEndRenderPass((*theCmdBuffers)[i]);

		if (vkEndCommandBuffer((*theCmdBuffers)[i]) != VK_SUCCESS) {
			throw std::runtime_error("failed to record command buffer!");
		}
	}
}

void Commands_Wrapper::CreateCommandPool(uint32_t graphicsFamily, VkCommandPool *cmdPool)
{
	//NewCommandPool();
	
	VkCommandPoolCreateInfo poolInfo = {};
	poolInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
	poolInfo.queueFamilyIndex = graphicsFamily;
	poolInfo.flags = 0; // Optional

	if (vkCreateCommandPool(device, &poolInfo, nullptr, cmdPool) != VK_SUCCESS) {
		throw std::runtime_error("failed to create command pool!");
	}


	//return &commandList[graphicsCommandIndex];
}

void Commands_Wrapper::ResetCommandPool(Command *com)
{
	if (!com)return;
	com->commandBufferNext = 0;
}

void Commands_Wrapper::ConfigureRenderPassEnd(VkCommandBuffer cmdBuffer)
{
	vkCmdEndRenderPass(cmdBuffer);
}

VkCommandBuffer Commands_Wrapper::CmdRenderBegin(uint32_t index, Pipeline *thePipe, VkFramebuffer fBuffer, VkPipeline pipeline, VkExtent2D extent, Command *graphicsPool)
{
	VkCommandBuffer commandBuffer;

	commandBuffer = CommandBeginSingleTime(graphicsPool);

	ConfigureRenderPass(
		commandBuffer,
		thePipe->renderPass,
		fBuffer,
		pipeline,
		extent);

	return commandBuffer;
}

Command* Commands_Wrapper::NewCommandPool()
{
	for (int i = 0; i < max_commands; ++i)
	{
		if (!commandList[i]._inuse)
		{
			commandList[i]._inuse = true;
			
			slog("Selected a new command pool");

			if (graphicsCommandIndex < 0)
			{
				graphicsCommandIndex = i;
			}

			return &commandList[i];
		}
	}

	slog("failed to get a new command pool, list full");
	return NULL;
}

void Commands_Wrapper::CmdRenderEnd(VkCommandBuffer cmdBuffer, Command *graphicsPool, VkQueue graphicsQueue)
{
	ConfigureRenderPassEnd(cmdBuffer);
	CommandEndSingleTime(graphicsPool, cmdBuffer, graphicsQueue, device);
}

void Commands_Wrapper::ConfigureRenderPass(VkCommandBuffer commandBuffer, VkRenderPass renderPass, VkFramebuffer framebuffer, VkPipeline graphicsPipeline, VkExtent2D extent)
{
	VkClearValue clearColor = { 0.05f, 0.1f, .27f, 1.0f };
	
	VkClearValue clearValues[1] = {};
	VkRenderPassBeginInfo renderPassInfo = {};

	clearValues[0].color.float32[3] = 1.0;
	//clearValues[1].depthStencil.depth = 1.0f;

	renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
	renderPassInfo.renderPass = renderPass;
	renderPassInfo.framebuffer = framebuffer;
	renderPassInfo.renderArea.offset.x = 0;
	renderPassInfo.renderArea.offset.y = 0;
	renderPassInfo.renderArea.extent = extent;
	renderPassInfo.clearValueCount = 1;
	renderPassInfo.pClearValues = &clearColor;

	vkCmdBeginRenderPass(commandBuffer, &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);
	vkCmdBindPipeline(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, graphicsPipeline);
}

VkCommandBuffer Commands_Wrapper::CommandBeginSingleTime(Command *cmd)
{
	VkCommandBufferAllocateInfo allocInfo = {};
	VkCommandBufferBeginInfo beginInfo = {};
	VkCommandBuffer commandBuffer;

	if (!cmd)
	{
		slog("com is NULL");
	}

	allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
	allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
	allocInfo.commandPool = cmd->commandPool;
	allocInfo.commandBufferCount = 1;

	vkAllocateCommandBuffers(device, &allocInfo, &commandBuffer);

	beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
	beginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;

	vkBeginCommandBuffer(commandBuffer, &beginInfo);

	return commandBuffer;
}

void Commands_Wrapper::CommandEndSingleTime(Command *com, VkCommandBuffer commandBuffer, VkQueue graphicsQueue, VkDevice lDevice)
{
	VkSubmitInfo submitInfo = {};

	vkEndCommandBuffer(commandBuffer);

	submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
	submitInfo.commandBufferCount = 1;
	submitInfo.pCommandBuffers = &commandBuffer;

	vkQueueSubmit(graphicsQueue, 1, &submitInfo, VK_NULL_HANDLE);
	vkQueueWaitIdle(graphicsQueue);

	vkFreeCommandBuffers(lDevice, com->commandPool, 1, &commandBuffer);
}

VkCommandBuffer* Commands_Wrapper::GetUsedCommandBuffers(Command *com)
{
	if (!com)
	{
		return {};
	}
	
	return com->commandBuffers.data();
}
