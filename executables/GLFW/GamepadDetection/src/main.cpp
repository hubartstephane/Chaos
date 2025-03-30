
#include "chaos/Chaos.h"

class WindowOpenGLTest : public chaos::Window
{
	CHAOS_DECLARE_OBJECT_CLASS(WindowOpenGLTest, chaos::Window);

protected:

	virtual bool OnDraw(chaos::GPURenderContext* render_context, chaos::GPUProgramProviderInterface const* uniform_provider, chaos::WindowDrawParams const& draw_params) override
	{
		glm::vec4 clear_color(0.1f, 0.0f, 0.0f, 0.0f);
		glClearBufferfv(GL_COLOR, 0, (GLfloat*)&clear_color);

		float far_plane = 1000.0f;
		glClearBufferfi(GL_DEPTH_STENCIL, 0, far_plane, 0);

		return true;
	}

	virtual bool DoTick(float delta_time) override
	{
		debug_display.Clear();

		for (int i = GLFW_JOYSTICK_1; i <= GLFW_JOYSTICK_LAST; ++i)
		{
			int present = glfwJoystickPresent(i);
			if (present)
			{
				bool input = chaos::GamepadManager::HasAnyInputs(i, 0.2f);

				debug_display.AddLine(chaos::StringTools::Printf("[%02d] : present. Input[%d]", i, input).c_str());
			}
			else
				debug_display.AddLine(chaos::StringTools::Printf("[%02d] : absent", i).c_str());

		}

		return true; //  refresh
	}

	virtual void OnDrawImGuiContent() override
	{
		chaos::Window::OnDrawImGuiContent();

		chaos::ImGuiTools::FullViewportWindow("fullscreen", 0, [this]()
		{
			debug_display.OnDrawImGuiContent(this);
		});
	}

protected:

	chaos::GLDebugOnScreenDisplay debug_display;
};

int main(int argc, char** argv, char** env)
{
	return chaos::RunSimpleWindowApplication<WindowOpenGLTest>(argc, argv, env);
}