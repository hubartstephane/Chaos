
#include "chaos/Chaos.h"

class MyWindow : public chaos::Window
{
	CHAOS_DECLARE_OBJECT_CLASS(MyWindow, chaos::Window);

protected:

	virtual bool OnDraw(chaos::GPURenderContext * render_context, chaos::GPUProgramProviderInterface const * uniform_provider, chaos::WindowDrawParams const& draw_params) override
	{
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
	chaos::WindowApplicationData window_application_data;
	window_application_data.main_window_class = MyWindow::GetStaticClass();
	return chaos::RunApplication<chaos::WindowApplication>(argc, argv, env, &window_application_data);
}

