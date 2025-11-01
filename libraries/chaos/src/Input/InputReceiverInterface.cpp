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

	bool InputReceiverInterface::CheckKeyDown(Key const* keys)
	{
		// early exit
		if (keys == nullptr)
			return false;
		// iteration
		for (size_t i = 0; keys[i] != Key::UNKNOWN; ++i)
			if (CheckKeyDown(keys[i]))
				return true;
		return false;
	}

	bool InputReceiverInterface::CheckKeyDown(Key key)
	{
		if (DoCheckKeyDown(key))
		{
			if (IsKeyboardInput(key))
				SetInputMode(InputMode::KEYBOARD);
			else if (IsMouseInput(key))
				SetInputMode(InputMode::MOUSE);
			else if (IsGamepadInput(key))
				SetInputMode(InputMode::GAMEPAD);
			return true;
		}
		return false;
	}

	bool InputReceiverInterface::DoCheckKeyDown(Key key) const
	{
		if (KeyboardAndMouseDevice const * keyboard_and_mouse_device = KeyboardAndMouseDevice::GetInstance())
			return IsInputActive(keyboard_and_mouse_device->GetInputState(key));
		return false;
	}

	void InputReceiverInterface::OnInputModeChanged(InputMode new_mode, InputMode old_mode)
	{
	}

	bool InputReceiverInterface::OnMouseMove(glm::vec2 const& delta)
	{
		if (OnMouseMoveImpl(delta))
		{
			SetInputMode(InputMode::MOUSE);
			return true;
		}
		return false;
	}

	bool InputReceiverInterface::OnMouseButton(MouseButtonEvent const &mouse_button_event)
	{
		if (OnMouseButtonImpl(mouse_button_event))
		{
			SetInputMode(InputMode::MOUSE);
			return true;
		}
		return false;
	}

	bool InputReceiverInterface::OnMouseWheel(double scroll_x, double scroll_y)
	{
		if (OnMouseWheelImpl(scroll_x, scroll_y))
		{
			SetInputMode(InputMode::MOUSE);
			return true;
		}
		return false;
	}

	bool InputReceiverInterface::OnKeyEvent(KeyEvent const & key_event)
	{
		if (OnKeyEventImpl(key_event))
		{
			SetInputMode(InputMode::KEYBOARD);
			return true;
		}
		return false;
	}

	bool InputReceiverInterface::OnCharEvent(unsigned int c)
	{
		if (OnCharEventImpl(c))
		{
			SetInputMode(InputMode::KEYBOARD);
			return true;
		}
		return false;
	}

	bool InputReceiverInterface::OnMouseMoveImpl(glm::vec2 const& delta)
	{
		return false;
	}

	bool InputReceiverInterface::OnMouseButtonImpl(MouseButtonEvent const& mouse_button_event)
	{
		return ProcessInputEvent(mouse_button_event.key);
	}

	bool InputReceiverInterface::OnKeyEventImpl(KeyEvent const& key_event)
	{
		return ProcessInputEvent(key_event.key);
	}

	bool InputReceiverInterface::OnMouseWheelImpl(double scroll_x, double scroll_y)
	{
		return false;
	}

	bool InputReceiverInterface::OnCharEventImpl(unsigned int c)
	{
		return false;
	}

	bool InputReceiverInterface::InvokeWithUpgradedInputDevice(InputDeviceInterface const * in_input_device, InvokeWithUpgradedInputDeviceFunction in_func)
	{
		return in_func(in_input_device); // by default, simple passthrough
	}

	bool InputReceiverInterface::EnumerateInputActions(InputActionEnumerator & in_action_enumerator, EnumerateInputActionContext in_context)
	{
		return false;
	}

	bool InputReceiverInterface::TraverseInputReceiver(InputReceiverTraverser & in_traverser, InputDeviceInterface const* in_input_device)
	{
		return in_traverser.Process(this, in_input_device);
	}

}; // namespace chaos
