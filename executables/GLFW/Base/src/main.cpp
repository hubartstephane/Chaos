
#include "chaos/Chaos.h"

uint64_t render_stamp = 0;

chaos::shared_ptr<chaos::GPUFence> render_fence;

class WindowOpenGLTest : public chaos::Window
{
	CHAOS_DECLARE_OBJECT_CLASS(WindowOpenGLTest, chaos::Window);

protected:

	virtual bool OnDraw(chaos::GPURenderer * renderer, chaos::GPUProgramProviderInterface const * uniform_provider, chaos::WindowDrawParams const& draw_params) override
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

	virtual void OnIconifiedStateChange(bool iconified)
	{
		std::cout << "OnIconifiedStateChange : " << iconified << std::endl;
	}

	virtual void OnFocusStateChange(bool gain_focus)
	{
		std::cout << "OnFocusStateChange : " << gain_focus << std::endl;
	}
};

int main(int argc, char ** argv, char ** env)
{
	chaos::WindowPlacementInfo placement_info;

	return chaos::RunWindowApplication<WindowOpenGLTest>(argc, argv, env, placement_info);
}

