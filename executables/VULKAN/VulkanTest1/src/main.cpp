#include <chaos/chaos.h>



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

uint32_t vertex_shader[] = { 
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

uint32_t frag_shader[] = { 
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

        return true;
    }

    void Finalize()
    {
        // VULKAN destruction
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

    bool InitializeGLFW()
    {
        glfwInit();
        return true;
    }

    void FinalizeGLFW()
    {
        glfwTerminate();
    }

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

    VkPresentModeKHR SelectBestPresentMode(std::vector<VkPresentModeKHR> const& present_modes)
    {
        VkPresentModeKHR searched_modes[] = { VK_PRESENT_MODE_MAILBOX_KHR, VK_PRESENT_MODE_FIFO_KHR };
        for (VkPresentModeKHR mode : searched_modes)
            if (std::ranges::find(present_modes, mode) != std::ranges::end(present_modes))
                return mode;
        return present_modes[0];
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

    uint32_t SelectBestSurfaceImageCount(VkSurfaceCapabilitiesKHR const& surface_caps)
    {
        uint32_t result = surface_caps.minImageCount + 1;
        if (surface_caps.maxImageCount > 0 && result > surface_caps.maxImageCount)
            result = surface_caps.maxImageCount;
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

    void DestroyKHRSurface()
    {
        if (vk_khr_surface != VK_NULL_HANDLE)
        {
            vkDestroySurfaceKHR(vk_instance, vk_khr_surface, nullptr);
            vk_khr_surface = VK_NULL_HANDLE;
        }
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

    bool ExtractSwapChainImage()
    {
        uint32_t image_count = 0;
        vkGetSwapchainImagesKHR(vk_device, vk_swap_chain, &image_count, nullptr);
        vk_swap_chain_images.resize(image_count);
        vkGetSwapchainImagesKHR(vk_device, vk_swap_chain, &image_count, vk_swap_chain_images.data());
        return true;
    }

    void DestroySwapChain()
    {
        if (vk_swap_chain != VK_NULL_HANDLE)
        {
            vkDestroySwapchainKHR(vk_device, vk_swap_chain, nullptr);
            vk_swap_chain = VK_NULL_HANDLE;
        }
    }

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

    void DestroySwapChainImageViews()
    {
        for (VkImageView image_view : vk_swap_chain_image_views)
            vkDestroyImageView(vk_device, image_view, nullptr);
        vk_swap_chain_image_views.clear();
    }

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

    void DestroyRenderPass()
    {
        if (vk_render_pass != VK_NULL_HANDLE)
        {
            vkDestroyRenderPass(vk_device, vk_render_pass, nullptr);
            vk_render_pass = VK_NULL_HANDLE;
        }
    }

    VkShaderModule CreateShaderModule(uint32_t const* data, size_t data_size)
    {
        VkShaderModuleCreateInfo create_info{};
        create_info.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
        create_info.codeSize = data_size;
        create_info.pCode = data;

        VkShaderModule result = VK_NULL_HANDLE;
        if (vkCreateShaderModule(vk_device, &create_info, nullptr, &result) != VK_SUCCESS)
            return result;
        return VK_NULL_HANDLE;
    }

    bool CreatePipeline()
    {
         vk_frag_module = CreateShaderModule(frag_shader, sizeof(frag_shader));
         if (vk_frag_module == VK_NULL_HANDLE)
             return false;

         vk_vert_module = CreateShaderModule(vertex_shader, sizeof(vertex_shader));
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

        VkPipelineDepthStencilStateCreateInfo unused_yet;

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

        return false;
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
};



int CHAOS_MAIN(int argc, char ** argv, char ** env)
{
    VulkanApplication application;
    application.Run();
    return 0;
}