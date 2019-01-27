#pragma once
#include "VulkanCore.h"

class RenderImage
{
private:
	VkFramebuffer m_FrameBuffer;
	VkClearValue m_ClearValue;
	VkExtent2D m_Extends;
public:
	const VkExtent2D& GetExtend() const;
	const VkFramebuffer& GetFrameBuffer() const;
	const VkClearValue& GetClearValue() const;
};

class RenderProxy
{
private:
	std::vector<VkCommandBuffer> m_CommandBuffers;
	RenderImage m_RenderImage;
public:
	const VkCommandBuffer& GetCommandBuffer() const;
	void QueryCommandBuffer();
	void BeginRender();
	void Render();
	void EndRender();

	RenderProxy();
	virtual ~RenderProxy();
};