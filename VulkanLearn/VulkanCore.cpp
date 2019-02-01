#include "pch.h"
#include "VulkanCore.h"
#include <iostream>
#include <array>
#include "VkStringEnum.h"
#include "RenderProxy.h"
VulkanCore* VulkanCore::core = nullptr;
std::mutex VulkanCore::lock;
uint32_t VulkanCore::ShadowPassIndex = 0;
uint32_t VulkanCore::GeometryPassIndex = 1;
uint32_t VulkanCore::TransparentPassIndex = 2;
uint32_t VulkanCore::MaxRenderMaxIndex = 3;

VulkanCore::VulkanCore() : m_PhysicsDevicesIninted(false), m_Instance(VK_NULL_HANDLE), m_Device(VK_NULL_HANDLE), m_UsedQueueId(),m_Surface(VK_NULL_HANDLE),
	m_hWnd(0), m_hInstance(0), m_SwapChain(VK_NULL_HANDLE), m_UsedPhysicalDevice(0xffffffff)
{
	InitDevice(0);
	CreateCommandBuffersForSwapChain();
	m_RenderPass.resize(MaxRenderMaxIndex);
	InitGeometryPass();
	InitTransparentPass();
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
	Resize(0, 0);
	return true;
}

void VulkanCore::DrawFrame()
{
	m_TempProxyCommandBuffer.clear();
	
	
	uint32_t imageIndex = 0;
	vkAcquireNextImageKHR(m_Device, m_SwapChain, 0xffffffff, m_PresentSemaphores[m_CurrentFrameIndex], VK_NULL_HANDLE, &imageIndex);

	
	for (int i = 0; i < m_RenderProxys.size(); ++i)
	{
		if (m_RenderProxys[i]->GetCommandBuffer() != VK_NULL_HANDLE)
		{
			auto&& commandQueue = m_RenderProxys[i]->GetCommandBuffer();
			m_TempProxyCommandBuffer.push_back(commandQueue);
			m_RenderProxys[i]->BeginRender();
			m_RenderProxys[i]->Render();
			m_RenderProxys[i]->EndRender();
		}
	}
	
	NEW_ST(VkSubmitInfo, submitInfo);
	submitInfo.commandBufferCount = m_TempProxyCommandBuffer.size();
	submitInfo.pCommandBuffers = m_TempProxyCommandBuffer.data();
	submitInfo.pNext = nullptr;
	submitInfo.pSignalSemaphores = &m_DrawSemaphores[m_CurrentFrameIndex];
	VkPipelineStageFlags stages[] = { VkPipelineStageFlagBits::VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT };
	submitInfo.pWaitDstStageMask = stages;
	submitInfo.pWaitSemaphores = &m_PresentSemaphores[m_CurrentFrameIndex];
	submitInfo.signalSemaphoreCount = 1;
	submitInfo.sType = VkStructureType::VK_STRUCTURE_TYPE_SUBMIT_INFO;
	submitInfo.waitSemaphoreCount = 1;
	vkQueueSubmit(m_GraphicsQueue, 1, &submitInfo, VK_NULL_HANDLE);

	NEW_ST(VkPresentInfoKHR, presentInfo);
	presentInfo.pImageIndices = &imageIndex;
	presentInfo.pNext = nullptr;
	presentInfo.pResults = 0;
	presentInfo.pSwapchains = &m_SwapChain;
	presentInfo.pWaitSemaphores = &m_DrawSemaphores[m_CurrentFrameIndex];
	presentInfo.sType = VkStructureType::VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
	presentInfo.swapchainCount = 1;
	presentInfo.waitSemaphoreCount = 1;
	
	vkQueuePresentKHR(m_PresentQueue, &presentInfo);
	m_CurrentFrameIndex = (m_CurrentFrameIndex + 1) % m_FrameBuffers.size();
}

void VulkanCore::AddRenderProxy(const RenderProxy * proxy)
{
	bool has = false;
	for (auto&& i : m_RenderProxys)
	{
		if (i == proxy)
		{
			has = true;
			break;
		}
	}
	if (!has)
	{
		m_RenderProxys.push_back(const_cast<RenderProxy*>(proxy));
	}
}

const std::vector<RenderProxy*> VulkanCore::GetRenderProxys() const
{
	return m_RenderProxys;
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
			DrawFrame();
		}
	}
}

const VkDevice & VulkanCore::GetDevice() const
{
	// TODO: 在此处插入 return 语句
	return m_Device;
}

const VkCommandPool & VulkanCore::GetCommandPool() const
{
	// TODO: 在此处插入 return 语句
	return m_CommandPool;
}

const VkRenderPass & VulkanCore::GetRenderPass() const
{
	// TODO: 在此处插入 return 语句
	return VK_NULL_HANDLE;
}

const VkFramebuffer & VulkanCore::GetCurrentFrameBuffer() const
{
	// TODO: 在此处插入 return 语句
	return VK_NULL_HANDLE;
}

const VkExtent2D & VulkanCore::GetSwapChainExtent() const
{
	// TODO: 在此处插入 return 语句
	return m_SwapChainExtent2D;
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
	m_UsedPhysicalDevice = index;
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
					m_GraphicsFamily = queueFimilyId;
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
					m_UsedQueueId.insert(index);
					std::cout << "Surface Support At Queue Index " << index << std::endl;
					m_PresentFamily = index;
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
			vkGetDeviceQueue(m_Device,m_GraphicsFamily, 0, &m_GraphicsQueue);
			vkGetDeviceQueue(m_Device,m_PresentFamily, 0, &m_PresentQueue);
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

void VulkanCore::Resize(int w, int h)
{
	if (m_UsedPhysicalDevice == 0xffffffff)
		return;
	
	for (auto& imageView : m_SwapChainImageViews)
	{
		vkDestroyImageView(m_Device, imageView, nullptr);
	}
	m_SwapChainImageViews.clear();
	for (auto& frameBuffer : m_FrameBuffers)
	{
		vkDestroyFramebuffer(m_Device, frameBuffer, nullptr);
	}
	m_FrameBuffers.clear();
	for (auto& depthStencil : m_SwapChainDepthStencil)
	{
		vkDestroyImage(m_Device, depthStencil, nullptr);
	}
	m_SwapChainDepthStencil.clear();
	for (auto& depthStencilView : m_SwapChainDepthStencilView)
	{
		vkDestroyImageView(m_Device, depthStencilView, nullptr);
	}
	m_SwapChainDepthStencilView.clear();
	VkSurfaceCapabilitiesKHR extends;
	vkGetPhysicalDeviceSurfaceCapabilitiesKHR(m_PhysicsDevices[m_UsedPhysicalDevice], m_Surface, &extends);

	NEW_ST(VkSwapchainCreateInfoKHR, createInfo);
	createInfo.clipped = VK_TRUE;
	createInfo.compositeAlpha = VkCompositeAlphaFlagBitsKHR::VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
	createInfo.flags = 0;
	createInfo.imageArrayLayers = 1;
	createInfo.imageColorSpace = VkColorSpaceKHR::VK_COLORSPACE_SRGB_NONLINEAR_KHR;
	createInfo.imageExtent = extends.currentExtent;
	createInfo.imageFormat = VkFormat::VK_FORMAT_B8G8R8A8_UNORM;
	createInfo.imageSharingMode = m_UsedQueueId.size() > 1 ? VkSharingMode::VK_SHARING_MODE_CONCURRENT : VkSharingMode::VK_SHARING_MODE_EXCLUSIVE;
	createInfo.imageUsage = VkImageUsageFlagBits::VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
	createInfo.minImageCount = 2;
	createInfo.oldSwapchain = m_SwapChain;
	createInfo.pNext = 0;
	std::vector<uint32_t> datas;
	for (auto& i : m_UsedQueueId)
	{
		datas.push_back(i);
	}
	createInfo.pQueueFamilyIndices = datas.data();
	createInfo.presentMode = VkPresentModeKHR::VK_PRESENT_MODE_FIFO_KHR;
	createInfo.preTransform = extends.currentTransform;
	createInfo.queueFamilyIndexCount = datas.size();
	createInfo.sType = VkStructureType::VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
	createInfo.surface = m_Surface;

	auto res = vkCreateSwapchainKHR(m_Device, &createInfo, nullptr, &m_SwapChain);
	if (res == VkResult::VK_SUCCESS)
	{
		m_SwapChainImages.clear();
		uint32_t imagesCount = 0;
		vkGetSwapchainImagesKHR(m_Device, m_SwapChain, &imagesCount, nullptr);
		m_SwapChainImages.resize(imagesCount);
		vkGetSwapchainImagesKHR(m_Device, m_SwapChain, &imagesCount, m_SwapChainImages.data());

		m_SwapChainFormat = VkFormat::VK_FORMAT_B8G8R8A8_UNORM;
		m_SwapChainExtent2D = extends.currentExtent;

		m_SwapChainImageViews.resize(m_SwapChainImages.size());
		m_SwapChainDepthStencilView.resize(m_SwapChainImages.size());
		m_SwapChainDepthStencil.resize(m_SwapChainImages.size());
		for (int i = 0; i < m_SwapChainImages.size(); ++i)
		{
			NEW_ST(VkImageViewCreateInfo, imageViewCreateInfo);
			imageViewCreateInfo.components.a = VkComponentSwizzle::VK_COMPONENT_SWIZZLE_A;
			imageViewCreateInfo.components.b = VkComponentSwizzle::VK_COMPONENT_SWIZZLE_B;
			imageViewCreateInfo.components.g = VkComponentSwizzle::VK_COMPONENT_SWIZZLE_G;
			imageViewCreateInfo.components.r = VkComponentSwizzle::VK_COMPONENT_SWIZZLE_R;
			imageViewCreateInfo.flags = 0;
			imageViewCreateInfo.format = m_SwapChainFormat;
			imageViewCreateInfo.image = m_SwapChainImages[i];
			imageViewCreateInfo.pNext = nullptr;
			imageViewCreateInfo.sType = VkStructureType::VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
			imageViewCreateInfo.subresourceRange.aspectMask = VkImageAspectFlagBits::VK_IMAGE_ASPECT_COLOR_BIT;
			imageViewCreateInfo.subresourceRange.baseArrayLayer = 0;
			imageViewCreateInfo.subresourceRange.baseMipLevel = 0;
			imageViewCreateInfo.subresourceRange.layerCount = 1;
			imageViewCreateInfo.subresourceRange.levelCount = 1;
			imageViewCreateInfo.viewType = VkImageViewType::VK_IMAGE_VIEW_TYPE_2D;
			
			auto resV = vkCreateImageView(m_Device, &imageViewCreateInfo, nullptr, &m_SwapChainImageViews[i]);
			if (resV == VkResult::VK_SUCCESS)
			{
				NEW_ST(VkImageCreateInfo, imageInfo);
				imageInfo.arrayLayers = 1;
				imageInfo.extent.height = extends.currentExtent.height;
				imageInfo.extent.width = extends.currentExtent.width;
				imageInfo.extent.depth = 1;
				imageInfo.flags = 0;
				imageInfo.format = VkFormat::VK_FORMAT_D24_UNORM_S8_UINT;
				imageInfo.imageType = VkImageType::VK_IMAGE_TYPE_2D;
				imageInfo.initialLayout = VkImageLayout::VK_IMAGE_LAYOUT_UNDEFINED;
				imageInfo.mipLevels = 1;
				imageInfo.pNext = nullptr;
				imageInfo.pQueueFamilyIndices = &m_GraphicsFamily;
				imageInfo.queueFamilyIndexCount = 1;
				imageInfo.samples = VkSampleCountFlagBits::VK_SAMPLE_COUNT_1_BIT;
				imageInfo.sharingMode = VkSharingMode::VK_SHARING_MODE_EXCLUSIVE;
				imageInfo.sType = VkStructureType::VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
				imageInfo.tiling = VkImageTiling::VK_IMAGE_TILING_OPTIMAL;
				imageInfo.usage = VkImageUsageFlagBits::VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT;
				
				auto resI = vkCreateImage(m_Device, &imageInfo,nullptr, &m_SwapChainDepthStencil[i]);
				if (resI == VkResult::VK_SUCCESS)
					std::cout << "Success Create Image At " << i << std::endl;
				else
				{
					std::cout << "Failed Create Image At " << i << std::endl;
				}


				NEW_ST(VkImageViewCreateInfo, imageViewInfo);
				imageViewInfo.components.a = VkComponentSwizzle::VK_COMPONENT_SWIZZLE_A;
				imageViewInfo.components.b = VkComponentSwizzle::VK_COMPONENT_SWIZZLE_B;
				imageViewInfo.components.g = VkComponentSwizzle::VK_COMPONENT_SWIZZLE_G;
				imageViewInfo.components.r = VkComponentSwizzle::VK_COMPONENT_SWIZZLE_R;
				imageViewInfo.flags = 0;
				imageViewInfo.format = VkFormat::VK_FORMAT_D24_UNORM_S8_UINT;
				imageViewInfo.image = m_SwapChainDepthStencil[i];
				imageViewInfo.pNext = nullptr;
				imageViewInfo.sType = VkStructureType::VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
				imageViewInfo.subresourceRange.aspectMask = VkImageAspectFlagBits::VK_IMAGE_ASPECT_DEPTH_BIT;// | VkImageAspectFlagBits::VK_IMAGE_ASPECT_STENCIL_BIT;
				imageViewInfo.subresourceRange.baseArrayLayer = 0;
				imageViewInfo.subresourceRange.baseMipLevel = 0;
				imageViewInfo.subresourceRange.layerCount = 1;
				imageViewInfo.subresourceRange.levelCount = 1;
				imageViewInfo.viewType = VkImageViewType::VK_IMAGE_VIEW_TYPE_2D;

				//auto resV = vkCreateImageView(m_Device, &imageViewInfo, nullptr, &m_SwapChainDepthStencilView[i]);
				if(resV == VkResult::VK_SUCCESS)
					std::cout << "Success Create Image View At " << i << std::endl;
				else
				{
					std::cout << "Failed Create Image View At " << i << std::endl;
				}
			}

		}
		m_FrameBuffers.resize(m_SwapChainImageViews.size());
		for (int i = 0; i < m_FrameBuffers.size(); ++i)
		{
			std::array<VkImageView, 2> views = {
				m_SwapChainImageViews[i],
				m_SwapChainDepthStencilView[i]
			};
			NEW_ST(VkFramebufferCreateInfo, frameBufferInfo);
			frameBufferInfo.attachmentCount = views.size();
			frameBufferInfo.flags = 0;
			frameBufferInfo.height = extends.currentExtent.height;
			frameBufferInfo.layers = 1;
			frameBufferInfo.pAttachments = views.data();
			frameBufferInfo.pNext = nullptr;
			frameBufferInfo.sType = VkStructureType::VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
			frameBufferInfo.width = extends.currentExtent.width;
			frameBufferInfo.renderPass = m_RenderPass[GeometryPassIndex];
		}

		//vkGetDeviceQueue(m_Device, 0, 0, &m_Queue);
		std::cout << "Success Create SwapChine" << std::endl;
		VkPipelineVertexInputStateCreateInfo a;
		InitSyncSemaphore();
		InitSyncFence();
	}
	else
	{
		std::cout << "Failed Create SwapChine, ErrorCode " << res << std::endl;
	}
}
void VulkanCore::CreateCommandBuffersForSwapChain()
{
	NEW_ST(VkCommandPoolCreateInfo, poolCreateInfo);
	poolCreateInfo.flags = VkCommandPoolCreateFlagBits::VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
	poolCreateInfo.pNext = nullptr;
	poolCreateInfo.queueFamilyIndex = m_GraphicsFamily;
	poolCreateInfo.sType = VkStructureType::VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
	vkCreateCommandPool(m_Device, &poolCreateInfo, nullptr, &m_CommandPool);
	
	
}

void VulkanCore::InitSyncSemaphore()
{
	m_DrawSemaphores.clear();
	m_DrawSemaphores.resize(m_SwapChainImageViews.size());
	for (int i = 0; i < m_DrawSemaphores.size(); ++i)
	{
		NEW_ST(VkSemaphoreCreateInfo, semaCreateInfo);
		semaCreateInfo.pNext = nullptr;
		semaCreateInfo.flags = 0;
		semaCreateInfo.pNext = nullptr;
		semaCreateInfo.sType = VkStructureType::VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;
		vkCreateSemaphore(m_Device, &semaCreateInfo,nullptr, &m_DrawSemaphores[i]);
	}
	m_PresentSemaphores.clear();
	m_PresentSemaphores.resize(m_SwapChainImageViews.size());
	for (int i = 0; i < m_PresentSemaphores.size(); ++i)
	{
		NEW_ST(VkSemaphoreCreateInfo, semaCreateInfo);
		semaCreateInfo.pNext = nullptr;
		semaCreateInfo.flags = 0;
		semaCreateInfo.pNext = nullptr;
		semaCreateInfo.sType = VkStructureType::VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;
		vkCreateSemaphore(m_Device, &semaCreateInfo, nullptr, &m_PresentSemaphores[i]);
	}
}

void VulkanCore::InitSyncFence()
{
	m_Fences.clear();
	m_Fences.resize(m_SwapChainImageViews.size());
	for (int i = 0; i < m_Fences.size(); ++i)
	{
		NEW_ST(VkFenceCreateInfo, fenceCreateInfo);
		fenceCreateInfo.flags = VkFenceCreateFlagBits::VK_FENCE_CREATE_SIGNALED_BIT;
		fenceCreateInfo.pNext = nullptr;
		fenceCreateInfo.sType = VkStructureType::VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
		vkCreateFence(m_Device, &fenceCreateInfo, nullptr, &m_Fences[i]);
	}
}

void VulkanCore::InitShadowPass()
{
	if (m_RenderPass[ShadowPassIndex] != VK_NULL_HANDLE)
	{
		return;
	}
	NEW_ST(VkAttachmentDescription, attachmentDesc);
	attachmentDesc.finalLayout = VkImageLayout::VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
}

void VulkanCore::InitGeometryPass()
{
	NEW_ST(VkAttachmentDescription, attachmentDesc);
	attachmentDesc.finalLayout = VkImageLayout::VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
	attachmentDesc.flags = 0;
	attachmentDesc.format = m_SwapChainFormat;
	attachmentDesc.initialLayout = VkImageLayout::VK_IMAGE_LAYOUT_UNDEFINED;
	attachmentDesc.loadOp = VkAttachmentLoadOp::VK_ATTACHMENT_LOAD_OP_CLEAR;
	attachmentDesc.samples = VkSampleCountFlagBits::VK_SAMPLE_COUNT_1_BIT;
	attachmentDesc.stencilLoadOp = VkAttachmentLoadOp::VK_ATTACHMENT_LOAD_OP_CLEAR;
	attachmentDesc.stencilStoreOp = VkAttachmentStoreOp::VK_ATTACHMENT_STORE_OP_STORE;
	attachmentDesc.storeOp = VkAttachmentStoreOp::VK_ATTACHMENT_STORE_OP_STORE;

	NEW_ST(VkAttachmentDescription, attachmentDepthDesc);
	attachmentDesc.finalLayout = VkImageLayout::VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;
	attachmentDesc.flags = 0;
	attachmentDesc.format = VkFormat::VK_FORMAT_D24_UNORM_S8_UINT;
	attachmentDesc.initialLayout = VkImageLayout::VK_IMAGE_LAYOUT_UNDEFINED;
	attachmentDesc.loadOp = VkAttachmentLoadOp::VK_ATTACHMENT_LOAD_OP_CLEAR;
	attachmentDesc.samples = VkSampleCountFlagBits::VK_SAMPLE_COUNT_1_BIT;
	attachmentDesc.stencilLoadOp = VkAttachmentLoadOp::VK_ATTACHMENT_LOAD_OP_CLEAR;
	attachmentDesc.stencilStoreOp = VkAttachmentStoreOp::VK_ATTACHMENT_STORE_OP_STORE;
	attachmentDesc.storeOp = VkAttachmentStoreOp::VK_ATTACHMENT_STORE_OP_STORE;
	
	NEW_ST(VkAttachmentReference, reference);
	reference.attachment = 0;
	reference.layout = VkImageLayout::VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

	NEW_ST(VkAttachmentReference, depthReference);
	depthReference.attachment = 1;
	depthReference.layout = VkImageLayout::VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

	NEW_ST(VkSubpassDescription, subpassDesc);
	subpassDesc.colorAttachmentCount = 1;
	subpassDesc.flags = 0;
	subpassDesc.inputAttachmentCount = 0;
	subpassDesc.pColorAttachments = &reference;
	subpassDesc.pDepthStencilAttachment = &depthReference;
	subpassDesc.pInputAttachments = nullptr;
	subpassDesc.pipelineBindPoint = VkPipelineBindPoint::VK_PIPELINE_BIND_POINT_GRAPHICS;
	
	std::array<VkAttachmentDescription, 2> attachments = {
		attachmentDesc,
		attachmentDepthDesc
	};
	NEW_ST(VkRenderPassCreateInfo, passCreateInfo);
	passCreateInfo.attachmentCount = attachments.size();
	passCreateInfo.dependencyCount = 0;
	passCreateInfo.flags = 0;
	passCreateInfo.pAttachments = attachments.data();
	passCreateInfo.pDependencies = nullptr;
	passCreateInfo.pNext = nullptr;
	passCreateInfo.pSubpasses = &subpassDesc;
	passCreateInfo.sType = VkStructureType::VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
	passCreateInfo.subpassCount = 1;
	auto res = vkCreateRenderPass(m_Device, &passCreateInfo, nullptr, &m_RenderPass[GeometryPassIndex]);
	if (res == VkResult::VK_SUCCESS)
	{
		std::cout << "Create Geometry Pass Success" << std::endl;
	}
	else
	{
		std::cout << "Create Geometry Pass Failed" << std::endl;
	}
}

void VulkanCore::InitTransparentPass()
{
	NEW_ST(VkAttachmentDescription, attachmentDesc);
	attachmentDesc.finalLayout = VkImageLayout::VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;
	attachmentDesc.flags = 0;
	attachmentDesc.format = m_SwapChainFormat;
	attachmentDesc.initialLayout = VkImageLayout::VK_IMAGE_LAYOUT_UNDEFINED;
	attachmentDesc.loadOp = VkAttachmentLoadOp::VK_ATTACHMENT_LOAD_OP_LOAD;
	attachmentDesc.samples = VkSampleCountFlagBits::VK_SAMPLE_COUNT_1_BIT;
	attachmentDesc.stencilLoadOp = VkAttachmentLoadOp::VK_ATTACHMENT_LOAD_OP_LOAD;
	attachmentDesc.stencilStoreOp = VkAttachmentStoreOp::VK_ATTACHMENT_STORE_OP_STORE;
	attachmentDesc.storeOp = VkAttachmentStoreOp::VK_ATTACHMENT_STORE_OP_STORE;

	NEW_ST(VkAttachmentDescription, attachmentDepthDesc);
	attachmentDesc.finalLayout = VkImageLayout::VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;
	attachmentDesc.flags = 0;
	attachmentDesc.format = VkFormat::VK_FORMAT_D24_UNORM_S8_UINT;
	attachmentDesc.initialLayout = VkImageLayout::VK_IMAGE_LAYOUT_UNDEFINED;
	attachmentDesc.loadOp = VkAttachmentLoadOp::VK_ATTACHMENT_LOAD_OP_LOAD;
	attachmentDesc.samples = VkSampleCountFlagBits::VK_SAMPLE_COUNT_1_BIT;
	attachmentDesc.stencilLoadOp = VkAttachmentLoadOp::VK_ATTACHMENT_LOAD_OP_LOAD;
	attachmentDesc.stencilStoreOp = VkAttachmentStoreOp::VK_ATTACHMENT_STORE_OP_DONT_CARE;
	attachmentDesc.storeOp = VkAttachmentStoreOp::VK_ATTACHMENT_STORE_OP_DONT_CARE;

	NEW_ST(VkAttachmentReference, reference);
	reference.attachment = 0;
	reference.layout = VkImageLayout::VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

	NEW_ST(VkAttachmentReference, depthReference);
	depthReference.attachment = 1;
	depthReference.layout = VkImageLayout::VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

	NEW_ST(VkSubpassDescription, subpassDesc);
	subpassDesc.colorAttachmentCount = 1;
	subpassDesc.flags = 0;
	subpassDesc.inputAttachmentCount = 0;
	subpassDesc.pColorAttachments = &reference;
	subpassDesc.pDepthStencilAttachment = &depthReference;
	subpassDesc.pInputAttachments = nullptr;
	subpassDesc.pipelineBindPoint = VkPipelineBindPoint::VK_PIPELINE_BIND_POINT_GRAPHICS;

	std::array<VkAttachmentDescription, 2> attachments = {
		attachmentDesc,
		attachmentDepthDesc
	};
	NEW_ST(VkRenderPassCreateInfo, passCreateInfo);
	passCreateInfo.attachmentCount = attachments.size();
	passCreateInfo.dependencyCount = 0;
	passCreateInfo.flags = 0;
	passCreateInfo.pAttachments = attachments.data();
	passCreateInfo.pDependencies = nullptr;
	passCreateInfo.pNext = nullptr;
	passCreateInfo.pSubpasses = &subpassDesc;
	passCreateInfo.sType = VkStructureType::VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
	passCreateInfo.subpassCount = 1;
	auto res = vkCreateRenderPass(m_Device, &passCreateInfo, nullptr, &m_RenderPass[GeometryPassIndex]);
	if (res == VkResult::VK_SUCCESS)
	{
		std::cout << "Create Transparent Pass Success" << std::endl;
	}
	else
	{
		std::cout << "Create Transparent Pass Failed" << std::endl;
	}
}

#pragma endregion
