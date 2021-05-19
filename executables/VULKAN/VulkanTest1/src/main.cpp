#include <stdio.h>
#include <stdlib.h>
#include <boost/crc.hpp> 
#include <chaos/chaos.h>

#include <ranges>

class A {};

void f(A& a = A()) {}


int main(int argc, char** argv, char** env)
{
    f();

    const std::vector<std::vector<int>> v{ {1,2}, {3,4,5}, {6}, {7,8,9} };
    auto jv = std::ranges::join_view(v);

    //for (int i : std::views::iota(1, 10))
      //  i = i;


    std::vector v = { 1, 2, 3, 4 , 5, 6, 10 };

    //auto x = v | std::views::transform([](int i) { return i * i; }) | std::views::filter([](int i) {return i > 0; });


    //auto x = v | std::views::drop_while([](int i) { return (i < 7); });

    auto x = v | std::views::transform([](int i) { return float(i); });


    for (auto i : x)
        i = i;

    return 0;
}

#if 0
#include <chaos/chaos.h>

#include <vulkan/vulkan.h>


int CHAOS_MAIN(int argc, char ** argv, char ** env)
{



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
            std::cout << ext << "\n";
        }
            
        // extensions VULKAN
        uint32_t extensionCount = 0;
        vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, nullptr);

        std::vector<VkExtensionProperties> extensions;
        extensions.insert(extensions.begin(), extensionCount, {});

        vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, extensions.data());

        // create vulkan
        VkInstance instance;

        VkApplicationInfo appInfo{};
        appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
        appInfo.pApplicationName = "Hello Triangle";
        appInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
        appInfo.pEngineName = "No Engine";
        appInfo.engineVersion = VK_MAKE_VERSION(1, 0, 0);
        appInfo.apiVersion = VK_API_VERSION_1_0;


        VkInstanceCreateInfo createInfo{};
        createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
        createInfo.pApplicationInfo = &appInfo;
        createInfo.enabledExtensionCount = glfwExtensionCount;
        createInfo.ppEnabledExtensionNames = glfwExtensions;

        VkResult result = vkCreateInstance(&createInfo, nullptr, &instance);
        if (result == VK_SUCCESS)
        {



            // Loop
            while (!glfwWindowShouldClose(window))
            {
                glfwPollEvents();
            }





            vkDestroyInstance(instance, nullptr);
        }
        glfwDestroyWindow(window);
    }
 
    glfwTerminate();



	return 0;
}

#endif


