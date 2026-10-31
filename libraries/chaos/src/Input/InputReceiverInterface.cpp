#include "chaos/ChaosPCH.h"

#include "chaos/ChaosInternals.h"

namespace chaos
{
	char const* InputReceiverInterface::GetInputReceiverName() const
	{
		if (Object const* object = auto_cast(this))
			return object->GetClass()->GetClassName().c_str();
		return nullptr;
	}

	void InputReceiverInterface::SetInputMode(InputMode new_mode)
	{
		if (new_mode == input_mode)
			return;
		InputMode old_mode = input_mode;
		input_mode = new_mode;
		OnInputModeChanged(new_mode, old_mode);
	}

	void InputReceiverInterface::SetInputModeForInvolvedDevices(InputDeviceTypeMask devices)
	{
		// early exit
		if (devices == InputDeviceTypeMask::None)
			return;

		// avoid input mode change if possible
		if (input_mode == InputMode::Keyboard && HasAnyFlags(devices, InputDeviceTypeMask::Keyboard))
			return;
		if (input_mode == InputMode::Mouse && HasAnyFlags(devices, InputDeviceTypeMask::Mouse))
			return;
		if (input_mode == InputMode::Gamepad && HasAnyFlags(devices, InputDeviceTypeMask::Gamepad))
			return;

		// change the mode
		if (HasAnyFlags(devices, InputDeviceTypeMask::Keyboard))
			return SetInputMode(InputMode::Keyboard);
		if (HasAnyFlags(devices, InputDeviceTypeMask::Mouse))
			return SetInputMode(InputMode::Mouse);
		if (HasAnyFlags(devices, InputDeviceTypeMask::Gamepad))
			return SetInputMode(InputMode::Gamepad);
	}

	void InputReceiverInterface::OnInputModeChanged(InputMode new_mode, InputMode old_mode)
	{
	}

	bool InputReceiverInterface::OnMouseMove(MouseMoveEvent const& mouse_move_event)
	{
		if (OnMouseMoveImpl(mouse_move_event))
		{
			SetInputMode(InputMode::Mouse);
			return true;
		}
		return false;
	}

	bool InputReceiverInterface::OnMouseButton(MouseButtonEvent const &mouse_button_event)
	{
		if (OnMouseButtonImpl(mouse_button_event))
		{
			SetInputMode(InputMode::Mouse);
			return true;
		}
		return false;
	}

	bool InputReceiverInterface::OnMouseWheel(MouseWheelEvent const& mouse_wheel_event)
	{
		if (OnMouseWheelImpl(mouse_wheel_event))
		{
			SetInputMode(InputMode::Mouse);
			return true;
		}
		return false;
	}

	bool InputReceiverInterface::OnKeyEvent(KeyEvent const & key_event)
	{
		if (OnKeyEventImpl(key_event))
		{
			SetInputMode(InputMode::Keyboard);
			return true;
		}
		return false;
	}

	bool InputReceiverInterface::OnCharEvent(CharEvent const& char_event)
	{
		if (OnCharEventImpl(char_event))
		{
			SetInputMode(InputMode::Keyboard);
			return true;
		}
		return false;
	}

	bool InputReceiverInterface::OnMouseMoveImpl(MouseMoveEvent const& mouse_move_event)
	{
		return ProcessInputEvent(Input2D::MouseDelta);
	}

	bool InputReceiverInterface::OnMouseButtonImpl(MouseButtonEvent const& mouse_button_event)
	{
		return ProcessInputEvent(mouse_button_event.key);
	}

	bool InputReceiverInterface::OnKeyEventImpl(KeyEvent const& key_event)
	{
		return ProcessInputEvent(key_event.key);
	}

	bool InputReceiverInterface::OnMouseWheelImpl(MouseWheelEvent const & mouse_wheel_event)
	{
		return ProcessInputEvent(Input2D::MouseWheel);
	}

	bool InputReceiverInterface::OnCharEventImpl(CharEvent const& char_event)
	{
		return false;
	}

	bool InputReceiverInterface::InvokeWithUpgradedInputDevice(InputDeviceInterface const * in_input_device, InvokeWithUpgradedInputDeviceFunction in_func)
	{
		return in_func(in_input_device); // by default, simple passthrough
	}

	bool InputReceiverInterface::EnumerateInputActions(InputActionProcessor & in_action_processor, EnumerateInputActionContext in_context)
	{
		return false;
	}

	bool InputReceiverInterface::TraverseInputReceiver(InputReceiverTraverser & in_traverser)
	{
		return in_traverser.Process(this);
	}

}; // namespace chaos
