#include "pch.h"
#include "VkStringEnum.h"

std::map<VkQueueFlagBits, std::string> VkStringEnum::VkQueueFlagBitsString =
{
	{VkQueueFlagBits::VK_QUEUE_GRAPHICS_BIT, "VK_QUEUE_GRAPHICS_BIT"},
	{VkQueueFlagBits::VK_QUEUE_COMPUTE_BIT, "VK_QUEUE_COMPUTE_BIT"},
	{VkQueueFlagBits::VK_QUEUE_TRANSFER_BIT, "VK_QUEUE_TRANSFER_BIT"},
	{VkQueueFlagBits::VK_QUEUE_SPARSE_BINDING_BIT, "VK_QUEUE_SPARSE_BINDING_BIT"},
	{VkQueueFlagBits::VK_QUEUE_PROTECTED_BIT, "VK_QUEUE_PROTECTED_BIT"},
	{VkQueueFlagBits::VK_QUEUE_FLAG_BITS_MAX_ENUM, "VK_QUEUE_FLAG_BITS_MAX_ENUM"}
};

std::string VkStringEnum::CheckSupportQueueFlag(const VkQueueFamilyProperties & queueFamilyProp)
{
	std::string ret;
	for (uint32_t flag = 1; flag <= VkQueueFlagBits::VK_QUEUE_FLAG_BITS_MAX_ENUM; flag <<= 1)
	{
		if (queueFamilyProp.queueFlags&flag)
		{
			if (ret.size() != 0)
			{
				ret += "|";
			}
			ret += VkQueueFlagBitsString[(VkQueueFlagBits)flag];
		}
	}
	return ret;
}
