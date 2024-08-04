#include <stdlib.h>
#include <stdio.h>
#include <iostream>



//#define GLEW_STATIC
#include <GL/glew.h>
#if _WIN32
ddd
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










void DrawWindow(GLFWwindow * window)
{
	glClearColor(1.0f, 0.0f, 0.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	
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
		
		while (!glfwWindowShouldClose(window))
		{
			DrawWindow(window);
			glfwPollEvents();
		}
		glfwDestroyWindow(window);
		
		
	}
	glfwTerminate();

	return 0;
}
 