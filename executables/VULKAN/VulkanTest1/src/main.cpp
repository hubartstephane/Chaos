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

        return true;
    }

    void Finalize()
    {
        // VULKAN destruction
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

        if (glfw_window = glfwCreateWindow(800, 600, "Vulkan", nullptr, nullptr))
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
        create_info.enabledExtensionCount = (uint32_t)extensions.size();
        create_info.ppEnabledExtensionNames = extensions.data();
        create_info.enabledLayerCount = (uint32_t)validation_layers.size();
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
            if (device_properties.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU && DoesPhysicalDeviceSupportExtensions(device, required_device_extensions))
            {
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
                    {
                        vk_physical_device = device;
                        vk_graphic_queue_family_index = graphic_queue.value();
                        vk_present_queue_family_index = present_queue.value();
                        return true;
                    }
                }
            }
        }
        return false;
    }
    
    bool CreateLogicalDevice()
    {
        // create logical device + queues
        std::vector<VkDeviceQueueCreateInfo> queue_create_info{};

        std::vector<uint32_t> queue_indices = { vk_graphic_queue_family_index, vk_present_queue_family_index };
        queue_indices |= ranges::actions::unique;

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

        VkDeviceCreateInfo create_info{};
        create_info.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
        create_info.pQueueCreateInfos = queue_create_info.data();
        create_info.queueCreateInfoCount = uint32_t(queue_create_info.size());
        create_info.pEnabledFeatures = &device_features;
        create_info.enabledLayerCount = uint32_t(validation_layers.size());
        create_info.ppEnabledLayerNames = validation_layers.data();

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
};



int CHAOS_MAIN(int argc, char ** argv, char ** env)
{
    VulkanApplication application;
    application.Run();
    return 0;
}