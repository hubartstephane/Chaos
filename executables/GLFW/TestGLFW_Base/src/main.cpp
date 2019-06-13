
#include <chaos/StandardHeaders.h> 
#include <chaos/FileTools.h> 
#include <chaos/MyGLFWGamepadManager.h> 
#include <chaos/MyGLFWSingleWindowApplication.h> 
#include <chaos/MyGLFWWindow.h> 
#include <chaos/WinTools.h> 
#include <chaos/MyGLFWTools.h> 
#include <chaos/GPUFence.h> 
#include <chaos/Application.h>


uint64_t render_stamp = 0;

chaos::shared_ptr<chaos::GPUFence> render_fence;

class MyGLFWWindowTest : public chaos::MyGLFW::Window
{

protected:

	virtual bool OnDraw(chaos::Renderer * renderer, glm::ivec2 size) override
	{
		
		uint64_t ts = renderer->GetTimestamp();

		if (render_fence == nullptr)
		{
			render_fence = renderer->GetCurrentFrameFence();
			render_stamp = ts;
		}
		else
		{
			if (render_fence->WaitForCompletion(0.0f))
			{
				uint64_t dt = ts - render_stamp;
				render_fence = nullptr;
				render_stamp = 0;

			}
		}

		glm::vec4 clear_color(0.1f, 0.0f, 0.0f, 0.0f);
		glClearBufferfv(GL_COLOR, 0, (GLfloat*)&clear_color);

		float far_plane = 1000.0f;
		glClearBufferfi(GL_DEPTH_STENCIL, 0, far_plane, 0);
		return true;
	}

	virtual bool OnKeyEvent(int key, int scan_code, int action, int modifier) override
	{
		if (key == GLFW_KEY_F10 && action == GLFW_PRESS)
		{
			ToggleFullscreen();
			return true;
		}
		if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
		{
			RequireWindowClosure();
			return true;
		}
		return chaos::MyGLFW::Window::OnKeyEvent(key, scan_code, action, modifier);
	}

	virtual void TweakHints(chaos::MyGLFW::WindowHints & hints, GLFWmonitor * monitor, bool pseudo_fullscreen) const override
	{
		chaos::MyGLFW::Window::TweakHints(hints, monitor, pseudo_fullscreen);

	//	hints.toplevel = 1;
		hints.decorated = 1;
		hints.resizable = 1;
	}

	virtual void OnIconifiedStateChange(bool iconified)
	{
		std::cout << "OnIconifiedStateChange : " << iconified << std::endl;
	}

	virtual void OnFocusStateChange(bool gain_focus)
	{
		std::cout << "OnFocusStateChange : " << gain_focus << std::endl;
	}
};

int CHAOS_MAIN(int argc, char ** argv, char ** env)
{
	chaos::MyGLFW::SingleWindowApplicationParams params;
	params.monitor = nullptr;
	params.width = 300;
	params.height = 300;
	params.monitor_index = 0;
	chaos::MyGLFW::RunWindowApplication<MyGLFWWindowTest>(argc, argv, env, params);

	return 0;
}

