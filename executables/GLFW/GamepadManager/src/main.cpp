
#include "chaos/Chaos.h"

class TestGamepadCallbacks : public chaos::GamepadCallbacks
{
public:

  TestGamepadCallbacks(chaos::ImGuiUserMessageObject * in_imgui_user_message) : imgui_user_message(in_imgui_user_message) {}

  virtual bool AcceptPhysicalDevice(chaos::PhysicalGamepad * physical_device) override
  {
	  if (!physical_device->IsKeyDown(chaos::Key::GAMEPAD_A))
		  return false;
	  return true;
  }

  virtual bool OnGamepadConnected(chaos::Gamepad * gamepad) override
  {
    if (imgui_user_message != nullptr)
      imgui_user_message->AddLine("TestGamepadCallbacks::OnGamepadConnected");
    return true;
  }

  virtual bool OnGamepadDisconnected(chaos::Gamepad * gamepad) override
  {
    if (imgui_user_message != nullptr)
      imgui_user_message->AddLine("TestGamepadCallbacks::OnGamepadDisconnected");
    return true;
  }

  virtual void OnGamepadStateUpdated(class chaos::GamepadState & gamepad_state) override
  {
  //  if (gamepad_state.IsButtonPressed(chaos::Key::GAMEPAD_Y))
    {
   //   if (imgui_user_message != nullptr)
    //    imgui_user_message->AddLine("Y");
    }
  }


protected:

  chaos::ImGuiUserMessageObject * imgui_user_message = nullptr;
};




class MyGamepadManager : public chaos::GamepadManager
{
public:

  MyGamepadManager(chaos::ImGuiUserMessageObject * in_imgui_user_message) : imgui_user_message(in_imgui_user_message) {}

protected:

	virtual bool DoPoolGamepad(chaos::PhysicalGamepad * physical_gamepad) override
	{
		return true;
	}

protected:

	chaos::ImGuiUserMessageObject * imgui_user_message = nullptr;
};

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

		return true;
	}

	virtual void Finalize() override
	{
		main_gamepad = nullptr;
    gamepad_manager = nullptr;
		chaos::Window::Finalize();
	}

	virtual bool OnInitialize(chaos::JSONReadConfiguration config) override
	{
		if (!chaos::Window::OnInitialize(config))
			return false;

		gamepad_manager = new MyGamepadManager(&imgui_user_message);

		main_gamepad = gamepad_manager->AllocateGamepad(false, new TestGamepadCallbacks(&imgui_user_message));
		return true;
	}

	virtual bool DoTick(float delta_time) override
	{
		gamepad_manager->Tick(0.0f);

		if (main_gamepad->IsPresent())
		{

#if 0
			int button_count = main_gamepad->GetButtonCount();
			for (int i = 0; i < button_count; ++i)
			{
				if (main_gamepad->IsButtonPressed(i))
					imgui_user_message.AddLine(chaos::StringTools::Printf("BUTTON [%d] PRESSED", i).c_str(), 1.0f);
			}

			int axis_count = main_gamepad->GetAxisCount();
			for (int i = 0; i < axis_count; ++i)
			{
				float value = main_gamepad->GetInputValue(i);
				if (value != 0.0f)
					imgui_user_message.AddLine(chaos::StringTools::Printf("AXIS [%d] = %f", i, value).c_str(), 1.0f);
			}

			if (main_gamepad->IsAnyAxisAction())
				imgui_user_message.AddLine("AXIS ACTION", 1.0f);

#else

#define TEST_BUTTON(x) if (main_gamepad->IsKeyDown(chaos::Key::GAMEPAD_##x))\
imgui_user_message.AddLine("Pressed : " #x, 1.0f);

			TEST_BUTTON(A);
			TEST_BUTTON(B);
			TEST_BUTTON(X);
			TEST_BUTTON(Y);

			TEST_BUTTON(LEFT_BUMPER);
			TEST_BUTTON(RIGHT_BUMPER);
			TEST_BUTTON(SPECIAL_LEFT);
			TEST_BUTTON(SPECIAL_RIGHT);

			TEST_BUTTON(LEFT_TRIGGER);
			TEST_BUTTON(RIGHT_TRIGGER);

			TEST_BUTTON(DPAD_UP);
			TEST_BUTTON(DPAD_DOWN);
			TEST_BUTTON(DPAD_LEFT);
			TEST_BUTTON(DPAD_RIGHT);

			glm::vec2 l = main_gamepad->GetInputValue(chaos::Input2D::LEFT_STICK);
			if (l.x != 0.0f || l.y != 0.0f)
				imgui_user_message.AddLine(chaos::StringTools::Printf("LEFT AXIS x : %0.3f   y : %0.3f", l.x, l.y).c_str(), 1.0f);

			glm::vec2 r = main_gamepad->GetInputValue(chaos::Input2D::RIGHT_STICK);
			if (r.x != 0.0f || r.y != 0.0f)
				imgui_user_message.AddLine(chaos::StringTools::Printf("RIGHT AXIS x : %0.3f  y : %0.3f", r.x, r.y).c_str(), 1.0f);

			float left_trigger = main_gamepad->GetInputValue(chaos::Input1D::LEFT_TRIGGER);
			if (left_trigger)
				imgui_user_message.AddLine(chaos::StringTools::Printf("LEFT TRIGGER  %0.3f", left_trigger).c_str(), 1.0f);

			float right_trigger = main_gamepad->GetInputValue(chaos::Input1D::RIGHT_TRIGGER);
			if (right_trigger)
				imgui_user_message.AddLine(chaos::StringTools::Printf("RIGHT TRIGGER  %0.3f", right_trigger).c_str(), 1.0f);
#endif
		}

		return true; // no redraw
	}

	virtual void OnDrawImGuiContent() override
	{
		chaos::Window::OnDrawImGuiContent();

		chaos::ImGuiTools::FullViewportWindow("fullscreen", 0, [this]()
		{
			imgui_user_message.OnDrawImGuiContent(this);
		});
	}

protected:

	chaos::shared_ptr<MyGamepadManager> gamepad_manager;

	chaos::ImGuiUserMessageObject imgui_user_message;

	chaos::shared_ptr<chaos::Gamepad> main_gamepad;
};

int main(int argc, char ** argv, char ** env)
{
	return chaos::RunSimpleWindowApplication<WindowOpenGLTest>(argc, argv, env);
}