#include <stdlib.h>
#include <stdio.h>
#include <iostream>

//#define GLEW_STATIC
#include <GL/glew.h>
#if _WIN32
#include <GL/wglew.h>
#endif

#if __linux__
#include <GL/glxew.h>
#endif

// XXX : GL.h requires windows.h
#include <GL/gl.h>
#include <GL/glu.h>

#if __linux__
#include <GLFW/glfw3.h>
#define GLFW_EXPOSE_NATIVE_X11
#define GLFW_EXPOSE_NATIVE_GLX
#include <GLFW/glfw3native.h>
#endif


#if _WIN32
#include <GLFW/glfw3.h>
#define GLFW_EXPOSE_NATIVE_WIN32
#define GLFW_EXPOSE_NATIVE_WGL
#include <GLFW/glfw3native.h>
#endif

#include "imgui.h"
#include "backends/imgui_impl_opengl3.h"
#include "backends/imgui_impl_glfw.h"

void DrawWindow(GLFWwindow * window)
{
	ImGui_ImplOpenGL3_NewFrame();
	ImGui_ImplGlfw_NewFrame();
	ImGui::NewFrame();
	
	ImGui::Begin("Hello world");
	ImGui::End();	
	
	glClearColor(1.0f, 0.0f, 0.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	
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
		ImGui::CreateContext();
		ImGuiIO& io = ImGui::GetIO(); (void)io;
		io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
		io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls
		ImGui::StyleColorsDark();		
				
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
		ImGui::DestroyContext();
		
		glfwDestroyWindow(window);
	}
	glfwTerminate();

	return 0;
}
 