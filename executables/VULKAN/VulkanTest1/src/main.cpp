#include <chaos/chaos.h>

#include <range/v3/numeric/accumulate.hpp>
#include <range/v3/view/iota.hpp>
#include <range/v3/view/take.hpp>
#include <range/v3/view/join.hpp>
#include <range/v3/view/transform.hpp>

#include <range/v3/algorithm/all_of.hpp>
#include <range/v3/algorithm/any_of.hpp>
#include <range/v3/algorithm/for_each.hpp>
#include <range/v3/algorithm/none_of.hpp>
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
        VkDebugUtilsMessengerCreateInfoEXT createInfo{};
        createInfo.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
        createInfo.messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
        createInfo.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
        createInfo.pfnUserCallback = debugCallback;
        createInfo.pUserData = nullptr; // Optional
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

        if (!CreateInstance())
            return false;
        if (!SelectPysicalDevice())
            return false;
        if (!CreateLogicalDevice())
            return false;
        if (!CreateDebugMessager())
            return false;

        if (!InitializeGLFW())
            return false;
        if (!CreateMainWindow())
            return false;

        return true;
    }

    void Finalize()
    {
        FinalizeGLFW();
        DestroyMainWindow();
        DestroyDebugMessager();
        DestroyLogicalDevice();
        DestroyInstance();
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
            vk_instance = VK_NULL_HANDLE;
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
        uint32_t glfwExtensionCount = 0;
        const char** glfwExtensions = nullptr;

        glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);
        for (uint32_t i = 0; i < glfwExtensionCount; ++i)
        {
            char const* ext = glfwExtensions[i];
            std::cout << "GLFW extension: " << ext << "\n";
        }

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
        create_info.enabledExtensionCount = glfwExtensionCount;
        create_info.ppEnabledExtensionNames = glfwExtensions;
        create_info.enabledLayerCount = (uint32_t)validation_layers.size();
        create_info.ppEnabledLayerNames = validation_layers.data();

        VkDebugUtilsMessengerCreateInfoEXT debug_create_info = GetDebugUtilsMessengerCreateInfo();
        create_info.pNext = &debug_create_info;

        return (vkCreateInstance(&create_info, nullptr, &vk_instance) == VK_SUCCESS);
    }

    bool SelectPysicalDevice()
    {
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
            if (device_properties.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU)
            {
                // check queue families for this device
                uint32_t queue_family_count = 0;
                vkGetPhysicalDeviceQueueFamilyProperties(device, &queue_family_count, nullptr);

                std::vector<VkQueueFamilyProperties> queue_families_properties(queue_family_count);
                vkGetPhysicalDeviceQueueFamilyProperties(device, &queue_family_count, queue_families_properties.data());
                for (VkQueueFamilyProperties const& queue_family_property : queue_families_properties)
                {
                    if (queue_family_property.queueFlags & VK_QUEUE_GRAPHICS_BIT)
                    {
                        vk_physical_device = device;
                        vk_queue_family_index = uint32_t(&queue_family_property - &queue_families_properties[0]);
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
        float priority = 1.0f;

        VkDeviceQueueCreateInfo queue_create_info{};
        queue_create_info.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
        queue_create_info.queueFamilyIndex = vk_queue_family_index;
        queue_create_info.queueCount = 1;
        queue_create_info.pQueuePriorities = &priority;

        VkPhysicalDeviceFeatures device_features{};

        // see VK_KHR_swapchain layer later

        std::vector<const char*> validation_layers = GetValidationLayers();

        VkDeviceCreateInfo create_info{};
        create_info.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
        create_info.pQueueCreateInfos = &queue_create_info;
        create_info.queueCreateInfoCount = 1;
        create_info.pEnabledFeatures = &device_features;
        create_info.enabledLayerCount = uint32_t(validation_layers.size());
        create_info.ppEnabledLayerNames = validation_layers.data();

        if (vkCreateDevice(vk_physical_device, &create_info, nullptr, &vk_device) == VK_SUCCESS)
        {
            // get the queue
            vkGetDeviceQueue(vk_device, vk_queue_family_index, 0, &vk_graphics_queue);
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
        auto func = (PFN_vkCreateDebugUtilsMessengerEXT) vkGetInstanceProcAddr(vk_instance, "vkCreateDebugUtilsMessengerEXT");
        if (func != nullptr)
        {
            VkDebugUtilsMessengerCreateInfoEXT debug_create_info = GetDebugUtilsMessengerCreateInfo();
            func(vk_instance, &debug_create_info, nullptr, &vk_debug_messager);
            return true;
        }
        return false;
    }

    void DestroyDebugMessager()
    {
        if (vk_debug_messager != nullptr)
        {
            auto func = (PFN_vkDestroyDebugUtilsMessengerEXT) vkGetInstanceProcAddr(vk_instance, "vkDestroyDebugUtilsMessengerEXT");
            if (func != nullptr) {
                func(vk_instance, vk_debug_messager, nullptr);
            vk_debug_messager = VK_NULL_HANDLE;
        }
    }

protected:

    VkInstance vk_instance = VK_NULL_HANDLE;

    VkPhysicalDevice vk_physical_device = VK_NULL_HANDLE;

    uint32_t vk_queue_family_index = 0;
                
    VkDevice vk_device = VK_NULL_HANDLE;

    VkQueue vk_graphics_queue = VK_NULL_HANDLE;

    VkDebugUtilsMessengerEXT vk_debug_messager = VK_NULL_HANDLE;

    GLFWwindow* glfw_window = nullptr;
};























int CHAOS_MAIN(int argc, char ** argv, char ** env)
{
    VulkanApplication application;
    application.Run();
    return 0;



     



    glfwInit();
    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
    glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);

    GLFWwindow * window = glfwCreateWindow(800, 600, "Vulkan", nullptr, nullptr);
    if (window != nullptr)
    {
        // extensions GLFW
        uint32_t glfwExtensionCount = 0;
        const char** glfwExtensions = nullptr;

        glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);

        for (uint32_t i = 0; i < glfwExtensionCount; ++i)
        {
            char const* ext = glfwExtensions[i];
            std::cout << "GLFW extension: " << ext << "\n";
        }
            
        // extensions VULKAN
        uint32_t extensionCount = 0;
        vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, nullptr);

        std::vector<VkExtensionProperties> extensions(extensionCount);
        vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, extensions.data());
        for (VkExtensionProperties const& ext : extensions)
            std::cout << "Vulkan extension: " << ext.extensionName << "\n";
        
        // layers
        uint32_t layerCount = 0;
        vkEnumerateInstanceLayerProperties(&layerCount, nullptr);

        std::vector<VkLayerProperties> availableLayers(layerCount);
        vkEnumerateInstanceLayerProperties(&layerCount, availableLayers.data());
        for (VkLayerProperties const& layer : availableLayers)
            std::cout << "Vulkan layer: " << layer.layerName << "\n";

        // create vulkan

#if 0
        VkDebugUtilsMessengerCreateInfoEXT createInfo{};
        createInfo.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
        createInfo.messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
        createInfo.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
        createInfo.pfnUserCallback = debugCallback;
        createInfo.pUserData = nullptr; // Optional
#endif

        VkInstance instance;

        VkApplicationInfo appInfo{};
        appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
        appInfo.pApplicationName = "Hello Triangle";
        appInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
        appInfo.pEngineName = "No Engine";
        appInfo.engineVersion = VK_MAKE_VERSION(1, 0, 0);
        appInfo.apiVersion = VK_API_VERSION_1_0;
#if 0
        createInfo.pNext = (VkDebugUtilsMessengerCreateInfoEXT*) &debugCreateInfo; // create a temporary debugger for the creation only
#endif




        std::vector<const char*> validationLayers = {"VK_LAYER_KHRONOS_validation" /*, "VK_EXT_debug_utils"*/};

        VkInstanceCreateInfo createInfo{};
        createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
        createInfo.pApplicationInfo = &appInfo;
        createInfo.enabledExtensionCount = glfwExtensionCount;
        createInfo.ppEnabledExtensionNames = glfwExtensions;
        createInfo.enabledLayerCount = (uint32_t)validationLayers.size();
        createInfo.ppEnabledLayerNames = validationLayers.data();

        VkResult result = vkCreateInstance(&createInfo, nullptr, &instance);
        if (result == VK_SUCCESS)
        {


 
#if 0
            
            VkDebugUtilsMessengerEXT debug_messager;
            auto func = (PFN_vkCreateDebugUtilsMessengerEXT) vkGetInstanceProcAddr(instance, "vkCreateDebugUtilsMessengerEXT");
            if (func != nullptr)
                func(instance, &createInfo, nullptr, &debug_messager);
#endif

            // Physical device
            VkPhysicalDevice physical_device = VK_NULL_HANDLE;

            uint32_t deviceCount = 0;
            vkEnumeratePhysicalDevices(instance, &deviceCount, nullptr);

            std::vector<VkPhysicalDevice> devices(deviceCount);
            vkEnumeratePhysicalDevices(instance, &deviceCount, devices.data());
            for (VkPhysicalDevice device : devices)
            {
                VkPhysicalDeviceProperties device_properties;
                vkGetPhysicalDeviceProperties(device, &device_properties);

                std::cout << "Device Name:            " << device_properties.deviceName << "\n";
                std::cout << "Max Image Dimension 2D: " << device_properties.limits.maxImageDimension2D << "\n";
                if (device_properties.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU)
                {
                    physical_device = device;
                }
            }

            // queue famillies
            std::optional<uint32_t> queue_family;

            uint32_t queueFamilyCount = 0;
            vkGetPhysicalDeviceQueueFamilyProperties(physical_device, &queueFamilyCount, nullptr);

            std::vector<VkQueueFamilyProperties> queueFamilies(queueFamilyCount);
            vkGetPhysicalDeviceQueueFamilyProperties(physical_device, &queueFamilyCount, queueFamilies.data());
            for (VkQueueFamilyProperties const& qf : queueFamilies)
            {
                if (qf.queueFlags & VK_QUEUE_GRAPHICS_BIT)
                {
                    queue_family = uint32_t(&qf - &queueFamilies[0]);
                    break;
                }
            }

            if (queue_family.has_value())
            {
                // create logical device + queues
                float priority = 1.0f;

                VkDeviceQueueCreateInfo queueCreateInfo{};
                queueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
                queueCreateInfo.queueFamilyIndex = queue_family.value();
                queueCreateInfo.queueCount = 1;
                queueCreateInfo.pQueuePriorities = &priority;

                VkPhysicalDeviceFeatures deviceFeatures{};

                // see VK_KHR_swapchain layer later

                VkDeviceCreateInfo createInfo{};
                createInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
                createInfo.pQueueCreateInfos = &queueCreateInfo;
                createInfo.queueCreateInfoCount = 1;
                createInfo.pEnabledFeatures = &deviceFeatures;
                createInfo.enabledLayerCount = uint32_t(validationLayers.size());
                createInfo.ppEnabledLayerNames = validationLayers.data();

                VkDevice device;
                if (vkCreateDevice(physical_device, &createInfo, nullptr, &device) == VK_SUCCESS)
                {
                    // get the queue
                    VkQueue graphics_queue;
                    vkGetDeviceQueue(device, queue_family.value(), 0, &graphics_queue);


                    // Loop
                    while (!glfwWindowShouldClose(window))
                    {
                        glfwPollEvents();
                    }




                    // destroy logical device
                    vkDestroyDevice(device, nullptr);
                }
            }

                




            // destroy the logger messager
#if 0
            auto func = (PFN_vkDestroyDebugUtilsMessengerEXT) vkGetInstanceProcAddr(instance, "vkDestroyDebugUtilsMessengerEXT");
            if (func != nullptr) {
                func(instance, debug_messager, nullptr);
#endif
   



            vkDestroyInstance(instance, nullptr);
        }
        glfwDestroyWindow(window);
    }
 
    glfwTerminate();



	return 0;
}