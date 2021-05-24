#include <stdio.h>
#include <stdlib.h>
#include <boost/crc.hpp> 
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

class A
{
public:

    A(int i)
        : values(i, i) {};

    auto const & f() const { return values; }

    std::vector<int> values;
};

#include <vector>
using std::cout;


auto is_six = [](int i) { return i == 6; };

int main(int argc, char** argv, char** env)
{
   


    chaos::WinTools::AllocConsoleAndRedirectStdOutput();

    std::vector<int> v{ 6, 2, 3, 4, 5, 6 };

    ranges::actions::sort(v);

    for (auto i : v)
    {
        std::cout << i << "\n";
    }



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


