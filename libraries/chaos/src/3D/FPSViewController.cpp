#include "chaos/ChaosPCH.h"
#include "chaos/ChaosInternals.h"

namespace chaos
{
	char const* FPSViewController::GetInputReceiverName() const
	{
		return "FPSViewController";
	}

	void FPSViewController::SetMouseEnabled(bool in_mouse_enabled)
	{
		mouse_enabled = in_mouse_enabled;
	}

	bool FPSViewController::EnumerateInputActions(InputActionProcessor & in_action_processor, EnumerateInputActionContext in_context)
	{
		float frame_duration = (float)FrameTimeManager::GetInstance()->GetCurrentFrameDuration();

		// check for key displacement
		auto CheckCameraKey = [this, frame_duration, &in_action_processor](Key const & key, char const * title, float speed, void (FPSView::*func)(float))
		{
			if (in_action_processor.CheckAndProcess(Active(key), title, [this, frame_duration, speed, &func, key]()
			{
				(fps_view.*func)(speed * frame_duration);
			}))
			{
				return true;
			}
			return false; 
		};

		if (CheckCameraKey(input_config.left_button, "Strafe Left", config.strafe_speed, &FPSView::StrafeLeft) ||
			CheckCameraKey(input_config.right_button, "Strafe Right", config.strafe_speed, &FPSView::StrafeRight) ||
			CheckCameraKey(input_config.forward_button, "Move Forward", config.forward_speed, &FPSView::GoForward) ||
			CheckCameraKey(input_config.backward_button, "Move Backward", config.back_speed, &FPSView::GoBackward) ||
			CheckCameraKey(input_config.up_button, "Move Up", config.up_speed, &FPSView::GoUp) ||
			CheckCameraKey(input_config.down_button, "Move Down", config.down_speed, &FPSView::GoDown) ||
			CheckCameraKey(input_config.yaw_left_button, "Increment Yaw", config.yaw_speed, &FPSView::IncrementYaw) ||
			CheckCameraKey(input_config.yaw_right_button, "Decrement Yaw", -config.yaw_speed, &FPSView::IncrementYaw) ||
			CheckCameraKey(input_config.pitch_up_button, "Increment Pitch", config.pitch_speed, &FPSView::IncrementPitch) ||
			CheckCameraKey(input_config.pitch_down_button, "Decrement Pitch", -config.pitch_speed, &FPSView::IncrementPitch))
		{
			return true;
		}

		// move the camera
		if (mouse_enabled)
		{
			glm::vec2 mouse_delta = { 0.0f, 0.0f };

			auto MoveCameraRequest = And(
				Active(input_config.rotation_button), 
				QueryInput(Input2D::MOUSE_DELTA, &mouse_delta, QueryInputRequestType::Active));

			if (in_action_processor.CheckAndProcess(MoveCameraRequest, "Move Camera", [this, &mouse_delta]()
			{
				fps_view.IncrementYaw(-(float)(mouse_delta.x * config.mouse_sensibility));
				fps_view.IncrementPitch(-(float)(mouse_delta.y * config.mouse_sensibility));
			
			}))
			{
				return true;
			}
		}
		return false;
	}

}; // namespace chaos
