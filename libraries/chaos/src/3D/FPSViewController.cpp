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

	bool FPSViewController::EnumerateInputActions(InputActionEnumerator & in_action_enumerator, EnumerateInputActionContext in_context)
	{
		float frame_duration = (float)FrameTimeManager::GetInstance()->GetCurrentFrameDuration();

		auto CheckCameraKey = [this, frame_duration, &in_action_enumerator](const Key & key, char const * title, float speed, void (FPSView::*func)(float))
		{
			if (in_action_enumerator.CheckAndProcess(RequestKeyDown(key), title, [this, frame_duration, speed, &func, key]()
			{
				(fps_view.*func)(speed * frame_duration);
			}))
			{
				return true;
			}
			return false; 
		};

		return 
			CheckCameraKey(input_config.left_button, "Strafe Left", config.strafe_speed, &FPSView::StrafeLeft) ||
			CheckCameraKey(input_config.right_button, "Strafe Right", config.strafe_speed, &FPSView::StrafeRight) ||
			CheckCameraKey(input_config.forward_button, "Move Forward", config.forward_speed, &FPSView::GoForward) ||
			CheckCameraKey(input_config.backward_button, "Move Backward", config.back_speed, &FPSView::GoBackward) ||
			CheckCameraKey(input_config.up_button, "Move Up", config.up_speed, &FPSView::GoUp) ||
			CheckCameraKey(input_config.down_button, "Move Down", config.down_speed, &FPSView::GoDown) ||
			CheckCameraKey(input_config.yaw_left_button, "Increment Yaw", config.yaw_speed, &FPSView::IncrementYaw) ||
			CheckCameraKey(input_config.yaw_right_button, "Decrement Yaw", -config.yaw_speed, &FPSView::IncrementYaw) ||
			CheckCameraKey(input_config.pitch_up_button, "Increment Pitch", config.pitch_speed, &FPSView::IncrementPitch) ||
			CheckCameraKey(input_config.pitch_down_button, "Decrement Pitch", -config.pitch_speed, &FPSView::IncrementPitch);
	}

	bool FPSViewController::OnMouseMoveImpl(glm::vec2 const& delta)
	{
		if (!IsMouseEnabled())
			return false;

		if (config.must_click_to_rotate && !mouse_captured) // not ready to handle the mouse movement
			return false;

		fps_view.IncrementYaw(-(float)(delta.x * config.mouse_sensibility));
		fps_view.IncrementPitch(-(float)(delta.y * config.mouse_sensibility));

		return true;
	}

	bool FPSViewController::OnMouseButtonImpl(MouseButtonEvent const &mouse_button_event)
	{
		if (!IsMouseEnabled())
			return false;

		if (!config.must_click_to_rotate) // no need to catch input
			return false;

		if (mouse_button_event.IsKeyDown(input_config.rotation_button))
			mouse_captured = true;
		else if (mouse_button_event.IsKeyReleased(input_config.rotation_button))
			mouse_captured = false;

		return true;
	}

}; // namespace chaos
