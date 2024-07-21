#define _ITERATOR_DEBUG_LEVELc 0

#include <chaos/chaos.h>

//#include "shaderc/shaderc.hpp"

#include "glslang/public/ShaderLang.h"
#include "spirv_cross/spirv_glsl.hpp"

#include "glslang/SPIRV/GlslangToSpv.h"

#if 0
#include "../SPIRV/GlslangToSpv.h"
#include "../SPIRV/GLSL.std.450.h"
#include "../SPIRV/doc.h"
#include "../SPIRV/disassemble.h"
#include "../glslang/OSDependent/osinclude.h"

// Build-time generated includes
#include "glslang/build_info.h"
#endif


#include <range/v3/numeric/accumulate.hpp>
#include <range/v3/view/iota.hpp>
#include <range/v3/view/take.hpp>
#include <range/v3/view/join.hpp>
#include <range/v3/view/transform.hpp>

#include <range/v3/algorithm/all_of.hpp>
#include <range/v3/algorithm/find_if.hpp>
#include <range/v3/algorithm/any_of.hpp>
#include <range/v3/algorithm/for_each.hpp>
#include <range/v3/algorithm/none_of.hpp>
#include <range/v3/algorithm/unique.hpp>
#include <range/v3/view/all.hpp>
#include <range/v3/view/repeat_n.hpp>
#include <range/v3/view/addressof.hpp>
#include <range/v3/view/adjacent_filter.hpp>
#include <range/v3/view/intersperse.hpp>
#include <range/v3/view/concat.hpp>
#include <range/v3/view/cartesian_product.hpp>
#include <range/v3/view/generate.hpp>
#include <range/v3/view/partial_sum.hpp>
#include <range/v3/view/sample.hpp>

#include <range/v3/action/sort.hpp>
#include <range/v3/action/unique.hpp>


#include <vulkan/vulkan.h>

//F:\Personnel\Programmation\external\VulkanSDK\1.2.162.0\Bin\glslc.exe shader.vert -o vert.spv -mfmt=c
//F:\Personnel\Programmation\external\VulkanSDK\1.2.162.0\Bin\glslc.exe shader.frag -o frag.spv -mfmt=c

std::vector<uint32_t> vertex_shader = {
0x07230203,0x00010000,0x000d000a,0x00000036,
0x00000000,0x00020011,0x00000001,0x0006000b,
0x00000001,0x4c534c47,0x6474732e,0x3035342e,
0x00000000,0x0003000e,0x00000000,0x00000001,
0x0008000f,0x00000000,0x00000004,0x6e69616d,
0x00000000,0x00000022,0x00000026,0x00000031,
0x00030003,0x00000002,0x000001c2,0x00090004,
0x415f4c47,0x735f4252,0x72617065,0x5f657461,
0x64616873,0x6f5f7265,0x63656a62,0x00007374,
0x000a0004,0x475f4c47,0x4c474f4f,0x70635f45,
0x74735f70,0x5f656c79,0x656e696c,0x7269645f,
0x69746365,0x00006576,0x00080004,0x475f4c47,
0x4c474f4f,0x6e695f45,0x64756c63,0x69645f65,
0x74636572,0x00657669,0x00040005,0x00000004,
0x6e69616d,0x00000000,0x00050005,0x0000000c,
0x69736f70,0x6e6f6974,0x00000073,0x00040005,
0x00000017,0x6f6c6f63,0x00007372,0x00060005,
0x00000020,0x505f6c67,0x65567265,0x78657472,
0x00000000,0x00060006,0x00000020,0x00000000,
0x505f6c67,0x7469736f,0x006e6f69,0x00070006,
0x00000020,0x00000001,0x505f6c67,0x746e696f,
0x657a6953,0x00000000,0x00070006,0x00000020,
0x00000002,0x435f6c67,0x4470696c,0x61747369,
0x0065636e,0x00070006,0x00000020,0x00000003,
0x435f6c67,0x446c6c75,0x61747369,0x0065636e,
0x00030005,0x00000022,0x00000000,0x00060005,
0x00000026,0x565f6c67,0x65747265,0x646e4978,
0x00007865,0x00050005,0x00000031,0x67617266,
0x6f6c6f43,0x00000072,0x00050048,0x00000020,
0x00000000,0x0000000b,0x00000000,0x00050048,
0x00000020,0x00000001,0x0000000b,0x00000001,
0x00050048,0x00000020,0x00000002,0x0000000b,
0x00000003,0x00050048,0x00000020,0x00000003,
0x0000000b,0x00000004,0x00030047,0x00000020,
0x00000002,0x00040047,0x00000026,0x0000000b,
0x0000002a,0x00040047,0x00000031,0x0000001e,
0x00000000,0x00020013,0x00000002,0x00030021,
0x00000003,0x00000002,0x00030016,0x00000006,
0x00000020,0x00040017,0x00000007,0x00000006,
0x00000002,0x00040015,0x00000008,0x00000020,
0x00000000,0x0004002b,0x00000008,0x00000009,
0x00000003,0x0004001c,0x0000000a,0x00000007,
0x00000009,0x00040020,0x0000000b,0x00000006,
0x0000000a,0x0004003b,0x0000000b,0x0000000c,
0x00000006,0x0004002b,0x00000006,0x0000000d,
0x00000000,0x0004002b,0x00000006,0x0000000e,
0xbf000000,0x0005002c,0x00000007,0x0000000f,
0x0000000d,0x0000000e,0x0004002b,0x00000006,
0x00000010,0x3f000000,0x0005002c,0x00000007,
0x00000011,0x00000010,0x00000010,0x0005002c,
0x00000007,0x00000012,0x0000000e,0x00000010,
0x0006002c,0x0000000a,0x00000013,0x0000000f,
0x00000011,0x00000012,0x00040017,0x00000014,
0x00000006,0x00000003,0x0004001c,0x00000015,
0x00000014,0x00000009,0x00040020,0x00000016,
0x00000006,0x00000015,0x0004003b,0x00000016,
0x00000017,0x00000006,0x0004002b,0x00000006,
0x00000018,0x3f800000,0x0006002c,0x00000014,
0x00000019,0x00000018,0x0000000d,0x0000000d,
0x0006002c,0x00000014,0x0000001a,0x0000000d,
0x00000018,0x0000000d,0x0006002c,0x00000014,
0x0000001b,0x0000000d,0x0000000d,0x00000018,
0x0006002c,0x00000015,0x0000001c,0x00000019,
0x0000001a,0x0000001b,0x00040017,0x0000001d,
0x00000006,0x00000004,0x0004002b,0x00000008,
0x0000001e,0x00000001,0x0004001c,0x0000001f,
0x00000006,0x0000001e,0x0006001e,0x00000020,
0x0000001d,0x00000006,0x0000001f,0x0000001f,
0x00040020,0x00000021,0x00000003,0x00000020,
0x0004003b,0x00000021,0x00000022,0x00000003,
0x00040015,0x00000023,0x00000020,0x00000001,
0x0004002b,0x00000023,0x00000024,0x00000000,
0x00040020,0x00000025,0x00000001,0x00000023,
0x0004003b,0x00000025,0x00000026,0x00000001,
0x00040020,0x00000028,0x00000006,0x00000007,
0x00040020,0x0000002e,0x00000003,0x0000001d,
0x00040020,0x00000030,0x00000003,0x00000014,
0x0004003b,0x00000030,0x00000031,0x00000003,
0x00040020,0x00000033,0x00000006,0x00000014,
0x00050036,0x00000002,0x00000004,0x00000000,
0x00000003,0x000200f8,0x00000005,0x0003003e,
0x0000000c,0x00000013,0x0003003e,0x00000017,
0x0000001c,0x0004003d,0x00000023,0x00000027,
0x00000026,0x00050041,0x00000028,0x00000029,
0x0000000c,0x00000027,0x0004003d,0x00000007,
0x0000002a,0x00000029,0x00050051,0x00000006,
0x0000002b,0x0000002a,0x00000000,0x00050051,
0x00000006,0x0000002c,0x0000002a,0x00000001,
0x00070050,0x0000001d,0x0000002d,0x0000002b,
0x0000002c,0x0000000d,0x00000018,0x00050041,
0x0000002e,0x0000002f,0x00000022,0x00000024,
0x0003003e,0x0000002f,0x0000002d,0x0004003d,
0x00000023,0x00000032,0x00000026,0x00050041,
0x00000033,0x00000034,0x00000017,0x00000032,
0x0004003d,0x00000014,0x00000035,0x00000034,
0x0003003e,0x00000031,0x00000035,0x000100fd,
0x00010038 };

std::vector<uint32_t> frag_shader = {
0x07230203,0x00010000,0x000d000a,0x00000013,
0x00000000,0x00020011,0x00000001,0x0006000b,
0x00000001,0x4c534c47,0x6474732e,0x3035342e,
0x00000000,0x0003000e,0x00000000,0x00000001,
0x0007000f,0x00000004,0x00000004,0x6e69616d,
0x00000000,0x00000009,0x0000000c,0x00030010,
0x00000004,0x00000007,0x00030003,0x00000002,
0x000001c2,0x00090004,0x415f4c47,0x735f4252,
0x72617065,0x5f657461,0x64616873,0x6f5f7265,
0x63656a62,0x00007374,0x000a0004,0x475f4c47,
0x4c474f4f,0x70635f45,0x74735f70,0x5f656c79,
0x656e696c,0x7269645f,0x69746365,0x00006576,
0x00080004,0x475f4c47,0x4c474f4f,0x6e695f45,
0x64756c63,0x69645f65,0x74636572,0x00657669,
0x00040005,0x00000004,0x6e69616d,0x00000000,
0x00050005,0x00000009,0x4374756f,0x726f6c6f,
0x00000000,0x00050005,0x0000000c,0x67617266,
0x6f6c6f43,0x00000072,0x00040047,0x00000009,
0x0000001e,0x00000000,0x00040047,0x0000000c,
0x0000001e,0x00000000,0x00020013,0x00000002,
0x00030021,0x00000003,0x00000002,0x00030016,
0x00000006,0x00000020,0x00040017,0x00000007,
0x00000006,0x00000004,0x00040020,0x00000008,
0x00000003,0x00000007,0x0004003b,0x00000008,
0x00000009,0x00000003,0x00040017,0x0000000a,
0x00000006,0x00000003,0x00040020,0x0000000b,
0x00000001,0x0000000a,0x0004003b,0x0000000b,
0x0000000c,0x00000001,0x0004002b,0x00000006,
0x0000000e,0x3f800000,0x00050036,0x00000002,
0x00000004,0x00000000,0x00000003,0x000200f8,
0x00000005,0x0004003d,0x0000000a,0x0000000d,
0x0000000c,0x00050051,0x00000006,0x0000000f,
0x0000000d,0x00000000,0x00050051,0x00000006,
0x00000010,0x0000000d,0x00000001,0x00050051,
0x00000006,0x00000011,0x0000000d,0x00000002,
0x00070050,0x00000007,0x00000012,0x0000000f,
0x00000010,0x00000011,0x0000000e,0x0003003e,
0x00000009,0x00000012,0x000100fd,0x00010038 };

static VKAPI_ATTR VkBool32 VKAPI_CALL debugCallback(
    VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
    VkDebugUtilsMessageTypeFlagsEXT messageType,
    const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,
    void* pUserData)
{
    if (messageSeverity >= VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT)
        std::cerr << "validation layer: " << pCallbackData->pMessage << "\n";

    return VK_FALSE;
}

class VulkanApplication
{
public:

    void Run()
    {
        if (Initialize())
        {
            RunMainLoop();
        }
        Finalize();
    }

protected:

    VkDebugUtilsMessengerCreateInfoEXT GetDebugUtilsMessengerCreateInfo()
    {
        VkDebugUtilsMessengerCreateInfoEXT result{};
        result.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
        result.messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
        result.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
        result.pfnUserCallback = debugCallback;
        result.pUserData = nullptr; // Optional
        return result;
    }

    void RunMainLoop()
    {
        while (!glfwWindowShouldClose(glfw_window))
        {
            glfwPollEvents();
            DrawFrame();
        }
    }

    bool Initialize()
    {
        chaos::WinTools::AllocConsoleAndRedirectStdOutput();
        LogLayers();
        LogExtensions();

        // GLFW initialization
        if (!InitializeGLFW())
            return false;
        if (!CreateMainWindow())
            return false;
        // VULKAN initialization
        if (!CreateInstance())
            return false;
        if (!CreateDebugMessager())
            return false;
        if (!CreateKHRSurface())
            return false;
        if (!SelectPysicalDevice())
            return false;
        if (!CreateLogicalDevice())
            return false;
        if (!CreateSwapChain())
            return false;
        if (!ExtractSwapChainImage())
            return false;
        if (!CreateSwapChainImageViews())
            return false;
        if (!CreateRenderPass())
            return false;
        if (!CreatePipeline())
            return false;
        if (!CreateFramebuffers())
            return false;
        if (!CreateCommandPool())
            return false;
        if (!CreateCommandBuffers())
            return false;
        if (!CreateSemaphores())
            return false;

        return true;
    }

    // 1 --------------------------------------------
    bool InitializeGLFW()
    {
        glfwInit();
        return true;
    }
    // 2 --------------------------------------------
    bool CreateMainWindow()
    {
        glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
        glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);
        if (glfw_window = glfwCreateWindow(900, 900, "Vulkan", nullptr, nullptr))
        {
            return true;
        }
        return false;
    }
    // 3 --------------------------------------------
    std::vector<const char*> GetValidationLayers()
    {
        return {"VK_LAYER_KHRONOS_validation" /*, "VK_EXT_debug_utils"*/};
    }

    bool CreateInstance()
    {
        // required extensions for GLFW
        std::vector<char const*> extensions;

        uint32_t glfwExtensionCount = 0;
        const char** glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);
        for (uint32_t i = 0; i < glfwExtensionCount; ++i)
            extensions.push_back(glfwExtensions[i]);

        for (char const * ext : extensions)
            std::cout << "GLFW extension: " << ext << "\n";

        VkApplicationInfo app_info{};
        app_info.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
        app_info.pApplicationName = "Hello Triangle";
        app_info.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
        app_info.pEngineName = "No Engine";
        app_info.engineVersion = VK_MAKE_VERSION(1, 0, 0);
        app_info.apiVersion = VK_API_VERSION_1_0;

        std::vector<const char*> validation_layers = GetValidationLayers();

        VkInstanceCreateInfo create_info{};
        create_info.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
        create_info.pApplicationInfo = &app_info;
        create_info.enabledExtensionCount = uint32_t(extensions.size());
        create_info.ppEnabledExtensionNames = extensions.data();
        create_info.enabledLayerCount = uint32_t(validation_layers.size());
        create_info.ppEnabledLayerNames = validation_layers.data();

#if 0
        VkDebugUtilsMessengerCreateInfoEXT debug_create_info = GetDebugUtilsMessengerCreateInfo();
        create_info.pNext = &debug_create_info;
#endif

        return (vkCreateInstance(&create_info, nullptr, &vk_instance) == VK_SUCCESS);
    }
    // 4 --------------------------------------------
    bool CreateDebugMessager()
    {
#if 0
        auto func = (PFN_vkCreateDebugUtilsMessengerEXT) vkGetInstanceProcAddr(vk_instance, "vkCreateDebugUtilsMessengerEXT");
        if (func != nullptr)
        {
            VkDebugUtilsMessengerCreateInfoEXT debug_create_info = GetDebugUtilsMessengerCreateInfo();
            func(vk_instance, &debug_create_info, nullptr, &vk_debug_messager);
            return true;
        }
        return false;
#endif
        return true;
    }
    // 5 --------------------------------------------
    bool CreateKHRSurface()
    {
#if 0 // platform dependant version
        VkWin32SurfaceCreateInfoKHR createInfo{};
        createInfo.sType = VK_STRUCTURE_TYPE_WIN32_SURFACE_CREATE_INFO_KHR;
        createInfo.hwnd = glfwGetWin32Window(glfw_window);
        createInfo.hinstance = GetModuleHandle(nullptr);
        return (vkCreateWin32SurfaceKHR(vk_instance, &createInfo, nullptr, &vk_khr_surface) == VK_SUCCESS);
#else // generic version
        return (glfwCreateWindowSurface(vk_instance, glfw_window, nullptr, &vk_khr_surface) == VK_SUCCESS);
#endif
    }
    // 6 --------------------------------------------
   std::vector<const char*> GetRequiredDeviceExtensions()
    {
        return {VK_KHR_SWAPCHAIN_EXTENSION_NAME};
    }

    bool DoesPhysicalDeviceSupportExtensions(VkPhysicalDevice device, std::vector<const char*> const& required_device_extensions)
    {
        uint32_t extension_count = 0;
        vkEnumerateDeviceExtensionProperties(device, nullptr, &extension_count, nullptr);

        std::vector<VkExtensionProperties> available_extensions(extension_count);
        vkEnumerateDeviceExtensionProperties(device, nullptr, &extension_count, available_extensions.data());

        for (const auto& searched_ext : required_device_extensions)
        {
            auto it = std::ranges::find_if(available_extensions, [searched_ext](char const* ext) { return chaos::StringTools::Stricmp(searched_ext, ext) == 0; }, &VkExtensionProperties::extensionName);
            if (it == std::ranges::end(available_extensions))
                return false;
        }

        return true;
    }

    VkSurfaceFormatKHR SelectBestSurfaceFormat(std::vector<VkSurfaceFormatKHR> const & formats)
    {
        for (VkSurfaceFormatKHR const& format : formats)
            if (format.format == VK_FORMAT_B8G8R8A8_SRGB && format.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR)
                return format;
        return formats[0];
    }

    VkExtent2D SelectBestSurfaceExtend(VkSurfaceCapabilitiesKHR const & surface_caps)
    {
        if (surface_caps.currentExtent.width != std::numeric_limits<uint32_t>::max())
            return surface_caps.currentExtent;

        int width, height;
        glfwGetFramebufferSize(glfw_window, &width, &height);

        VkExtent2D result = { static_cast<uint32_t>(width), static_cast<uint32_t>(height) };

        result.width = std::max(surface_caps.minImageExtent.width, std::min(surface_caps.maxImageExtent.width, result.width));
        result.height = std::max(surface_caps.minImageExtent.height, std::min(surface_caps.maxImageExtent.height, result.height));
        return result;
    }

    bool SelectPysicalDevice()
    {
        std::vector<char const*> required_device_extensions = GetRequiredDeviceExtensions();

        uint32_t device_count = 0;
        vkEnumeratePhysicalDevices(vk_instance, &device_count, nullptr);

        std::vector<VkPhysicalDevice> devices(device_count);
        vkEnumeratePhysicalDevices(vk_instance, &device_count, devices.data());
        for (VkPhysicalDevice device : devices)
        {
            VkPhysicalDeviceProperties device_properties;
            vkGetPhysicalDeviceProperties(device, &device_properties);

            std::cout << "Device Name:            " << device_properties.deviceName << "\n";
            std::cout << "Max Image Dimension 2D: " << device_properties.limits.maxImageDimension2D << "\n";

            // GPU ?
            if (device_properties.deviceType != VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU)
                continue;
            // support wanted extensions ?
            if (!DoesPhysicalDeviceSupportExtensions(device, required_device_extensions))
                continue;

            std::optional<uint32_t> graphic_queue;
            std::optional<uint32_t> present_queue;

            // check queue families for this device
            uint32_t queue_family_count = 0;
            vkGetPhysicalDeviceQueueFamilyProperties(device, &queue_family_count, nullptr);

            std::vector<VkQueueFamilyProperties> queue_families_properties(queue_family_count);
            vkGetPhysicalDeviceQueueFamilyProperties(device, &queue_family_count, queue_families_properties.data());

            // the device must have a queue family for graphic and a family for present
            for (VkQueueFamilyProperties const& queue_family_property : queue_families_properties) // all queues
            {
                uint32_t index = uint32_t(&queue_family_property - &queue_families_properties[0]);

                if (!graphic_queue.has_value())
                    if (queue_family_property.queueFlags & VK_QUEUE_GRAPHICS_BIT)
                        graphic_queue = index;

                if (!present_queue.has_value())
                {
                    VkBool32 present_support = false;
                    if (vkGetPhysicalDeviceSurfaceSupportKHR(device, index, vk_khr_surface, &present_support) == VK_SUCCESS)
                        if (present_support)
                            present_queue = index;
                }
                if (graphic_queue.has_value() && present_queue.has_value())
                    break;
            }

            if (!graphic_queue.has_value() || !present_queue.has_value())
                continue;

            // swap chain: caps
            VkSurfaceCapabilitiesKHR surface_caps;
            vkGetPhysicalDeviceSurfaceCapabilitiesKHR(device, vk_khr_surface, &surface_caps);

            // swap chain: surfaces
            uint32_t format_count = 0;
            vkGetPhysicalDeviceSurfaceFormatsKHR(device, vk_khr_surface, &format_count, nullptr);

            std::vector<VkSurfaceFormatKHR> surface_formats(format_count);
            vkGetPhysicalDeviceSurfaceFormatsKHR(device, vk_khr_surface, &format_count, surface_formats.data());
            if (surface_formats.empty())
                continue;

            // swap chain: present mode
            uint32_t present_mode_count = 0;
            vkGetPhysicalDeviceSurfacePresentModesKHR(device, vk_khr_surface, &present_mode_count, nullptr);

            std::vector<VkPresentModeKHR> present_modes(present_mode_count);
            vkGetPhysicalDeviceSurfacePresentModesKHR(device, vk_khr_surface, &present_mode_count, present_modes.data());
            if (present_modes.empty())
                continue;

            // get this device
            vk_physical_device = device;
            vk_graphic_queue_family_index = graphic_queue.value();
            vk_present_queue_family_index = present_queue.value();
            vk_surface_caps = surface_caps;
            vk_surface_formats = std::move(surface_formats);
            vk_present_modes = std::move(present_modes);
            return true;
        }
        return false;
    }
    // 7 --------------------------------------------
    bool CreateLogicalDevice()
    {
        // create logical device + queues
        std::vector<VkDeviceQueueCreateInfo> queue_create_info{};

        std::vector<uint32_t> queue_indices = { vk_graphic_queue_family_index, vk_present_queue_family_index };
        queue_indices |= ranges::actions::unique; // cannot request multiple times for the same family index

        for (uint32_t index : queue_indices)
        {
            float priority = 1.0f;

            VkDeviceQueueCreateInfo info{};
            info.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
            info.queueFamilyIndex = index;
            info.queueCount = 1;
            info.pQueuePriorities = &priority;
            queue_create_info.push_back(info);
        }

        VkPhysicalDeviceFeatures device_features{};

        // see VK_KHR_swapchain layer later

        std::vector<const char*> validation_layers = GetValidationLayers();
        std::vector<char const*> required_device_extensions = GetRequiredDeviceExtensions();

        VkDeviceCreateInfo create_info{};
        create_info.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
        create_info.pQueueCreateInfos = queue_create_info.data();
        create_info.queueCreateInfoCount = uint32_t(queue_create_info.size());
        create_info.pEnabledFeatures = &device_features;
        create_info.enabledLayerCount = uint32_t(validation_layers.size());
        create_info.ppEnabledLayerNames = validation_layers.data();
        create_info.enabledExtensionCount = uint32_t(required_device_extensions.size());
        create_info.ppEnabledExtensionNames = required_device_extensions.data();

        if (vkCreateDevice(vk_physical_device, &create_info, nullptr, &vk_device) == VK_SUCCESS)
        {
            // get the queue
            vkGetDeviceQueue(vk_device, vk_graphic_queue_family_index, 0, &vk_graphic_queue);
            vkGetDeviceQueue(vk_device, vk_present_queue_family_index, 0, &vk_present_queue);
            return true;
        }
        return false;
    }
    // 8 --------------------------------------------
    VkPresentModeKHR SelectBestPresentMode(std::vector<VkPresentModeKHR> const& present_modes)
    {
        VkPresentModeKHR searched_modes[] = { VK_PRESENT_MODE_MAILBOX_KHR, VK_PRESENT_MODE_FIFO_KHR };
        for (VkPresentModeKHR mode : searched_modes)
            if (std::ranges::find(present_modes, mode) != std::ranges::end(present_modes))
                return mode;
        return present_modes[0];
    }

    uint32_t SelectBestSurfaceImageCount(VkSurfaceCapabilitiesKHR const& surface_caps)
    {
        uint32_t result = surface_caps.minImageCount + 1;
        if (surface_caps.maxImageCount > 0 && result > surface_caps.maxImageCount)
            result = surface_caps.maxImageCount;
        return result;
    }

    bool CreateSwapChain()
    {
        VkSurfaceFormatKHR surface_format = SelectBestSurfaceFormat(vk_surface_formats);
        VkPresentModeKHR present_mode = SelectBestPresentMode(vk_present_modes);
        VkExtent2D extent = SelectBestSurfaceExtend(vk_surface_caps);
        uint32_t image_count = SelectBestSurfaceImageCount(vk_surface_caps);

        VkSwapchainCreateInfoKHR create_info{};
        create_info.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
        create_info.surface = vk_khr_surface;
        create_info.minImageCount = image_count;
        create_info.imageFormat = surface_format.format;
        create_info.imageColorSpace = surface_format.colorSpace;
        create_info.imageExtent = extent;
        create_info.imageArrayLayers = 1; // for stereoscopic
        create_info.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;

        uint32_t queue_family_indices[] = {vk_graphic_queue_family_index, vk_present_queue_family_index};

        if (vk_graphic_queue_family_index != vk_present_queue_family_index)
        {
            create_info.imageSharingMode = VK_SHARING_MODE_CONCURRENT; // images are shared between queues from different families
            create_info.queueFamilyIndexCount = 2;
            create_info.pQueueFamilyIndices = queue_family_indices;
        }
        else
        {
            create_info.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
            create_info.queueFamilyIndexCount = 0; // Optional
            create_info.pQueueFamilyIndices = nullptr; // Optional
        }

        create_info.preTransform = vk_surface_caps.currentTransform;
        create_info.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
        create_info.presentMode = present_mode;
        create_info.clipped = VK_TRUE; // do you care of pixel hidden by another window ?
        create_info.oldSwapchain = VK_NULL_HANDLE;

        return (vkCreateSwapchainKHR(vk_device, &create_info, nullptr, &vk_swap_chain) == VK_SUCCESS);
    }
    // 9 --------------------------------------------
    bool ExtractSwapChainImage()
    {
        uint32_t image_count = 0;
        vkGetSwapchainImagesKHR(vk_device, vk_swap_chain, &image_count, nullptr);
        vk_swap_chain_images.resize(image_count);
        vkGetSwapchainImagesKHR(vk_device, vk_swap_chain, &image_count, vk_swap_chain_images.data());
        return true;
    }
    // 10 --------------------------------------------
    bool CreateSwapChainImageViews()
    {
         VkSurfaceFormatKHR surface_format = SelectBestSurfaceFormat(vk_surface_formats);

        for (VkImage image : vk_swap_chain_images)
        {
            VkImageViewCreateInfo create_info{};
            create_info.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
            create_info.image = image;
            create_info.viewType = VK_IMAGE_VIEW_TYPE_2D;
            create_info.format = surface_format.format;
            create_info.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
            create_info.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
            create_info.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
            create_info.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;
            create_info.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
            create_info.subresourceRange.baseMipLevel = 0;
            create_info.subresourceRange.levelCount = 1;
            create_info.subresourceRange.baseArrayLayer = 0;
            create_info.subresourceRange.layerCount = 1;

            VkImageView image_view = VK_NULL_HANDLE;
            if (vkCreateImageView(vk_device, &create_info, nullptr, &image_view) != VK_SUCCESS)
                return false;
            vk_swap_chain_image_views.push_back(image_view);
        }
        return true;
    }
    // 11 --------------------------------------------
    bool CreateRenderPass()
    {
        VkAttachmentDescription color_attachment{};
        color_attachment.format = SelectBestSurfaceFormat(vk_surface_formats).format;
        color_attachment.samples = VK_SAMPLE_COUNT_1_BIT;
        color_attachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
        color_attachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
        color_attachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
        color_attachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
        color_attachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
        color_attachment.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;

        VkAttachmentReference color_attachment_ref{};
        color_attachment_ref.attachment = 0; // layout(location = 0) out vec4 outColor
        color_attachment_ref.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

        VkSubpassDescription subpass{};
        subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
        subpass.colorAttachmentCount = 1;
        subpass.pColorAttachments = &color_attachment_ref;

        VkRenderPassCreateInfo render_pass_info{};
        render_pass_info.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
        render_pass_info.attachmentCount = 1;
        render_pass_info.pAttachments = &color_attachment;
        render_pass_info.subpassCount = 1;
        render_pass_info.pSubpasses = &subpass;

        if (vkCreateRenderPass(vk_device, &render_pass_info, nullptr, &vk_render_pass) != VK_SUCCESS)
            return false;

        return true;
    }
    // 12 --------------------------------------------
    VkShaderModule CreateShaderModule(uint32_t const* data, size_t data_size)
    {
        VkShaderModuleCreateInfo create_info{};
        create_info.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
        create_info.codeSize = data_size;
        create_info.pCode = data;

        VkShaderModule result = VK_NULL_HANDLE;
        if (vkCreateShaderModule(vk_device, &create_info, nullptr, &result) != VK_SUCCESS)
            return VK_NULL_HANDLE;
        return result;
    }

    bool CreatePipeline()
    {
         vk_frag_module = CreateShaderModule(frag_shader.data(), frag_shader.size());
         if (vk_frag_module == VK_NULL_HANDLE)
             return false;

         vk_vert_module = CreateShaderModule(vertex_shader.data(), vertex_shader.size());
         if (vk_vert_module == VK_NULL_HANDLE)
             return false;

        VkPipelineShaderStageCreateInfo vertex_stage_info{};
        vertex_stage_info.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
        vertex_stage_info.stage = VK_SHADER_STAGE_VERTEX_BIT;
        vertex_stage_info.module = vk_vert_module;
        vertex_stage_info.pName = "main";

        VkPipelineShaderStageCreateInfo frag_stage_info{};
        frag_stage_info.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
        frag_stage_info.stage = VK_SHADER_STAGE_FRAGMENT_BIT;
        frag_stage_info.module = vk_frag_module;
        frag_stage_info.pName = "main";

        VkPipelineShaderStageCreateInfo shader_stages[] = {frag_stage_info, vertex_stage_info};


        VkPipelineVertexInputStateCreateInfo vertex_input_info{};
        vertex_input_info.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
        vertex_input_info.vertexBindingDescriptionCount = 0;
        vertex_input_info.pVertexBindingDescriptions = nullptr; // Optional
        vertex_input_info.vertexAttributeDescriptionCount = 0;
        vertex_input_info.pVertexAttributeDescriptions = nullptr; // Optional


        VkPipelineInputAssemblyStateCreateInfo input_assembly{};
        input_assembly.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
        input_assembly.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
        input_assembly.primitiveRestartEnable = VK_FALSE;

        VkExtent2D swap_chain_extent = SelectBestSurfaceExtend(vk_surface_caps);

        VkViewport viewport{};
        viewport.x = 0.0f;
        viewport.y = 0.0f;
        viewport.width = (float) swap_chain_extent.width;
        viewport.height = (float) swap_chain_extent.height;
        viewport.minDepth = 0.0f;
        viewport.maxDepth = 1.0f;

        VkRect2D scissor{};
        scissor.offset = {0, 0};
        scissor.extent = swap_chain_extent;

        VkPipelineViewportStateCreateInfo viewport_state{};
        viewport_state.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
        viewport_state.viewportCount = 1;
        viewport_state.pViewports = &viewport;
        viewport_state.scissorCount = 1;
        viewport_state.pScissors = &scissor;

        VkPipelineRasterizationStateCreateInfo rasterizer{};
        rasterizer.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
        rasterizer.depthClampEnable = VK_FALSE;
        rasterizer.rasterizerDiscardEnable = VK_FALSE;
        rasterizer.polygonMode = VK_POLYGON_MODE_FILL;
        rasterizer.lineWidth = 1.0f;
        rasterizer.cullMode = VK_CULL_MODE_BACK_BIT;
        rasterizer.frontFace = VK_FRONT_FACE_CLOCKWISE;
        rasterizer.depthBiasEnable = VK_FALSE;
        rasterizer.depthBiasConstantFactor = 0.0f; // Optional
        rasterizer.depthBiasClamp = 0.0f; // Optional
        rasterizer.depthBiasSlopeFactor = 0.0f; // Optional

        VkPipelineMultisampleStateCreateInfo multisampling{};
        multisampling.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
        multisampling.sampleShadingEnable = VK_FALSE;
        multisampling.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;
        multisampling.minSampleShading = 1.0f; // Optional
        multisampling.pSampleMask = nullptr; // Optional
        multisampling.alphaToCoverageEnable = VK_FALSE; // Optional
        multisampling.alphaToOneEnable = VK_FALSE; // Optional

        //VkPipelineDepthStencilStateCreateInfo unused_yet;

        VkPipelineColorBlendAttachmentState color_blend_attachment{};
        color_blend_attachment.colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
        color_blend_attachment.blendEnable = VK_FALSE;
        color_blend_attachment.srcColorBlendFactor = VK_BLEND_FACTOR_ONE; // Optional
        color_blend_attachment.dstColorBlendFactor = VK_BLEND_FACTOR_ZERO; // Optional
        color_blend_attachment.colorBlendOp = VK_BLEND_OP_ADD; // Optional
        color_blend_attachment.srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE; // Optional
        color_blend_attachment.dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO; // Optional
        color_blend_attachment.alphaBlendOp = VK_BLEND_OP_ADD; // Optional

        VkPipelineColorBlendStateCreateInfo color_blending{};
        color_blending.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
        color_blending.logicOpEnable = VK_FALSE;
        color_blending.logicOp = VK_LOGIC_OP_COPY; // Optional
        color_blending.attachmentCount = 1;
        color_blending.pAttachments = &color_blend_attachment;
        color_blending.blendConstants[0] = 0.0f; // Optional
        color_blending.blendConstants[1] = 0.0f; // Optional
        color_blending.blendConstants[2] = 0.0f; // Optional
        color_blending.blendConstants[3] = 0.0f; // Optional

        VkDynamicState dynamic_states[] = {
            VK_DYNAMIC_STATE_VIEWPORT,
            VK_DYNAMIC_STATE_LINE_WIDTH
        };

        VkPipelineDynamicStateCreateInfo dynamic_state{};
        dynamic_state.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
        dynamic_state.dynamicStateCount = 2;
        dynamic_state.pDynamicStates = dynamic_states;

        VkPipelineLayoutCreateInfo pipeline_layout_info{};
        pipeline_layout_info.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
        pipeline_layout_info.setLayoutCount = 0; // Optional
        pipeline_layout_info.pSetLayouts = nullptr; // Optional
        pipeline_layout_info.pushConstantRangeCount = 0; // Optional
        pipeline_layout_info.pPushConstantRanges = nullptr; // Optional

        if (vkCreatePipelineLayout(vk_device, &pipeline_layout_info, nullptr, &vk_pipeline_layout) != VK_SUCCESS)
            return false;

        VkGraphicsPipelineCreateInfo pipeline_info{};
        pipeline_info.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
        pipeline_info.stageCount = 2;
        pipeline_info.pStages = shader_stages;
        pipeline_info.pVertexInputState = &vertex_input_info;
        pipeline_info.pInputAssemblyState = &input_assembly;
        pipeline_info.pViewportState = &viewport_state;
        pipeline_info.pRasterizationState = &rasterizer;
        pipeline_info.pMultisampleState = &multisampling;
        pipeline_info.pDepthStencilState = nullptr; // Optional
        pipeline_info.pColorBlendState = &color_blending;
        pipeline_info.pDynamicState = nullptr; // Optional
        pipeline_info.layout = vk_pipeline_layout;
        pipeline_info.renderPass = vk_render_pass;
        pipeline_info.subpass = 0;
        pipeline_info.basePipelineHandle = VK_NULL_HANDLE; // Optional
        pipeline_info.basePipelineIndex = -1; // Optional

        if (vkCreateGraphicsPipelines(vk_device, VK_NULL_HANDLE, 1, &pipeline_info, nullptr, &vk_graphics_pipeline) != VK_SUCCESS)
            return false;

        return true;
    }
    // 13 --------------------------------------------
   bool CreateFramebuffers()
    {
        VkExtent2D swap_chain_extent = SelectBestSurfaceExtend(vk_surface_caps);

        vk_swap_chain_framebuffers.resize(vk_swap_chain_image_views.size());

        for (size_t i = 0; i < vk_swap_chain_image_views.size(); i++)
        {
            VkImageView attachments[] = { vk_swap_chain_image_views[i] };

            VkFramebufferCreateInfo framebuffer_info{};
            framebuffer_info.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
            framebuffer_info.renderPass = vk_render_pass;
            framebuffer_info.attachmentCount = 1;
            framebuffer_info.pAttachments = attachments;
            framebuffer_info.width = swap_chain_extent.width;
            framebuffer_info.height = swap_chain_extent.height;
            framebuffer_info.layers = 1;

            if (vkCreateFramebuffer(vk_device, &framebuffer_info, nullptr, &vk_swap_chain_framebuffers[i]) != VK_SUCCESS)
                return false;
        }
        return true;
    }
    // 14 --------------------------------------------
    bool CreateCommandPool()
    {
        VkCommandPoolCreateInfo pool_info{};
        pool_info.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
        pool_info.queueFamilyIndex = vk_graphic_queue_family_index;
        pool_info.flags = 0; // Optional

        if (vkCreateCommandPool(vk_device, &pool_info, nullptr, &vk_command_pool) != VK_SUCCESS)
            return false;

        return true;
    }
    // 15 --------------------------------------------
    bool CreateCommandBuffers()
    {
        vk_command_buffers.resize(vk_swap_chain_image_views.size());

        VkCommandBufferAllocateInfo alloc_info{};
        alloc_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
        alloc_info.commandPool = vk_command_pool;
        alloc_info.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
        alloc_info.commandBufferCount = (uint32_t) vk_command_buffers.size();

        if (vkAllocateCommandBuffers(vk_device, &alloc_info, vk_command_buffers.data()) != VK_SUCCESS) // XXX : allocation X command buffers with a single call
            return false;




        VkExtent2D swap_chain_extent = SelectBestSurfaceExtend(vk_surface_caps);

        for (size_t i = 0; i < vk_command_buffers.size(); i++)
        {
            VkCommandBufferBeginInfo begin_info{};
            begin_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
            begin_info.flags = 0; // Optional
            begin_info.pInheritanceInfo = nullptr; // Optional

            if (vkBeginCommandBuffer(vk_command_buffers[i], &begin_info) != VK_SUCCESS)
                return false;

            VkRenderPassBeginInfo render_pass_info{};
            render_pass_info.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
            render_pass_info.renderPass = vk_render_pass;
            render_pass_info.framebuffer = vk_swap_chain_framebuffers[i];
            render_pass_info.renderArea.offset = {0, 0};
            render_pass_info.renderArea.extent = swap_chain_extent;

            VkClearValue clear_color = {0.0f, 0.0f, 0.0f, 1.0f};
            render_pass_info.clearValueCount = 1;
            render_pass_info.pClearValues = &clear_color;

            vkCmdBeginRenderPass(vk_command_buffers[i], &render_pass_info, VK_SUBPASS_CONTENTS_INLINE); // no return value

            vkCmdBindPipeline(vk_command_buffers[i], VK_PIPELINE_BIND_POINT_GRAPHICS, vk_graphics_pipeline);

            vkCmdDraw(vk_command_buffers[i], 3, 1, 0, 0);

            vkCmdEndRenderPass(vk_command_buffers[i]);

            if (vkEndCommandBuffer(vk_command_buffers[i]) != VK_SUCCESS)
                return false;
        }

        return true;
    }
    // 16 --------------------------------------------
    bool CreateSemaphores()
    {
        VkSemaphoreCreateInfo semaphore_info{};
        semaphore_info.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

        if (vkCreateSemaphore(vk_device, &semaphore_info, nullptr, &vk_image_available_semaphore) != VK_SUCCESS)
            return false;
        if (vkCreateSemaphore(vk_device, &semaphore_info, nullptr, &vk_render_finished_semaphore) != VK_SUCCESS)
            return false;

        return true;
    }

    // ===========================================

    void Finalize()
    {
        // VULKAN destruction
        DestroySemaphores();
        DestroyCommandBuffers();
        DestroyCommandPool();
        DestroyFramebuffers();
        DestroyPipeline();
        DestroyRenderPass();
        DestroySwapChainImageViews();
        DestroySwapChain();
        DestroyLogicalDevice();
        DestroyKHRSurface();
        DestroyDebugMessager();
        DestroyInstance();
        // GLFW destruction
        DestroyMainWindow();
        FinalizeGLFW();
    }

    void DestroyInstance()
    {
        if (vk_instance != VK_NULL_HANDLE)
        {
            vkDestroyInstance(vk_instance, nullptr);
            vk_instance = VK_NULL_HANDLE;
        }
    }

    void DestroyLogicalDevice()
    {
        if (vk_device != VK_NULL_HANDLE)
        {
            vkDestroyDevice(vk_device, nullptr);
            vk_device = VK_NULL_HANDLE;
        }
    }

    void DestroyMainWindow()
    {
        if (glfw_window != nullptr)
        {
            glfwDestroyWindow(glfw_window);
            glfw_window = nullptr;
        }
    }



    void FinalizeGLFW()
    {
        glfwTerminate();
    }










    void LogLayers()
    {
        uint32_t layer_count = 0;
        vkEnumerateInstanceLayerProperties(&layer_count, nullptr);

        std::vector<VkLayerProperties> available_layers(layer_count);
        vkEnumerateInstanceLayerProperties(&layer_count, available_layers.data());
        for (VkLayerProperties const& layer : available_layers)
            std::cout << "Vulkan layer: " << layer.layerName << "\n";
    }

    void LogExtensions()
    {
        uint32_t extension_count = 0;
        vkEnumerateInstanceExtensionProperties(nullptr, &extension_count, nullptr);

        std::vector<VkExtensionProperties> available_extensions(extension_count);
        vkEnumerateInstanceExtensionProperties(nullptr, &extension_count, available_extensions.data());
        for (VkExtensionProperties const& ext : available_extensions)
            std::cout << "Vulkan extension: " << ext.extensionName << "\n";
    }



    void DestroyDebugMessager()
    {
#if 0
        if (vk_debug_messager != nullptr)
        {
            auto func = (PFN_vkDestroyDebugUtilsMessengerEXT) vkGetInstanceProcAddr(vk_instance, "vkDestroyDebugUtilsMessengerEXT");
            if (func != nullptr)
                func(vk_instance, vk_debug_messager, nullptr);
            vk_debug_messager = VK_NULL_HANDLE;
        }
#endif
    }



    void DestroyKHRSurface()
    {
        if (vk_khr_surface != VK_NULL_HANDLE)
        {
            vkDestroySurfaceKHR(vk_instance, vk_khr_surface, nullptr);
            vk_khr_surface = VK_NULL_HANDLE;
        }
    }





    void DestroySwapChain()
    {
        if (vk_swap_chain != VK_NULL_HANDLE)
        {
            vkDestroySwapchainKHR(vk_device, vk_swap_chain, nullptr);
            vk_swap_chain = VK_NULL_HANDLE;
        }
    }



    void DestroySwapChainImageViews()
    {
        for (VkImageView image_view : vk_swap_chain_image_views)
            vkDestroyImageView(vk_device, image_view, nullptr);
        vk_swap_chain_image_views.clear();
    }



    void DestroyRenderPass()
    {
        if (vk_render_pass != VK_NULL_HANDLE)
        {
            vkDestroyRenderPass(vk_device, vk_render_pass, nullptr);
            vk_render_pass = VK_NULL_HANDLE;
        }
    }



    void DestroyPipeline()
    {
        if (vk_frag_module != VK_NULL_HANDLE)
        {
            vkDestroyShaderModule(vk_device, vk_frag_module, nullptr);
            vk_frag_module = VK_NULL_HANDLE;
        }

        if (vk_vert_module != VK_NULL_HANDLE)
        {
            vkDestroyShaderModule(vk_device, vk_vert_module, nullptr);
            vk_vert_module = VK_NULL_HANDLE;
        }

        if (vk_pipeline_layout != VK_NULL_HANDLE)
        {
            vkDestroyPipelineLayout(vk_device, vk_pipeline_layout, nullptr);
            vk_pipeline_layout = VK_NULL_HANDLE;
        }

        if (vk_graphics_pipeline != VK_NULL_HANDLE)
        {
            vkDestroyPipeline(vk_device, vk_graphics_pipeline, nullptr);
            vk_graphics_pipeline = VK_NULL_HANDLE;
        }
    }





    void DestroyFramebuffers()
    {
        for (auto framebuffer : vk_swap_chain_framebuffers)
            vkDestroyFramebuffer(vk_device, framebuffer, nullptr);
        vk_swap_chain_framebuffers.clear();
    }



    void DestroyCommandPool()
    {
        if (vk_command_pool != VK_NULL_HANDLE)
        {
            vkDestroyCommandPool(vk_device, vk_command_pool, nullptr);
            vk_command_pool = VK_NULL_HANDLE;
        }
    }



    void DestroyCommandBuffers()
    {
        vk_command_buffers.clear();
    }



    void DestroySemaphores()
    {
        if (vk_image_available_semaphore != VK_NULL_HANDLE)
        {
            vkDestroySemaphore(vk_device, vk_image_available_semaphore, nullptr);
            vk_image_available_semaphore = VK_NULL_HANDLE;
        }
        if (vk_render_finished_semaphore != VK_NULL_HANDLE)
        {
            vkDestroySemaphore(vk_device, vk_render_finished_semaphore, nullptr);
            vk_render_finished_semaphore = VK_NULL_HANDLE;
        }
    }

    // ===========================================

    void DrawFrame()
    {
        uint32_t image_index = 0;
        vkAcquireNextImageKHR(vk_device, vk_swap_chain, UINT64_MAX, vk_image_available_semaphore, VK_NULL_HANDLE, &image_index);

        VkSubmitInfo submitInfo{};
        submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;

        VkSemaphore waitSemaphores[] = {vk_image_available_semaphore};
        VkPipelineStageFlags waitStages[] = {VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT};
        submitInfo.waitSemaphoreCount = 1;
        submitInfo.pWaitSemaphores = waitSemaphores;
        submitInfo.pWaitDstStageMask = waitStages;
        submitInfo.commandBufferCount = 1;
        submitInfo.pCommandBuffers = &vk_command_buffers[image_index];
    }




protected:

    VkInstance vk_instance = VK_NULL_HANDLE;

    VkPhysicalDevice vk_physical_device = VK_NULL_HANDLE;

    uint32_t vk_graphic_queue_family_index = 0;

    uint32_t vk_present_queue_family_index = 0;

    VkDevice vk_device = VK_NULL_HANDLE;

    VkQueue vk_graphic_queue = VK_NULL_HANDLE;

    VkQueue vk_present_queue = VK_NULL_HANDLE;

    VkDebugUtilsMessengerEXT vk_debug_messager = VK_NULL_HANDLE;

    GLFWwindow* glfw_window = nullptr;

    VkSurfaceKHR vk_khr_surface = VK_NULL_HANDLE;

    VkSurfaceCapabilitiesKHR vk_surface_caps;

    std::vector<VkSurfaceFormatKHR> vk_surface_formats;

    std::vector<VkPresentModeKHR> vk_present_modes;

    VkSwapchainKHR vk_swap_chain = VK_NULL_HANDLE;

    std::vector<VkImage> vk_swap_chain_images;

    std::vector<VkImageView> vk_swap_chain_image_views;

    VkShaderModule vk_frag_module = VK_NULL_HANDLE;

    VkShaderModule vk_vert_module = VK_NULL_HANDLE;

    VkPipelineLayout vk_pipeline_layout = VK_NULL_HANDLE;

    VkRenderPass vk_render_pass = VK_NULL_HANDLE;

    VkPipeline vk_graphics_pipeline = VK_NULL_HANDLE;

    std::vector<VkFramebuffer> vk_swap_chain_framebuffers;

    VkCommandPool vk_command_pool = VK_NULL_HANDLE;

    std::vector<VkCommandBuffer> vk_command_buffers;

    VkSemaphore vk_image_available_semaphore = VK_NULL_HANDLE;

    VkSemaphore vk_render_finished_semaphore = VK_NULL_HANDLE;
};


//====================================================================




static const TBuiltInResource builtin_resources = {
  /* .MaxLights = */ 32,
  /* .MaxClipPlanes = */ 6,
  /* .MaxTextureUnits = */ 32,
  /* .MaxTextureCoords = */ 32,
  /* .MaxVertexAttribs = */ 64,
  /* .MaxVertexUniformComponents = */ 4096,
  /* .MaxVaryingFloats = */ 64,
  /* .MaxVertexTextureImageUnits = */ 32,
  /* .MaxCombinedTextureImageUnits = */ 80,
  /* .MaxTextureImageUnits = */ 32,
  /* .MaxFragmentUniformComponents = */ 4096,
  /* .MaxDrawBuffers = */ 32,
  /* .MaxVertexUniformVectors = */ 128,
  /* .MaxVaryingVectors = */ 8,
  /* .MaxFragmentUniformVectors = */ 16,
  /* .MaxVertexOutputVectors = */ 16,
  /* .MaxFragmentInputVectors = */ 15,
  /* .MinProgramTexelOffset = */ -8,
  /* .MaxProgramTexelOffset = */ 7,
  /* .MaxClipDistances = */ 8,
  /* .MaxComputeWorkGroupCountX = */ 65535,
  /* .MaxComputeWorkGroupCountY = */ 65535,
  /* .MaxComputeWorkGroupCountZ = */ 65535,
  /* .MaxComputeWorkGroupSizeX = */ 1024,
  /* .MaxComputeWorkGroupSizeY = */ 1024,
  /* .MaxComputeWorkGroupSizeZ = */ 64,
  /* .MaxComputeUniformComponents = */ 1024,
  /* .MaxComputeTextureImageUnits = */ 16,
  /* .MaxComputeImageUniforms = */ 8,
  /* .MaxComputeAtomicCounters = */ 8,
  /* .MaxComputeAtomicCounterBuffers = */ 1,
  /* .MaxVaryingComponents = */ 60,
  /* .MaxVertexOutputComponents = */ 64,
  /* .MaxGeometryInputComponents = */ 64,
  /* .MaxGeometryOutputComponents = */ 128,
  /* .MaxFragmentInputComponents = */ 128,
  /* .MaxImageUnits = */ 8,
  /* .MaxCombinedImageUnitsAndFragmentOutputs = */ 8,
  /* .MaxCombinedShaderOutputResources = */ 8,
  /* .MaxImageSamples = */ 0,
  /* .MaxVertexImageUniforms = */ 0,
  /* .MaxTessControlImageUniforms = */ 0,
  /* .MaxTessEvaluationImageUniforms = */ 0,
  /* .MaxGeometryImageUniforms = */ 0,
  /* .MaxFragmentImageUniforms = */ 8,
  /* .MaxCombinedImageUniforms = */ 8,
  /* .MaxGeometryTextureImageUnits = */ 16,
  /* .MaxGeometryOutputVertices = */ 256,
  /* .MaxGeometryTotalOutputComponents = */ 1024,
  /* .MaxGeometryUniformComponents = */ 1024,
  /* .MaxGeometryVaryingComponents = */ 64,
  /* .MaxTessControlInputComponents = */ 128,
  /* .MaxTessControlOutputComponents = */ 128,
  /* .MaxTessControlTextureImageUnits = */ 16,
  /* .MaxTessControlUniformComponents = */ 1024,
  /* .MaxTessControlTotalOutputComponents = */ 4096,
  /* .MaxTessEvaluationInputComponents = */ 128,
  /* .MaxTessEvaluationOutputComponents = */ 128,
  /* .MaxTessEvaluationTextureImageUnits = */ 16,
  /* .MaxTessEvaluationUniformComponents = */ 1024,
  /* .MaxTessPatchComponents = */ 120,
  /* .MaxPatchVertices = */ 32,
  /* .MaxTessGenLevel = */ 64,
  /* .MaxViewports = */ 16,
  /* .MaxVertexAtomicCounters = */ 0,
  /* .MaxTessControlAtomicCounters = */ 0,
  /* .MaxTessEvaluationAtomicCounters = */ 0,
  /* .MaxGeometryAtomicCounters = */ 0,
  /* .MaxFragmentAtomicCounters = */ 8,
  /* .MaxCombinedAtomicCounters = */ 8,
  /* .MaxAtomicCounterBindings = */ 1,
  /* .MaxVertexAtomicCounterBuffers = */ 0,
  /* .MaxTessControlAtomicCounterBuffers = */ 0,
  /* .MaxTessEvaluationAtomicCounterBuffers = */ 0,
  /* .MaxGeometryAtomicCounterBuffers = */ 0,
  /* .MaxFragmentAtomicCounterBuffers = */ 1,
  /* .MaxCombinedAtomicCounterBuffers = */ 1,
  /* .MaxAtomicCounterBufferSize = */ 16384,
  /* .MaxTransformFeedbackBuffers = */ 4,
  /* .MaxTransformFeedbackInterleavedComponents = */ 64,
  /* .MaxCullDistances = */ 8,
  /* .MaxCombinedClipAndCullDistances = */ 8,
  /* .MaxSamples = */ 4,
  /* .maxMeshOutputVerticesNV = */ 256,
  /* .maxMeshOutputPrimitivesNV = */ 512,
  /* .maxMeshWorkGroupSizeX_NV = */ 32,
  /* .maxMeshWorkGroupSizeY_NV = */ 1,
  /* .maxMeshWorkGroupSizeZ_NV = */ 1,
  /* .maxTaskWorkGroupSizeX_NV = */ 32,
  /* .maxTaskWorkGroupSizeY_NV = */ 1,
  /* .maxTaskWorkGroupSizeZ_NV = */ 1,
  /* .maxMeshViewCountNV = */ 4,

  ///* .limits = */ {
    /* .nonInductiveForLoops = */ 1,
    /* .whileLoops = */ 1,
    /* .doWhileLoops = */ 1,
    /* .generalUniformIndexing = */ 1,
    /* .generalAttributeMatrixVectorIndexing = */ 1,
    /* .generalVaryingIndexing = */ 1,
    /* .generalSamplerIndexing = */ 1,
    /* .generalVariableIndexing = */ 1,
    /* .generalConstantMatrixVectorIndexing = */ 1,
    //},
};





char const* pixel_sourceXXX = R"FRAGMENT_SHADER(
    #version 450
    #extension GL_ARB_separate_shader_objects : require
    in vec3 vs_texcoord;
    in vec3 vs_color;



    out vec4 output_color;

    uniform sampler2DArray material;

    void main()
    {
	vec4 color = (vs_texcoord.z < 0.0)?
		vec4(1.0, 1.0, 1.0, 1.0) :
		texture(material, vs_texcoord);
      output_color.xyz = color.xyz * vs_color;
      output_color.a   = color.a;
    };
	)FRAGMENT_SHADER";



char const* pixel_source = R"FRAGMENT_SHADER(
    #version 450
    #extension GL_ARB_separate_shader_objects : require


    uniform vec3  zzz;
    uniform vec3  bbb;

    //layout(location = 0)



    out vec4 output_color;
    void main()
    {
      output_color = vec4(zzz.x, bbb.y, 1.0, 1.0);
    }
	)FRAGMENT_SHADER";


char const* vertex_source = R"VERTEX_SHADER(
    #version 450
    #extension GL_ARB_separate_shader_objects : require
    in vec2 position;
    in vec3 texcoord;
    in vec3 color;

    uniform mat4 local_to_cam;
    uniform float xxx;
    uniform float yyy[7];
    uniform vec3  zzz;

    out vec3 vs_texcoord;
    out vec3 vs_color;

    void main()
    {
      vs_texcoord = texcoord;
      vs_color    = color;
      gl_Position = local_to_cam * vec4(position.x + zzz.x, position.y, 0.0, 1.0);
    }
	)VERTEX_SHADER";











void f(int i)
{
    i = i;

}
void f(int i, int j)
{
    i = i;

}

void f(int i, int j, int k)
{
    j = j;

}




#define MAC(...) f(1, ##__VA_ARGS__)


int main(int argc, char ** argv, char ** env)
{
    MAC();
    MAC(2);
    MAC(2, 3);

    return 0;


 // VulkanApplication application;
 // application.Run();
 // return 0;



   // auto ppp = glslang::DefaultTBuiltInResource;

    glslang::InitializeProcess();

     const EShMessages message_flags = EShMessages(EShMessages::EShMsgSpvRules | EShMessages::EShMsgVulkanRules);


    glslang::TShader ps(EShLangFragment);
    char const* source2[] = { pixel_source };
    ps.setStrings(source2, 1);
    ps.setAutoMapLocations(true);
    ps.setAutoMapBindings(true);
    ps.setEnvInput(glslang::EShSource::EShSourceGlsl, EShLanguage::EShLangFragment, glslang::EShClient::EShClientVulkan, 450);
    ps.setEnvClient(glslang::EShClient::EShClientVulkan, glslang::EShTargetClientVersion::EShTargetVulkan_1_0);
    ps.setEnvTarget(glslang::EShTargetLanguage::EShTargetSpv, glslang::EShTargetLanguageVersion::EShTargetSpv_1_0);
    //ps.parse(&builtin_resources, 450, EProfile::ENoProfile, true, true, message_flags);


    bool b22 = ps.parse(&builtin_resources, 450, false, message_flags);

    auto xxx2 = ps.getInfoLog();

    auto yyy2 = ps.getInfoDebugLog();

    std::vector<unsigned int> spirv;
    spv::SpvBuildLogger logger;
    glslang::GlslangToSpv(*ps.getIntermediate(), spirv, &logger);

    auto p = logger.getAllMessages();

    //---------------------------------------------

    glslang::TShader vs(EShLangVertex);
    char const* sources1[] = { vertex_source };
    vs.setStrings(sources1, 1);
    vs.setEnvInput(glslang::EShSource::EShSourceGlsl, EShLanguage::EShLangVertex, glslang::EShClient::EShClientVulkan, 450);
    vs.setEnvClient(glslang::EShClient::EShClientVulkan, glslang::EShTargetClientVersion::EShTargetVulkan_1_0);
    vs.setEnvTarget(glslang::EShTargetLanguage::EShTargetSpv, glslang::EShTargetLanguageVersion::EShTargetSpv_1_0);

    vs.setAutoMapLocations(true);
    vs.setAutoMapBindings(true);

    bool bb3 = vs.parse(&builtin_resources, 450, EProfile::ENoProfile, true, true, message_flags);
    auto xxx1 = vs.getInfoLog();
    auto yyy1 = vs.getInfoDebugLog();

    std::vector<unsigned int> spirv2;
    spv::SpvBuildLogger logger2;
    glslang::GlslangToSpv(*vs.getIntermediate(), spirv2, &logger2);

    auto p2 = logger2.getAllMessages();

    //---------------------------------------------

    glslang::TProgram prog;
    prog.addShader(&vs);
    prog.addShader(&ps);
    bool b1 = prog.link(EShMsgDefault);

    bool b3 = prog.buildReflection(EShReflectionAllBlockVariables | EShReflectionIntermediateIO /*| EShReflectionAllIOVariables*/);
    bool b2 = prog.mapIO();
    prog.dumpReflection();

    int c1 = prog.getNumUniformBlocks();
    int c2 = prog.getNumLiveUniformBlocks();
    int c3 = prog.getNumUniformVariables();
    int c4 = prog.getNumLiveUniformVariables();

    int c5 = prog.getNumPipeInputs();
    int c6 = prog.getNumPipeOutputs();
    int ppp = prog.getUniformIndex("local_to_cam");
    int pppx = prog.getUniformBinding(ppp);

   // prog.

    for (int i = 0; i < c5; ++i) //getNumPipeInputs
    {
        glslang::TObjectReflection const& r = prog.getPipeInput(i);

        int b = r.getBinding();

        i = i;

    }

    for (int i = 0; i < c6; ++i) //getNumPipeOutputs
    {
        glslang::TObjectReflection const& r = prog.getPipeOutput(i);

        int b = r.getBinding();

        i = i;

    }






    for (int i = 0; i < c3; ++i) // getNumUniformVariables
    {
        glslang::TObjectReflection const& r = prog.getUniform(i);
        int binding = r.getBinding();
        const glslang::TType const * type = r.getType();

        char const * n = type->getBasicString();

        glslang::TBasicType basic_type = type->getBasicType(); // component type

        int v = type->getVectorSize();

        // offset: est OK
        // size: indique si c'est un tableau[7]
        // type.basicType: float/int...
        // type.vectorSize: si float2/float3  si c'est une matrice c'est 0
        // type.matrixCols/Rows (si pas matrice c'est 0)
        //
        // stages : c'est un mask qui indique dans quel shader est utilisé l uniform

        n = n;


    }






#if 1
   // ShInitialize();

    char const* a = glslang::GetEsslVersionString();
    char const* b = glslang::GetGlslVersionString();

   // ShFinalize();
#endif

    glslang::FinalizeProcess();


    return 0;
}