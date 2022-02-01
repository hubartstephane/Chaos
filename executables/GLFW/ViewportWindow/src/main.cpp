
#include <chaos/Chaos.h>

uint64_t render_stamp = 0;

chaos::shared_ptr<chaos::GPUFence> render_fence;

class WindowOpenGLTest : public chaos::ViewportWindow
{
	CHAOS_DECLARE_OBJECT_CLASS(WindowOpenGLTest, chaos::ViewportWindow);

protected:

	virtual bool InitializeFromConfiguration(nlohmann::json const& config) override
	{
		if (!chaos::ViewportWindow::InitializeFromConfiguration(config))
			return false;

		if (chaos::ViewportGridLayout* layout = new chaos::ViewportGridLayout)
		{
			layout->SetMaxViewportCount(17, false);
			layout->SetHorizontalFillMode(chaos::ViewportGridHorizontalFillMode::RIGHT_TO_LEFT, false);
			//layout->SetHorizontalFillMode(chaos::ViewportGridHorizontalFillMode::LEFT_TO_RIGHT, false);
			layout->SetVerticalFillMode(chaos::ViewportGridVerticalFillMode::TOP_TO_BOTTOM, false);
			//layout->SetVerticalFillMode(chaos::ViewportGridVerticalFillMode::BOTTOM_TO_TOP, false);
			layout->SetOrientation(chaos::Orientation::VERTICAL);
			SetViewportLayout(layout);

			for (int i = 0 ; i < 8; ++i)
				AddViewport(new chaos::Viewport, false);

			layout->UpdateWindowViewportPlacements();
		}
		return true;
	}


#if 0

	virtual bool OnDraw(chaos::GPURenderer * renderer, chaos::WindowDrawParams const& draw_params, chaos::GPUProgramProviderInterface const * uniform_provider) override
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

	virtual void TweakHints(chaos::WindowHints & hints, GLFWmonitor * monitor, bool pseudo_fullscreen) const override
	{
		chaos::Window::TweakHints(hints, monitor, pseudo_fullscreen);

	//	hints.toplevel = 1;
		hints.decorated = 1;
		hints.resizable = 1;
	}

#endif



};

int CHAOS_MAIN(int argc, char ** argv, char ** env)
{
	chaos::WindowParams params;
	params.monitor = nullptr;
	params.width = 300;
	params.height = 300;
	params.monitor_index = 0;

	chaos::WindowHints hints;

	return chaos::RunWindowApplication<WindowOpenGLTest>(argc, argv, env, params, hints);
}

