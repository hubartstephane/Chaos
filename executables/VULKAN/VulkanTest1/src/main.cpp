


#include <stdio.h>
#include <stdlib.h>
#include <boost/crc.hpp> 


template<typename T>
class B;


template<typename T>
class A
{
public:

    B<T> GetB()
    {
        return B<T>(this);
    }

    void f()
    {
        int i = 0;
        ++i;
    }

};

template<typename T>
class B
{
public:

    B(A<T>* in_a) :a(in_a) {}

    void f()
    {
        a->f();
    }


protected:

    A<T>* a = nullptr;
};


bool LoadFromJSON(int & i)
{


    return false;
}

template<typename T>
bool LoadFromJSON(A<T>& dst)
{


    return true;
}

template<typename T>
bool LoadFromJSON(T & dst)
{


    return true;
}





int main(int argc, char** argv, char** env)
{
    int i = 1;
    LoadFromJSON(i);
    float f = 1.1f;
    LoadFromJSON(f);
    A<int> a;
    LoadFromJSON(a);

    a.GetB().f();












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


