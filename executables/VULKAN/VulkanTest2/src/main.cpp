#include <chaos/chaos.h>

#include "vulkan/vulkan.h"

class VulkanExtensionProperties
{
public:

	VkExtensionProperties properties;
};

class VulkanLayerProperties
{
public:

	VkLayerProperties properties;

	std::vector<VulkanExtensionProperties> extension_properties;
};

class VulkanInstanceInfo
{
public:

	std::vector<VulkanLayerProperties> layer_properties;

	std::vector<VulkanExtensionProperties> extension_properties;
};

// ====================================================

std::vector<VulkanExtensionProperties> GetVulkanExtensionProperties(char const* layer_name)
{
	std::vector<VulkanExtensionProperties> result;

	uint32_t extension_count = 0;
	if (vkEnumerateInstanceExtensionProperties(layer_name, &extension_count, nullptr) != VK_SUCCESS)
	{
		return {};
	}

	if (extension_count > 0)
	{
		std::vector<VkExtensionProperties> extension_properties(extension_count);
		if (vkEnumerateInstanceExtensionProperties(layer_name, &extension_count, &extension_properties[0]) != VK_SUCCESS)
		{
			return {};
		}

		result.reserve(extension_count);
		for (VkExtensionProperties const& properties : extension_properties)
		{
			VulkanExtensionProperties p;
			p.properties = properties;
			result.push_back(std::move(p));
		}
	}
	return result;
}

VulkanInstanceInfo GetVulkanInstanceInfo()
{
	VulkanInstanceInfo result;

	// get instance extension
	result.extension_properties = GetVulkanExtensionProperties(nullptr);

	// get instance layers properties
	uint32_t layer_property_count = 0;
	if (vkEnumerateInstanceLayerProperties(&layer_property_count, nullptr) != VK_SUCCESS)
	{
		return {};
	}
	
	// get layers and their extensions
	if (layer_property_count > 0)
	{
		std::vector<VkLayerProperties> layer_properties(layer_property_count);
		if (vkEnumerateInstanceLayerProperties(&layer_property_count, &layer_properties[0]) != VK_SUCCESS)
		{
			return {};
		}
		result.layer_properties.reserve(layer_property_count);
		for (VkLayerProperties const& properties : layer_properties)
		{
			VulkanLayerProperties p;
			p.properties = properties;
			p.extension_properties = GetVulkanExtensionProperties(properties.layerName);
			result.layer_properties.push_back(std::move(p));
		}
	}
	return result;
}

// ====================================================================================

class VulkanPhysicalDeviceQueueFamilyInfo
{
public:

	VkQueueFamilyProperties properties;

};

class VulkanPhysicalDeviceInfo
{
public:

	VkPhysicalDevice physical_device = nullptr;

	VkPhysicalDeviceProperties properties;

	VkPhysicalDeviceFeatures features;

	VkPhysicalDeviceMemoryProperties memory_properties;

	std::vector<VulkanPhysicalDeviceQueueFamilyInfo> queue_family_properties;
};

std::vector<VulkanPhysicalDeviceQueueFamilyInfo> GetQueueFamiliesInfo(VkPhysicalDevice device)
{
	std::vector<VulkanPhysicalDeviceQueueFamilyInfo> result;

	uint32_t queue_family_count = 0;
	vkGetPhysicalDeviceQueueFamilyProperties(device, &queue_family_count, nullptr);

	if (queue_family_count > 0)
	{
		std::vector<VkQueueFamilyProperties> family_properties(queue_family_count);
		vkGetPhysicalDeviceQueueFamilyProperties(device, &queue_family_count, &family_properties[0]);

		result.reserve(queue_family_count);
		for (VkQueueFamilyProperties const& properties : family_properties)
		{
			VulkanPhysicalDeviceQueueFamilyInfo info;
			info.properties = properties;
			result.push_back(std::move(info));
		}
	}
	return result;
}


std::vector<VulkanPhysicalDeviceInfo> GetPhysicalDeviceInfo(VkInstance instance)
{
	std::vector<VulkanPhysicalDeviceInfo> result;

	uint32_t physical_device_count = 0;
	if (vkEnumeratePhysicalDevices(instance, &physical_device_count, nullptr) != VK_SUCCESS)
	{
		return {};
	}

	std::vector<VkPhysicalDevice> physical_devices(physical_device_count);
	if (physical_device_count > 0)
	{
		if (vkEnumeratePhysicalDevices(instance, &physical_device_count, &physical_devices[0]) != VK_SUCCESS)
		{
			return {};
		}
		for (VkPhysicalDevice device : physical_devices)
		{
			VulkanPhysicalDeviceInfo info;
			info.physical_device = device;
			info.queue_family_properties = GetQueueFamiliesInfo(device);
			vkGetPhysicalDeviceProperties(device, &info.properties);
			vkGetPhysicalDeviceFeatures(device, &info.features);
			vkGetPhysicalDeviceMemoryProperties(device, &info.memory_properties);
			result.push_back(std::move(info));
		}
	}
	return result;
}

// ====================================================================================



// ====================================================================================

int main(int argc, char ** argv, char *)
{
	VulkanInstanceInfo instance_info = GetVulkanInstanceInfo();

	VkApplicationInfo application_info = {};
	application_info.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
	application_info.pApplicationName = "Application";
	application_info.applicationVersion = 1;
	application_info.apiVersion = VK_MAKE_VERSION(1, 0, 0);

	VkInstanceCreateInfo instance_create_info = {};
	instance_create_info.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
	instance_create_info.pApplicationInfo = &application_info;

	VkInstance instance = nullptr;
	if (vkCreateInstance(&instance_create_info, nullptr, &instance) != VK_SUCCESS)
	{
		return -1;
	}


	std::vector<VulkanPhysicalDeviceInfo> physical_devices_info = GetPhysicalDeviceInfo(instance);

	for (VulkanPhysicalDeviceInfo const& device : physical_devices_info)
	{
		if (device.properties.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU)
		{
			for (VulkanPhysicalDeviceQueueFamilyInfo const& queue_family_info : device.queue_family_properties)
			{
				VkQueueFlags wanted_flags = (VK_QUEUE_GRAPHICS_BIT | VK_QUEUE_COMPUTE_BIT | VK_QUEUE_TRANSFER_BIT);
				if ((queue_family_info.properties.queueFlags & wanted_flags) == wanted_flags)
				{

					wanted_flags = wanted_flags;
				}
			}
		}



	}
	



#if 0
	VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU
		VK_PHYSICAL_DEVICE_TYPE_INTEGRATED_GPU = 1,
		VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU = 2,
		VK_PHYSICAL_DEVICE_TYPE_VIRTUAL_GPU = 3,
		VK_PHYSICAL_DEVICE_TYPE_CPU = 4,
#endif

	vkDestroyInstance(instance, nullptr);

	return 0;
}