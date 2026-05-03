#include <stdlib.h>
#include <stdio.h>
#include <iostream>



#define CHAOS_USE_OPENGL 1
#define CHAOS_USE_VULKAN 0

// GLEW
#if CHAOS_USE_OPENGL
#	if _WIN32
#		include <GL/glew.h>
#		include <GL/wglew.h>
#	elif __linux__
#		include <GL/glxew.h>
#	endif
#endif

// OPENGL
#if CHAOS_USE_OPENGL
#	include <GL/gl.h> // XXX : GL.h requires windows.h
#	include <GL/glu.h>
#endif

// VULKAN
#if CHAOS_USE_VULKAN
#	define VK_USE_PLATFORM_WIN32_KHR
#	include <vulkan/vulkan.h>
#	include <vulkan/vulkan_win32.h>
#endif

// GLFW
#if CHAOS_USE_VULKAN
#	define GLFW_INCLUDE_VULKAN
#endif
#include <GLFW/glfw3.h>
#if _WIN32
//#define GLFW_DLL
#	define GLFW_EXPOSE_NATIVE_WIN32
#   define GLFW_EXPOSE_NATIVE_WGL
#elif __linux__
#	if DEATH_USE_WAYLAND
#		define GLFW_EXPOSE_NATIVE_WAYLAND
#	elif DEATH_USE_X11
#		define GLFW_EXPOSE_NATIVE_X11
#	endif
#   define GLFW_EXPOSE_NATIVE_GLX
#endif
#include <GLFW/glfw3native.h>








#include "imgui.h"
#include "backends/imgui_impl_opengl3.h"
#include "backends/imgui_impl_glfw.h"

#include "implot.h"


void DrawImplot()
{
	int plot_style =
		ImPlotFlags_NoMenus |
		ImPlotFlags_NoTitle |
		ImPlotFlags_NoInputs |
		ImPlotFlags_NoBoxSelect;

	if (ImPlot::BeginPlot("function", ImVec2(-1, -1), plot_style))
	{
		constexpr size_t VALUE_COUNT = 256;

		float x[VALUE_COUNT];
		float y[VALUE_COUNT];

		for (size_t i = 0; i < VALUE_COUNT; ++i)
		{
			x[i] = float(i) / float(VALUE_COUNT - 1);
			y[i] = float(i);
		}

		ImPlot::PlotLine("implot test", x, y, VALUE_COUNT);

		ImPlot::EndPlot();
	}
}

void DrawWindow(GLFWwindow * window)
{
	ImGui_ImplOpenGL3_NewFrame();
	ImGui_ImplGlfw_NewFrame();
	ImGui::NewFrame();
		
	glClearColor(1.0f, 0.0f, 0.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	
	DrawImplot();

	ImGui::Render();
	ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
	
	glfwSwapBuffers(window);
}

int main(int argc, char ** argv)
{
	if (!glfwInit())
		return -1;
		
	GLFWwindow* window = glfwCreateWindow(640, 480, "My Title", NULL, NULL);
	if (window != nullptr)
	{
		glfwMakeContextCurrent(window);
		
		GLenum err = glewInit();

		IMGUI_CHECKVERSION();
		ImGuiContext * imgui_context = ImGui::CreateContext();
		ImGuiIO& io = ImGui::GetIO(); (void)io;
		io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
		io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls
		ImGui::StyleColorsDark();		
		ImGui::SetCurrentContext(imgui_context);

		ImPlotContext* implot_context = ImPlot::CreateContext();
		ImPlot::SetCurrentContext(implot_context);
				
		char const * glsl_version = "#version 130";
		
		ImGui_ImplGlfw_InitForOpenGL(window, true);
		ImGui_ImplOpenGL3_Init(glsl_version);
				
		while (!glfwWindowShouldClose(window))
		{
			DrawWindow(window);
			glfwPollEvents();
		}
		
		ImGui_ImplOpenGL3_Shutdown();
		ImGui_ImplGlfw_Shutdown();
		ImPlot::DestroyContext(implot_context);
		ImGui::DestroyContext(imgui_context);
		
		glfwDestroyWindow(window);
	}
	glfwTerminate();

	return 0;
}
 