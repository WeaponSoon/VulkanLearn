#pragma once
#include <vulkan/vulkan.h>
#include <vector>
#include <string>
#include <map>
class VkStringEnum
{
public:
	static std::map<VkQueueFlagBits, std::string> VkQueueFlagBitsString;

public:
	
	static std::string CheckSupportQueueFlag(const VkQueueFamilyProperties& queueFamilyProp);
};