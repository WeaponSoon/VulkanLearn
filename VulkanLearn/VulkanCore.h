#pragma once
#include <vulkan/vulkan.h>
#include <vector>
#include <Windows.h>
#include <set>
#include <mutex>
#include <string>
#include <tchar.h>
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
	VkQueue m_Queue;
	bool m_PhysicsDevicesIninted;
	std::set<uint32_t> m_UsedQueueId;
	VkSurfaceKHR m_Surface;
	VkSwapchainKHR m_SwapChain;
	uint32_t m_UsedPhysicalDevice;
	
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
	
	void Run();

	static VulkanCore* GetCore();
	
private:
	void InitInstance();
	void InitPhysicsDevices();
	void InitDevice(uint32_t whichPhysicalDevice);
	void CreateAppWindow();
	void CreateSurfaceKHR();
	void Resize(int w, int h);


	VulkanCore();
	VulkanCore(const VulkanCore&) = delete;
	VulkanCore(VulkanCore&&) = delete;
	VulkanCore& operator==(const VulkanCore&) = delete;
	VulkanCore& operator==(VulkanCore&&) = delete;
};