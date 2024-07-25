#include <stdlib.h>
#include <stdio.h>
#include <iostream>

#include <vulkan/vulkan.h>

int main(int argc, char ** argv)
{
	uint32_t extension_count = 0;
	vkEnumerateInstanceExtensionProperties(nullptr, &extension_count, nullptr);
	
	std::cout << "test vulkan: extension_count = " << extension_count << std::endl;
	return 0;
}
 