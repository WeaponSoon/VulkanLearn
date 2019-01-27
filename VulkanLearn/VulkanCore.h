#pragma once
#include <vulkan/vulkan.h>
#include <vector>
#include <Windows.h>
#include <set>
#include <mutex>
#include <string>
#include <tchar.h>
#include "RenderProxy.h"
#include "WindowsWnd.h"
#define NEW_ST(st, ins)\
	st ins;\
	ZeroMemory(&ins, sizeof(st));
#define CORE VulkanCore::GetCore()
class VulkanCore
{
private:
	VkInstance m_Instance;
	std::vector<VkPhysicalDevice> m_PhysicsDevices;
	VkDevice m_Device;
	VkQueue m_GraphicsQueue;
	VkQueue m_PresentQueue;
	bool m_PhysicsDevicesIninted;
	std::set<uint32_t> m_UsedQueueId;
	VkSurfaceKHR m_Surface;
	VkSwapchainKHR m_SwapChain;
	uint32_t m_UsedPhysicalDevice;
	std::vector<VkImage> m_SwapChainImages;
	VkExtent2D m_SwapChainExtent2D;
	VkFormat m_SwapChainFormat;
	std::vector<VkImageView> m_SwapChainImageViews;
	VkCommandPool m_CommandPool;
	uint32_t m_GraphicsFamily;
	uint32_t m_PresentFamily;
	VkRenderPass m_RenderPass;
	std::vector<VkFramebuffer> m_FrameBuffers;
	std::vector<RenderProxy*> m_RenderProxys;
	std::vector<VkCommandBuffer> m_TempProxyCommandBuffer;
	std::vector<VkSemaphore> m_DrawSemaphores;
	std::vector<VkSemaphore> m_PresentSemaphores;
	std::vector<VkFence> m_Fences;
	uint32_t m_CurrentFrameIndex;

#pragma region Platformœ‡πÿ
	HWND m_hWnd;
	HINSTANCE m_hInstance;
	WindowsWnd* m_WindObj;
#pragma endregion


	static VulkanCore* core;
	static std::mutex lock;
public:



	void ShowGlobalLayersAndExtensions();
	void ShowPhysicsDevicesInfo();
	bool ShowWindow();
	

	void DrawFrame();

	void AddRenderProxy(const RenderProxy* proxy);
	const std::vector<RenderProxy*> GetRenderProxys() const;

	void Run();

	const VkDevice& GetDevice() const;
	const VkCommandPool& GetCommandPool() const;
	const VkRenderPass& GetRenderPass() const;
	const VkFramebuffer& GetCurrentFrameBuffer() const;
	const VkExtent2D& GetSwapChainExtent() const;

	static VulkanCore* GetCore();
	
private:
	void InitInstance();
	void InitPhysicsDevices();
	void InitDevice(uint32_t whichPhysicalDevice);
	void CreateAppWindow();
	void CreateSurfaceKHR();
	void Resize(int w, int h);
	void CreateCommandBuffersForSwapChain();
	void InitSyncSemaphore();
	void InitSyncFence();


	VulkanCore();
	VulkanCore(const VulkanCore&) = delete;
	VulkanCore(VulkanCore&&) = delete;
	VulkanCore& operator==(const VulkanCore&) = delete;
	VulkanCore& operator==(VulkanCore&&) = delete;
};