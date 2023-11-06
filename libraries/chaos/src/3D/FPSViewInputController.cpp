#include "chaos/ChaosPCH.h"
#include "chaos/ChaosInternals.h"

namespace chaos
{
	void FPSViewInputController::SetMouseEnabled(bool in_mouse_enabled)
	{
		if (mouse_enabled == in_mouse_enabled)
			return;
		mouse_captured = false;
		mouse_enabled = in_mouse_enabled;
	}

	void FPSViewInputController::Tick(GLFWwindow * glfw_window, float delta_time)
	{
		HandleMouseInputs(glfw_window, delta_time);
		HandleKeyboardInputs(delta_time);
	}

	void FPSViewInputController::HandleMouseInputs(GLFWwindow * glfw_window, float delta_time)
	{
		if (!IsMouseEnabled())
			return;

		// handle mouse movement
		if (mouse_captured || !must_click_to_rotate)
		{
			if (must_click_to_rotate && !KeyboardState::GetMouseButtonState(MouseButton::BUTTON_1)->IsPressed())
				mouse_captured = false;
			else
			{
				double new_mouse_x = 0.0;
				double new_mouse_y = 0.0;
				glfwGetCursorPos(glfw_window, &new_mouse_x, &new_mouse_y);

				if (previous_mouse_x != INVALID_MOUSE_VALUE && previous_mouse_y != INVALID_MOUSE_VALUE) // only want to rotate if we have a valid mouse position
				{
					if (new_mouse_x != previous_mouse_x)
						fps_controller.IncrementYaw(-(float)((new_mouse_x - previous_mouse_x) * movement_speed.mouse_sensibility));
					if (new_mouse_y != previous_mouse_y)
						fps_controller.IncrementPitch(-(float)((new_mouse_y - previous_mouse_y) * movement_speed.mouse_sensibility));
				}
				previous_mouse_x = new_mouse_x;
				previous_mouse_y = new_mouse_y;
			}
		}
		else
		{
			if (KeyboardState::GetMouseButtonState(MouseButton::BUTTON_1)->IsPressed())
			{
				mouse_captured = true;
				previous_mouse_x = INVALID_MOUSE_VALUE;
				previous_mouse_y = INVALID_MOUSE_VALUE;
				glfwGetCursorPos(glfw_window, &previous_mouse_x, &previous_mouse_y);
			}
		}
	}

	void FPSViewInputController::HandleKeyboardInputs(float delta_time)
	{
		// handles the keys for displacement
		if (CheckKeyInput(keyboard_config.left))
			fps_controller.StrafeLeft(movement_speed.strafe * delta_time);
		if (CheckKeyInput(keyboard_config.right))
			fps_controller.StrafeRight(movement_speed.strafe * delta_time);
		if (CheckKeyInput(keyboard_config.forward))
			fps_controller.GoForward(movement_speed.forward * delta_time);
		if (CheckKeyInput(keyboard_config.backward))
			fps_controller.GoBackward(movement_speed.back * delta_time);
		if (CheckKeyInput(keyboard_config.up))
			fps_controller.position.y -= movement_speed.down * delta_time;
		if (CheckKeyInput(keyboard_config.down))
			fps_controller.position.y += movement_speed.up * delta_time;

		if (CheckKeyInput(keyboard_config.yaw_left))
			fps_controller.IncrementYaw(movement_speed.yaw * delta_time);
		if (CheckKeyInput(keyboard_config.yaw_right))
			fps_controller.IncrementYaw(-movement_speed.yaw * delta_time);

		if (CheckKeyInput(keyboard_config.pitch_up))
			fps_controller.IncrementPitch(movement_speed.pitch * delta_time);
		if (CheckKeyInput(keyboard_config.pitch_down))
			fps_controller.IncrementPitch(-movement_speed.pitch * delta_time);
	}

	bool FPSViewInputController::CheckKeyInput(KeyboardButton button) const
	{
		return (button != KeyboardButton::UNKNOWN && KeyboardState::GetKeyboardButtonState(button)->IsPressed());
	}

}; // namespace chaos
