#include "pch.h"
#include "VulkanCore.h"
#include <iostream>
#include "VkStringEnum.h"

VulkanCore* VulkanCore::core = nullptr;
std::mutex VulkanCore::lock;
VulkanCore::VulkanCore() : m_PhysicsDevicesIninted(false), m_Instance(VK_NULL_HANDLE), m_Device(VK_NULL_HANDLE), m_UsedQueueId(),m_Surface(VK_NULL_HANDLE),
	m_hWnd(0), m_hInstance(0)
{
	InitDevice(0);
}

void VulkanCore::InitInstance()
{
	if (m_Instance != VK_NULL_HANDLE)
		return;
	NEW_ST(VkApplicationInfo, appInfo);
	appInfo.apiVersion = VK_VERSION_1_0;
	appInfo.applicationVersion = VK_MAKE_VERSION(0, 0, 1);
	appInfo.engineVersion = VK_MAKE_VERSION(0, 0, 1);
	appInfo.pApplicationName = "VulkanLearn";
	appInfo.pEngineName = "VulkanLearnEngine";
	appInfo.pNext = 0;
	appInfo.sType = VkStructureType::VK_STRUCTURE_TYPE_APPLICATION_INFO;

	std::vector<const char*> khr_extensions = {
		VK_KHR_SURFACE_EXTENSION_NAME,
		VK_KHR_WIN32_SURFACE_EXTENSION_NAME
	};

	NEW_ST(VkInstanceCreateInfo, instanceCreateInfo);
	instanceCreateInfo.pNext = 0;
	instanceCreateInfo.sType = VkStructureType::VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
	instanceCreateInfo.enabledExtensionCount = khr_extensions.size();
	instanceCreateInfo.enabledLayerCount = 0;
	instanceCreateInfo.flags = 0;
	instanceCreateInfo.pApplicationInfo = &appInfo;
	instanceCreateInfo.ppEnabledExtensionNames = khr_extensions.data();
	instanceCreateInfo.ppEnabledLayerNames = 0;
	
	vkCreateInstance(&instanceCreateInfo, nullptr, &m_Instance);
}

void VulkanCore::ShowGlobalLayersAndExtensions()
{
	uint32_t globalLayerCount = 0;
	vkEnumerateInstanceLayerProperties(&globalLayerCount, nullptr);
	std::vector<VkLayerProperties> layers;
	layers.resize(globalLayerCount);
	vkEnumerateInstanceLayerProperties(&globalLayerCount, layers.data());

	{
		uint32_t extensionCount;
		vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, nullptr);
		std::vector<VkExtensionProperties> extensions;
		extensions.resize(extensionCount);
		vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, extensions.data());
		std::cout << "Null Layer Has " << extensionCount << " Extensions:" << std::endl;
		for (int i = 0; i < extensions.size(); ++i)
		{
			std::cout << "\t" << extensions[i].extensionName << std::endl;
		}
		std::cout << std::endl;
	}

	for (int i = 0; i < layers.size(); ++i)
	{
		uint32_t extensionCount;
		vkEnumerateInstanceExtensionProperties(layers[i].layerName, &extensionCount, nullptr);
		std::vector<VkExtensionProperties> extensions;
		extensions.resize(extensionCount);
		vkEnumerateInstanceExtensionProperties(layers[i].layerName, &extensionCount, extensions.data());
		std::cout << layers[i].layerName << " Layer Has " << extensionCount << " Extensions:" << std::endl;
		for (int i = 0; i < extensions.size(); ++i)
		{
			std::cout << "\t" << extensions[i].extensionName << std::endl;
		}
		std::cout << std::endl;
	}
}

void VulkanCore::ShowPhysicsDevicesInfo()
{
	InitPhysicsDevices();
	for (int i = 0; i < m_PhysicsDevices.size(); ++i)
	{
		auto& phyDev = m_PhysicsDevices[i];
		NEW_ST(VkPhysicalDeviceProperties, property);
		vkGetPhysicalDeviceProperties(phyDev, &property);
		std::cout << "Device " << property.deviceName << " At Position " << property.deviceID << std::endl;
		uint32_t layersCount = 0;
		vkEnumerateDeviceLayerProperties(phyDev, &layersCount, nullptr);
		std::vector<VkLayerProperties> layers;
		layers.resize(layersCount);
		vkEnumerateDeviceLayerProperties(phyDev, &layersCount, layers.data());
		std::cout << "\tWith " << layers.size() << " Layers" << std::endl;
		{
			uint32_t extensionCount = 0;
			vkEnumerateDeviceExtensionProperties(phyDev, nullptr, &extensionCount, nullptr);
			std::vector<VkExtensionProperties> extensions;
			extensions.resize(extensionCount);
			vkEnumerateDeviceExtensionProperties(phyDev, nullptr, &extensionCount, extensions.data());
			std::cout << "\t\tAt Null Layer Has " << extensionCount << " Extensions" << std::endl;
			for (int extensionId = 0; extensionId < extensions.size(); ++extensionId)
			{
				std::cout << "\t\t\t" << extensions[extensionId].extensionName << std::endl;
			}
			std::cout << std::endl;
		}

		for (int layerId = 0; layerId < layers.size(); ++layerId)
		{
			uint32_t extensionCount = 0;
			vkEnumerateDeviceExtensionProperties(phyDev, layers[layerId].layerName, &extensionCount, nullptr);
			std::vector<VkExtensionProperties> extensions;
			extensions.resize(extensionCount);
			vkEnumerateDeviceExtensionProperties(phyDev, layers[layerId].layerName, &extensionCount, extensions.data());
			std::cout << "\t\tAt "<< layers[layerId].layerName <<" Layer Has " << extensionCount << " Extensions" << std::endl;
			for (int extensionId = 0; extensionId < extensions.size(); ++extensionId)
			{
				std::cout << "\t\t\t" << extensions[extensionId].extensionName << std::endl;
			}
			std::cout << std::endl;
		}
		
		{
			uint32_t queueFamilyCount = 0;
			vkGetPhysicalDeviceQueueFamilyProperties(phyDev, &queueFamilyCount, nullptr);
			std::vector<VkQueueFamilyProperties> queueFamilies;
			queueFamilies.resize(queueFamilyCount);
			vkGetPhysicalDeviceQueueFamilyProperties(phyDev, &queueFamilyCount, queueFamilies.data());
			std::cout << "\tWith " << queueFamilyCount << " Queue Families" << std::endl;
			for (int queueFimilyId = 0; queueFimilyId < queueFamilies.size(); ++queueFimilyId)
			{
				std::cout << "\t\tNo."<< queueFimilyId << " Support Flags: " << VkStringEnum::CheckSupportQueueFlag(queueFamilies[queueFimilyId]) << std::endl;	
			}
		}
		
	}
}

bool VulkanCore::ShowWindow()
{
	if (m_WindObj == nullptr)
		return false;
	m_WindObj->Show(SW_SHOW);
	return true;
}

void VulkanCore::Run()
{
	MSG message = {};
	while (message.message != WM_QUIT)
	{
		if (PeekMessage(&message, NULL, 0, 0, PM_REMOVE))
		{
			TranslateMessage(&message);
			DispatchMessage(&message);
		}
		else
		{

		}
	}
}

VulkanCore * VulkanCore::GetCore()
{
	if (!core)
	{
		lock.lock();
		if (!core)
		{
			core = new VulkanCore();
		}
		lock.unlock();
	}
	return core;
}

void VulkanCore::InitPhysicsDevices()
{
	InitInstance();
	if (!m_PhysicsDevicesIninted)
	{
		uint32_t physicsDevicesCount = 0;
		vkEnumeratePhysicalDevices(m_Instance, &physicsDevicesCount, nullptr);
		m_PhysicsDevices.resize(physicsDevicesCount);
		vkEnumeratePhysicalDevices(m_Instance, &physicsDevicesCount, m_PhysicsDevices.data());
		m_PhysicsDevicesIninted = true;
	}
}

void VulkanCore::InitDevice(uint32_t index)
{
	InitPhysicsDevices();
	CreateSurfaceKHR();
	if (m_Device == VK_NULL_HANDLE)
	{
		m_UsedQueueId.clear();
		auto& phyDev = m_PhysicsDevices[index];
		// 找到支持渲染的队列族
		{
			uint32_t queueFamilyCount = 0;
			vkGetPhysicalDeviceQueueFamilyProperties(phyDev, &queueFamilyCount, nullptr);
			std::vector<VkQueueFamilyProperties> queueFamilies;
			queueFamilies.resize(queueFamilyCount);
			vkGetPhysicalDeviceQueueFamilyProperties(phyDev, &queueFamilyCount, queueFamilies.data());
			//std::cout << "\tWith " << queueFamilyCount << " Queue Families" << std::endl;
			for (int queueFimilyId = 0; queueFimilyId < queueFamilies.size(); ++queueFimilyId)
			{
				if (queueFamilies[queueFimilyId].queueFlags & VkQueueFlagBits::VK_QUEUE_GRAPHICS_BIT)
				{
					m_UsedQueueId.insert(queueFimilyId);
					std::cout << "VK_QUEUE_GRAPHICS_BIT Support At Queue Index " << queueFimilyId << std::endl;
					break;
				}
			}
		}
		// 找到支持呈现的队列族
		{
			uint32_t queueFamilyCount = 0;
			vkGetPhysicalDeviceQueueFamilyProperties(phyDev, &queueFamilyCount, nullptr);
			
			for (int index = 0; index < queueFamilyCount; ++index)
			{
				VkBool32 sur = VK_FALSE;
				vkGetPhysicalDeviceSurfaceSupportKHR(phyDev, index, m_Surface, &sur);
				if (sur == VK_TRUE)
				{
					m_UsedQueueId.insert(sur);
					std::cout << "Surface Support At Queue Index " << index << std::endl;
					break;
				}
			}
		}

		VkPhysicalDeviceFeatures features;
		vkGetPhysicalDeviceFeatures(phyDev, &features);
		
		std::vector<VkDeviceQueueCreateInfo> creatInfos;
		for (auto& i : m_UsedQueueId)
		{
			NEW_ST(VkDeviceQueueCreateInfo, queueCreateInfo);
			queueCreateInfo.flags = 0;
			queueCreateInfo.pNext = 0;
			queueCreateInfo.pQueuePriorities = nullptr;
			queueCreateInfo.queueCount = 1;
			queueCreateInfo.queueFamilyIndex = i;
			queueCreateInfo.sType = VkStructureType::VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;

			creatInfos.push_back(queueCreateInfo);
		}

		
		
		std::vector<const char*> extensionNames =
		{
			VK_KHR_SWAPCHAIN_EXTENSION_NAME
		};

		NEW_ST(VkDeviceCreateInfo, deviceCreateInfo);
		deviceCreateInfo.enabledExtensionCount = 1;
		deviceCreateInfo.enabledLayerCount = 0;
		deviceCreateInfo.flags = 0;
		deviceCreateInfo.pEnabledFeatures = &features;
		deviceCreateInfo.pNext = 0;
		deviceCreateInfo.ppEnabledExtensionNames = extensionNames.data();
		deviceCreateInfo.ppEnabledLayerNames = 0;
		deviceCreateInfo.pQueueCreateInfos = creatInfos.data();
		deviceCreateInfo.queueCreateInfoCount = creatInfos.size();
		deviceCreateInfo.sType = VkStructureType::VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
		
		auto res = vkCreateDevice(phyDev, &deviceCreateInfo, nullptr, &m_Device);
		if (res == VkResult::VK_SUCCESS)
		{
			vkGetDeviceQueue(m_Device,0,  0, &m_Queue);
			std::cout << "Success Create Device" << std::endl;
		}
		else
		{
			std::cout << "Failed Create Device, ErrorCode " << res << std::endl;
		}
	}
}
#pragma region Platform相关
void VulkanCore::CreateAppWindow()
{
	if (m_WindObj == nullptr)
	{
		m_WindObj = new WindowsWnd(0, 0, 800, 600);
		m_hWnd = m_WindObj->GetWindow();
	}
}

void VulkanCore::CreateSurfaceKHR()
{
	InitInstance();
	CreateAppWindow();
	if (m_Surface == VK_NULL_HANDLE)
	{
		NEW_ST(VkWin32SurfaceCreateInfoKHR, surfaceCreateInfo);
		surfaceCreateInfo.flags = 0;
		surfaceCreateInfo.sType = VkStructureType::VK_STRUCTURE_TYPE_WIN32_SURFACE_CREATE_INFO_KHR;
		surfaceCreateInfo.pNext = 0;
		surfaceCreateInfo.hwnd = m_hWnd;
		surfaceCreateInfo.hinstance = GetModuleHandle(NULL);

		vkCreateWin32SurfaceKHR(m_Instance, &surfaceCreateInfo, nullptr, &m_Surface);
	}
	
}

#pragma endregion
