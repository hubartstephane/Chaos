
#include <chaos/Chaos.h> 

class TestGamepadCallbacks : public chaos::MyGLFW::GamepadCallbacks
{
public:

  TestGamepadCallbacks(chaos::GLDebugOnScreenDisplay * in_debug_display) : debug_display(in_debug_display) {}

  virtual bool AcceptPhysicalDevice(chaos::MyGLFW::PhysicalGamepad * physical_device) override
  {
	  if (!physical_device->IsButtonPressed(chaos::XBoxButton::BUTTON_A))
		  return false;
	  return true;
  }

  virtual bool OnGamepadConnected(chaos::MyGLFW::Gamepad * gamepad) override
  {
    if (debug_display != nullptr)
      debug_display->AddLine("TestGamepadCallbacks::OnGamepadConnected");
    return true;
  }

  virtual bool OnGamepadDisconnected(chaos::MyGLFW::Gamepad * gamepad) override
  {
    if (debug_display != nullptr)
      debug_display->AddLine("TestGamepadCallbacks::OnGamepadDisconnected");
    return true;
  }

  virtual void OnGamepadDataUpdated(class chaos::MyGLFW::GamepadData & gamepad_data) override
  {
  //  if (gamepad_data.IsButtonPressed(chaos::XBoxButton::BUTTON_Y))
    {
   //   if (debug_display != nullptr)
    //    debug_display->AddLine("Y");
    }
  }


protected:

  chaos::GLDebugOnScreenDisplay * debug_display = nullptr;
};




class MyGamepadManager : public chaos::MyGLFW::GamepadManager
{
public:

  MyGamepadManager(chaos::GLDebugOnScreenDisplay * in_debug_display) : debug_display(in_debug_display) {}

protected:

	virtual bool DoPoolGamepad(chaos::MyGLFW::PhysicalGamepad * physical_gamepad) override
	{
		return true;
	}

protected:

	chaos::GLDebugOnScreenDisplay * debug_display = nullptr;
};

class MyGLFWWindowGamepadTest : public chaos::MyGLFW::Window
{

protected:

	virtual bool OnDraw(chaos::GPURenderer * renderer, chaos::box2 const & viewport, glm::ivec2 window_size) override
	{
		glm::vec4 clear_color(0.1f, 0.0f, 0.0f, 0.0f);
		glClearBufferfv(GL_COLOR, 0, (GLfloat*)&clear_color);

		float far_plane = 1000.0f;
		glClearBufferfi(GL_DEPTH_STENCIL, 0, far_plane, 0);

		debug_display.Display((int)(2.0f * viewport.half_size.x), (int)(2.0f * viewport.half_size.y));

		return true;
	}

	virtual void Finalize() override
	{
		main_gamepad = nullptr;
    gamepad_manager = nullptr;
		debug_display.Finalize();
		chaos::MyGLFW::Window::Finalize();
	}

	virtual bool InitializeFromConfiguration(nlohmann::json const & config, boost::filesystem::path const & config_path) override
	{
		if (!chaos::MyGLFW::Window::InitializeFromConfiguration(config, config_path))
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
		debug_params.character_width            = 15;
		debug_params.spacing                    = glm::ivec2( 0, 0);
		debug_params.crop_texture               = glm::ivec2(15, 7);

		if (!debug_display.Initialize(debug_params))
			return false;

		gamepad_manager = new MyGamepadManager(&debug_display);

		main_gamepad = gamepad_manager->AllocateGamepad(false, new TestGamepadCallbacks(&debug_display));
		return true;
	}

	virtual void TweakHints(chaos::MyGLFW::WindowHints & hints, GLFWmonitor * monitor, bool pseudo_fullscreen) const override
	{
		chaos::MyGLFW::Window::TweakHints(hints, monitor, pseudo_fullscreen);

		hints.toplevel  = 1;
		hints.decorated = 1;
	}

	virtual bool Tick(float delta_time) override
	{
		debug_display.Tick(delta_time);
		gamepad_manager->Tick(0.0f);

		if (main_gamepad->IsPresent())
		{

#if 0
			int button_count = main_gamepad->GetButtonCount();
			for (int i = 0; i < button_count; ++i)
			{
				if (main_gamepad->IsButtonPressed(i))
					debug_display.AddLine(chaos::StringTools::Printf("BUTTON [%d] PRESSED", i).c_str(), 1.0f);
			}

			int axis_count = main_gamepad->GetAxisCount();
			for (int i = 0; i < axis_count; ++i)
			{
				float value = main_gamepad->GetAxisValue(i);
				if (value != 0.0f) 
					debug_display.AddLine(chaos::StringTools::Printf("AXIS [%d] = %f", i, value).c_str(), 1.0f);
			}

			if (main_gamepad->IsAnyAxisAction())
				debug_display.AddLine("AXIS ACTION", 1.0f);

#else
			
#define TEST_BUTTON(x) if (main_gamepad->IsButtonPressed(chaos::XBoxButton::x))\
debug_display.AddLine("Pressed : " #x, 1.0f);

			TEST_BUTTON(BUTTON_A);
			TEST_BUTTON(BUTTON_B);
			TEST_BUTTON(BUTTON_X);
			TEST_BUTTON(BUTTON_Y);

			TEST_BUTTON(BUTTON_LEFTBUT);
			TEST_BUTTON(BUTTON_RIGHTBUT);
			TEST_BUTTON(BUTTON_SELECT);
			TEST_BUTTON(BUTTON_START);

			TEST_BUTTON(BUTTON_LEFTSTICK);
			TEST_BUTTON(BUTTON_RIGHTSTICK);

			TEST_BUTTON(BUTTON_UP);
			TEST_BUTTON(BUTTON_DOWN);
			TEST_BUTTON(BUTTON_LEFT);
			TEST_BUTTON(BUTTON_RIGHT);

			glm::vec2 l = main_gamepad->GetXBOXStickDirection(chaos::XBoxAxis::LEFT_AXIS);
			if (l.x != 0.0f || l.y != 0.0f)
				debug_display.AddLine(chaos::StringTools::Printf("LEFT AXIS x : %0.3f   y : %0.3f", l.x, l.y).c_str(), 1.0f);

			glm::vec2 r = main_gamepad->GetXBOXStickDirection(chaos::XBoxAxis::RIGHT_AXIS);
			if (r.x != 0.0f || r.y != 0.0f)
				debug_display.AddLine(chaos::StringTools::Printf("RIGHT AXIS x : %0.3f  y : %0.3f", r.x, r.y).c_str(), 1.0f);

			float left_trigger = main_gamepad->GetAxisValue(chaos::XBoxAxis::LEFT_TRIGGER);
			if (left_trigger)
				debug_display.AddLine(chaos::StringTools::Printf("LEFT TRIGGER  %0.3f", left_trigger).c_str(), 1.0f);

			float right_trigger = main_gamepad->GetAxisValue(chaos::XBoxAxis::RIGHT_TRIGGER);
			if (right_trigger)
				debug_display.AddLine(chaos::StringTools::Printf("RIGHT TRIGGER  %0.3f", right_trigger).c_str(), 1.0f);
#endif
		}

		return true; // no redraw
	}


protected:

	chaos::shared_ptr<MyGamepadManager> gamepad_manager;

	chaos::GLDebugOnScreenDisplay debug_display;

	chaos::shared_ptr<chaos::MyGLFW::Gamepad> main_gamepad;
};

int CHAOS_MAIN(int argc, char ** argv, char ** env)
{
  chaos::MyGLFW::SingleWindowApplicationParams params;
  params.monitor = nullptr;
  params.width = 700;
  params.height = 700;
  params.monitor_index = 0;
  chaos::MyGLFW::RunWindowApplication<MyGLFWWindowGamepadTest>(argc, argv, env, params);

	return 0;
}

