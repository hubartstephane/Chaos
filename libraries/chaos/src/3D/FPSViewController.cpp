#include "chaos/ChaosPCH.h"
#include "chaos/ChaosInternals.h"

namespace chaos
{
	void FPSViewController::SetMouseEnabled(bool in_mouse_enabled)
	{
		if (mouse_enabled == in_mouse_enabled)
			return;
		mouse_captured = false;
		mouse_enabled = in_mouse_enabled;
	}

	void FPSViewController::Tick(GLFWwindow * glfw_window, float delta_time)
	{
		HandleMouseInputs(glfw_window, delta_time);
		HandleKeyboardInputs(delta_time);
	}

	void FPSViewController::HandleMouseInputs(GLFWwindow * glfw_window, float delta_time)
	{
		if (!IsMouseEnabled())
			return;

		// handle mouse movement
		if (mouse_captured || !config.must_click_to_rotate)
		{
			if (config.must_click_to_rotate && !CheckMouseInput(input_config.rotation_button))
				mouse_captured = false;
			else
			{
				double new_mouse_x = 0.0;
				double new_mouse_y = 0.0;
				glfwGetCursorPos(glfw_window, &new_mouse_x, &new_mouse_y);

				if (previous_mouse_x != INVALID_MOUSE_VALUE && previous_mouse_y != INVALID_MOUSE_VALUE) // only want to rotate if we have a valid mouse position
				{
					if (new_mouse_x != previous_mouse_x)
						fps_view.IncrementYaw(-(float)((new_mouse_x - previous_mouse_x) * config.mouse_sensibility));
					if (new_mouse_y != previous_mouse_y)
						fps_view.IncrementPitch(-(float)((new_mouse_y - previous_mouse_y) * config.mouse_sensibility));
				}
				previous_mouse_x = new_mouse_x;
				previous_mouse_y = new_mouse_y;
			}
		}
		else
		{
			if (CheckMouseInput(input_config.rotation_button))
			{
				mouse_captured = true;
				previous_mouse_x = INVALID_MOUSE_VALUE;
				previous_mouse_y = INVALID_MOUSE_VALUE;
				glfwGetCursorPos(glfw_window, &previous_mouse_x, &previous_mouse_y);
			}
		}
	}

	void FPSViewController::HandleKeyboardInputs(float delta_time)
	{
		// handles the keys for displacement
		if (CheckKeyboardInput(input_config.left_button))
			fps_view.StrafeLeft(config.strafe_speed * delta_time);
		if (CheckKeyboardInput(input_config.right_button))
			fps_view.StrafeRight(config.strafe_speed * delta_time);

		if (CheckKeyboardInput(input_config.forward_button))
			fps_view.GoForward(config.forward_speed * delta_time);
		if (CheckKeyboardInput(input_config.backward_button))
			fps_view.GoBackward(config.back_speed * delta_time);

		if (CheckKeyboardInput(input_config.up_button))
			fps_view.position.y += config.up_speed * delta_time;
		if (CheckKeyboardInput(input_config.down_button))
			fps_view.position.y -= config.down_speed * delta_time;

		if (CheckKeyboardInput(input_config.yaw_left_button))
			fps_view.IncrementYaw(config.yaw_speed * delta_time);
		if (CheckKeyboardInput(input_config.yaw_right_button))
			fps_view.IncrementYaw(-config.yaw_speed * delta_time);

		if (CheckKeyboardInput(input_config.pitch_up_button))
			fps_view.IncrementPitch(config.pitch_speed * delta_time);
		if (CheckKeyboardInput(input_config.pitch_down_button))
			fps_view.IncrementPitch(-config.pitch_speed * delta_time);
	}

	bool FPSViewController::CheckMouseInput(MouseButton button) const
	{
		if (KeyboardAndMouseState const * keyboard_and_mouse_state = KeyboardAndMouseState::GetInstance())
			if (ButtonState const* state = keyboard_and_mouse_state->GetKeyState(button))
				return state->IsPressed();
		return false;
	}

	bool FPSViewController::CheckKeyboardInput(KeyboardButton button) const
	{
		if (KeyboardAndMouseState const * keyboard_and_mouse_state = KeyboardAndMouseState::GetInstance())
			if (ButtonState const* state = keyboard_and_mouse_state->GetKeyState(button))
				return state->IsPressed();
		return false;
	}

}; // namespace chaos
