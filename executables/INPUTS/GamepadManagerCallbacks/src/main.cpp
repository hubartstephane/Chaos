
#include "chaos/Chaos.h"

class TestGamepadCallbacks : public chaos::GamepadCallbacks
{
public:

	TestGamepadCallbacks(chaos::ImGuiUserMessageObject* in_imgui_user_message) : imgui_user_message(in_imgui_user_message) {}

	virtual bool AcceptPhysicalDevice(chaos::PhysicalGamepad* physical_device) override
	{
		if (IsInputActive(physical_device->GetInputState(chaos::Key::GAMEPAD_A)))
			return true;
		if (imgui_user_message != nullptr)
			if (physical_device->IsAnyInputActive())
				imgui_user_message->AddLine("AcceptPhysicalDevice gamepad refused. Need A to be pressed to acquire", 0.1f);
		return false;
	}

	virtual bool OnGamepadConnected(chaos::Gamepad* gamepad) override
	{
		if (imgui_user_message != nullptr)
			imgui_user_message->AddLine("TestGamepadCallbacks::OnGamepadConnected");
		return true;
	}

	virtual bool OnGamepadDisconnected(chaos::Gamepad* gamepad) override
	{
		if (imgui_user_message != nullptr)
			imgui_user_message->AddLine("TestGamepadCallbacks::OnGamepadDisconnected");
		return true;
	}

	virtual void OnGamepadStateUpdated(class chaos::GamepadState& gamepad_state) override
	{
#define TEST_BUTTON(x) if (gamepad_state.GetInputValue(chaos::Key::GAMEPAD_##x)) imgui_user_message->AddLine("Down : " #x, 0.1f);

		if (imgui_user_message != nullptr)
		{
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

			glm::vec2 l = gamepad_state.GetInputValue(chaos::Input2D::GAMEPAD_LEFT_STICK);
			if (l.x != 0.0f || l.y != 0.0f)
				imgui_user_message->AddLine(chaos::StringTools::Printf("LEFT AXIS x : %0.3f   y : %0.3f", l.x, l.y).c_str(), 1.0f);

			glm::vec2 r = gamepad_state.GetInputValue(chaos::Input2D::GAMEPAD_RIGHT_STICK);
			if (r.x != 0.0f || r.y != 0.0f)
				imgui_user_message->AddLine(chaos::StringTools::Printf("RIGHT AXIS x : %0.3f  y : %0.3f", r.x, r.y).c_str(), 1.0f);

			float left_trigger = gamepad_state.GetInputValue(chaos::Input1D::GAMEPAD_LEFT_TRIGGER);
			if (left_trigger)
				imgui_user_message->AddLine(chaos::StringTools::Printf("LEFT TRIGGER  %0.3f", left_trigger).c_str(), 1.0f);

			float right_trigger = gamepad_state.GetInputValue(chaos::Input1D::GAMEPAD_RIGHT_TRIGGER);
			if (right_trigger)
				imgui_user_message->AddLine(chaos::StringTools::Printf("RIGHT TRIGGER  %0.3f", right_trigger).c_str(), 1.0f);
		}
	}


protected:

	chaos::ImGuiUserMessageObject* imgui_user_message = nullptr;
};

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

	virtual void Finalize() override
	{
		main_gamepad = nullptr;
		chaos::Window::Finalize();
	}

	virtual bool OnInitialize(chaos::JSONReadConfiguration config) override
	{
		if (!chaos::Window::OnInitialize(config))
			return false;

		chaos::WindowApplication* window_application = chaos::Application::GetInstance();
		if (window_application == nullptr)
			return false;

		chaos::GamepadManager* gamepad_manager = window_application->GetGamepadManager();
		if (gamepad_manager == nullptr)
			return false;

		main_gamepad = gamepad_manager->AllocateGamepad(false, nullptr, new TestGamepadCallbacks(&imgui_user_message));
		if (main_gamepad == nullptr)
			return false;

		return true;
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

	chaos::ImGuiUserMessageObject imgui_user_message;

	chaos::shared_ptr<chaos::Gamepad> main_gamepad;
};

int main(int argc, char** argv, char** env)
{
	return chaos::RunSimpleWindowApplication<WindowOpenGLTest>(argc, argv, env);
}