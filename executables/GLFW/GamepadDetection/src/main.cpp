
#include "chaos/Chaos.h"

class WindowOpenGLTest : public chaos::Window
{
    CHAOS_DECLARE_OBJECT_CLASS(WindowOpenGLTest, chaos::Window);

protected:

  virtual bool OnDraw(chaos::GPURenderContext * render_context, chaos::GPUProgramProviderInterface const * uniform_provider, chaos::WindowDrawParams const& draw_params) override
  {
    glm::vec4 clear_color(0.1f, 0.0f, 0.0f, 0.0f);
    glClearBufferfv(GL_COLOR, 0, (GLfloat*)&clear_color);

    float far_plane = 1000.0f;
    glClearBufferfi(GL_DEPTH_STENCIL, 0, far_plane, 0);

		debug_display.Display(render_context, int(draw_params.viewport.size.x), int(draw_params.viewport.size.y));

    return true;
  }

  virtual void Finalize() override
  {
    debug_display.Finalize();
		chaos::Window::Finalize();
  }

  virtual bool OnInitialize(chaos::JSONReadConfiguration config) override
  {
		if (!chaos::Window::OnInitialize(config))
			return false;

    chaos::Application * application = chaos::Application::GetInstance();
    if (application == nullptr)
      return false;

    boost::filesystem::path resources_path = application->GetResourcesPath();

    boost::filesystem::path image_path     = resources_path / "font.png";

    chaos::GLDebugOnScreenDisplay::Params debug_params;
    debug_params.texture_path               = image_path;
    debug_params.font_characters            = " !\"#$%&'()*+,-./0123456789:;<=>?@ABCDEFGHIJKLMNOPQRSTUVWXYZ[\\]^_`abcdefghijklmnopqrstuvwxyz{|}~";
    debug_params.font_characters_per_line   = 10;
    debug_params.font_characters_line_count = 10;
    debug_params.character_width            = 20;
    debug_params.spacing                    = glm::ivec2( 0, 0);
    debug_params.crop_texture               = glm::ivec2(15, 7);

    if (!debug_display.Initialize(debug_params))
      return false;

    return true;
  }

  virtual bool DoTick(float delta_time) override
  {
    debug_display.Clear();

    for (int i = GLFW_JOYSTICK_1 ; i <= GLFW_JOYSTICK_LAST ; ++i)
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

    debug_display.Tick(delta_time);

    return true; //  refresh
  }

protected:

  chaos::GLDebugOnScreenDisplay debug_display;
};

int main(int argc, char ** argv, char ** env)
{
    return chaos::RunSimpleWindowApplication<WindowOpenGLTest>(argc, argv, env);
}