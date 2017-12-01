
#include <chaos/StandardHeaders.h> 
#include <chaos/FileTools.h> 
#include <chaos/MyGLFWGamepadManager.h> 
#include <chaos/MyGLFWWindow.h> 
#include <chaos/WinTools.h> 
#include <chaos/Application.h>
#include <chaos/GLDebugOnScreenDisplay.h>
#include <chaos/StringTools.h>

class TestGamepadCallbacks : public chaos::MyGLFW::GamepadCallbacks
{
public:

  TestGamepadCallbacks(chaos::GLDebugOnScreenDisplay * in_debug_display) : debug_display(in_debug_display) {}

  virtual bool AcceptPhysicalDevice(chaos::MyGLFW::PhysicalGamepad * physical_device) override
  {
	  if (!physical_device->IsButtonPressed(chaos::MyGLFW::XBOX_BUTTON_A))
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
    if (gamepad_data.IsButtonPressed(chaos::MyGLFW::XBOX_BUTTON_Y))
    {
      if (debug_display != nullptr)
        debug_display->AddLine("Y");
    }
  }


protected:

  chaos::GLDebugOnScreenDisplay * debug_display{ nullptr };
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

	chaos::GLDebugOnScreenDisplay * debug_display{ nullptr };
};

class MyGLFWWindowGamepadTest : public chaos::MyGLFW::Window
{

protected:

	virtual bool OnDraw(glm::ivec2 size) override
	{
		glm::vec4 clear_color(0.1f, 0.0f, 0.0f, 0.0f);
		glClearBufferfv(GL_COLOR, 0, (GLfloat*)&clear_color);

		float far_plane = 1000.0f;
		glClearBufferfi(GL_DEPTH_STENCIL, 0, far_plane, 0);

		debug_display.Display(size.x, size.y);    

		return true;
	}

	virtual void Finalize() override
	{
		main_gamepad = nullptr;
    gamepad_manager = nullptr;
		debug_display.Finalize();
	}

	virtual bool Initialize() override
	{
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

	virtual void TweakSingleWindowApplicationHints(chaos::MyGLFW::WindowHints & hints, GLFWmonitor * monitor, bool pseudo_fullscreen) const override
	{
		chaos::MyGLFW::Window::TweakSingleWindowApplicationHints(hints, monitor, pseudo_fullscreen);

		hints.toplevel  = 1;
		hints.decorated = 1;
	}

	virtual bool Tick(double delta_time) override
	{
		debug_display.Tick(delta_time);
		gamepad_manager->Tick(0.0f);

		if (main_gamepad->IsPresent())
		{

			int change = main_gamepad->GetButtonChanges(chaos::MyGLFW::XBOX_BUTTON_B);

			//if (change == chaos::MyGLFW::BUTTON_STAY_RELEASED)
			//	debug_display.AddLine("BUTTON_STAY_RELEASED", 1.0f);
			//if (change == chaos::MyGLFW::BUTTON_STAY_PRESSED)
			//	debug_display.AddLine("BUTTON_STAY_PRESSED", 1.0f);
			if (change == chaos::MyGLFW::BUTTON_BECOME_RELEASED)
				debug_display.AddLine("BUTTON_BECOME_RELEASED", 1.0f);
			if (change == chaos::MyGLFW::BUTTON_BECOME_PRESSED)
				debug_display.AddLine("BUTTON_BECOME_PRESSED", 1.0f);


			if (main_gamepad->IsAnyAction())
				debug_display.AddLine("IsAnyAction()", 1.0f);

			glm::vec2 l = main_gamepad->GetXBOXStickDirection(chaos::MyGLFW::XBOX_LEFT_AXIS);
			if (l.x != 0.0f || l.y != 0.0f)
				debug_display.AddLine(chaos::StringTools::Printf("LEFT x : %0.3f   y : %0.3f", l.x, l.y).c_str(), 1.0f);

			glm::vec2 r = main_gamepad->GetXBOXStickDirection(chaos::MyGLFW::XBOX_RIGHT_AXIS);
			if (r.x != 0.0f || r.y != 0.0f)
				debug_display.AddLine(chaos::StringTools::Printf("RIGHT x : %0.3f  y : %0.3f", r.x, r.y).c_str(), 1.0f);
		}

		return true; // no redraw
	}


protected:

	boost::intrusive_ptr<MyGamepadManager> gamepad_manager;

	chaos::GLDebugOnScreenDisplay debug_display;

	boost::intrusive_ptr<chaos::MyGLFW::Gamepad> main_gamepad;
};

int _tmain(int argc, char ** argv, char ** env)
{
	chaos::Application::Initialize<chaos::Application>(argc, argv, env);

	chaos::WinTools::AllocConsoleAndRedirectStdOutput();

	chaos::MyGLFW::SingleWindowApplicationParams params;
	params.monitor       = nullptr;
	params.width         = 700;
	params.height        = 700;
	params.monitor_index = 0;
	chaos::MyGLFW::Window::RunSingleWindowApplication<MyGLFWWindowGamepadTest>(params);

	chaos::Application::Finalize();

	return 0;
}

