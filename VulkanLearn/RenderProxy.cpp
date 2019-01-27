#include "pch.h"
#include "RenderProxy.h"

const VkCommandBuffer & RenderProxy::GetCommandBuffer() const
{
	// TODO: 在此处插入 return 语句
	return m_CommandBuffers[0];
}

void RenderProxy::QueryCommandBuffer()
{
	m_CommandBuffers.resize(1);
	auto&& device = CORE->GetDevice();
	auto&& pool = CORE->GetCommandPool();
	NEW_ST(VkCommandBufferAllocateInfo, allocalInfo);
	allocalInfo.commandBufferCount = 1;
	allocalInfo.commandPool = pool;
	allocalInfo.level = VkCommandBufferLevel::VK_COMMAND_BUFFER_LEVEL_PRIMARY;
	allocalInfo.pNext = nullptr;
	allocalInfo.sType = VkStructureType::VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
	vkAllocateCommandBuffers(device, &allocalInfo, m_CommandBuffers.data());
}

void RenderProxy::BeginRender()
{
	vkResetCommandBuffer(m_CommandBuffers[0], VkCommandBufferResetFlagBits::VK_COMMAND_BUFFER_RESET_RELEASE_RESOURCES_BIT);

	NEW_ST(VkCommandBufferBeginInfo, beginInfo);
	beginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;
	beginInfo.pInheritanceInfo = nullptr;
	beginInfo.pNext = nullptr;
	beginInfo.sType = VkStructureType::VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
	vkBeginCommandBuffer(m_CommandBuffers[0], &beginInfo);
	
	NEW_ST(VkRenderPassBeginInfo, renderBeginInfo);
	renderBeginInfo.clearValueCount = 1;
	renderBeginInfo.pClearValues = &m_RenderImage.GetClearValue();
	renderBeginInfo.framebuffer = m_RenderImage.GetFrameBuffer() == VK_NULL_HANDLE ? CORE->GetCurrentFrameBuffer() : m_RenderImage.GetFrameBuffer();
	renderBeginInfo.pNext = nullptr;
	renderBeginInfo.renderArea.offset.x = 0;
	renderBeginInfo.renderArea.offset.y = 0;
	renderBeginInfo.renderArea.extent = m_RenderImage.GetFrameBuffer() == VK_NULL_HANDLE ? CORE->GetSwapChainExtent() : m_RenderImage.GetExtend();
	renderBeginInfo.renderPass = CORE->GetRenderPass();
	renderBeginInfo.sType = VkStructureType::VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
	
	vkCmdBeginRenderPass(m_CommandBuffers[0], &renderBeginInfo, VK_SUBPASS_CONTENTS_INLINE);
}

void RenderProxy::Render()
{
	// TODO: 遍历可被渲染的物体列表，录入以下命令：设置渲染管线并提交资源，然后渲染
}

void RenderProxy::EndRender()
{
	vkEndCommandBuffer(m_CommandBuffers[0]);
}

RenderProxy::RenderProxy()
{
	m_CommandBuffers.resize(1);
	m_CommandBuffers[0] = VK_NULL_HANDLE;
	CORE->AddRenderProxy(this);
	QueryCommandBuffer();
}

RenderProxy::~RenderProxy()
{
	if (m_CommandBuffers[0] != VK_NULL_HANDLE)
	{
		vkFreeCommandBuffers(CORE->GetDevice(), CORE->GetCommandPool(), 1, &m_CommandBuffers[0]);
		m_CommandBuffers[0] = VK_NULL_HANDLE;
	}
}


const VkExtent2D & RenderImage::GetExtend() const
{
	// TODO: 在此处插入 return 语句
	return m_Extends;
}

const VkFramebuffer & RenderImage::GetFrameBuffer() const
{
	// TODO: 在此处插入 return 语句
	return m_FrameBuffer;
}

const VkClearValue & RenderImage::GetClearValue() const
{
	// TODO: 在此处插入 return 语句
	return m_ClearValue;
}
