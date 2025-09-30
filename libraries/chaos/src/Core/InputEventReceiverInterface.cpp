#include "chaos/ChaosPCH.h"

#include "chaos/ChaosInternals.h"

namespace chaos
{
	void InputEventReceiverInterface::SetInputMode(InputMode new_mode)
	{
		if (new_mode == input_mode)
			return;
		InputMode old_mode = input_mode;
		input_mode = new_mode;
		OnInputModeChanged(new_mode, old_mode);
	}

	bool InputEventReceiverInterface::CheckKeyDown(Key const* keys)
	{
		// early exit
		if (keys == nullptr)
			return false;
		// iteration
		for (size_t i = 0; keys[i].IsValid(); ++i)
			if (CheckKeyDown(keys[i]))
				return true;
		return false;
	}

	bool InputEventReceiverInterface::CheckKeyDown(Key key)
	{
		if (DoCheckKeyDown(key))
		{
			if (key.IsKeyboardKey())
				SetInputMode(InputMode::KEYBOARD);
			else if (key.IsMouseKey())
				SetInputMode(InputMode::MOUSE);
			else if (key.IsGamepadKey())
				SetInputMode(InputMode::GAMEPAD);
			return true;
		}
		return false;
	}

	bool InputEventReceiverInterface::DoCheckKeyDown(Key key) const
	{
		if (KeyboardAndMouseState const * keyboard_and_mouse_state = KeyboardAndMouseState::GetInstance())
			if (KeyState const* state = keyboard_and_mouse_state->GetInputState(key))
				return state->IsDown();
		return false;
	}

	void InputEventReceiverInterface::OnInputModeChanged(InputMode new_mode, InputMode old_mode)
	{
	}

	bool InputEventReceiverInterface::OnMouseMove(glm::vec2 const& delta)
	{
		if (OnMouseMoveImpl(delta))
		{
			SetInputMode(InputMode::MOUSE);
			return true;
		}
		return false;
	}

	bool InputEventReceiverInterface::OnMouseButton(MouseButtonEvent const &mouse_button_event)
	{
		if (OnMouseButtonImpl(mouse_button_event))
		{
			SetInputMode(InputMode::MOUSE);
			return true;
		}
		return false;
	}

	bool InputEventReceiverInterface::OnMouseWheel(double scroll_x, double scroll_y)
	{
		if (OnMouseWheelImpl(scroll_x, scroll_y))
		{
			SetInputMode(InputMode::MOUSE);
			return true;
		}
		return false;
	}

	bool InputEventReceiverInterface::OnKeyEvent(KeyEvent const & key_event)
	{
		if (OnKeyEventImpl(key_event))
		{
			SetInputMode(InputMode::KEYBOARD);
			return true;
		}
		return false;
	}

	bool InputEventReceiverInterface::OnCharEvent(unsigned int c)
	{
		if (OnCharEventImpl(c))
		{
			SetInputMode(InputMode::KEYBOARD);
			return true;
		}
		return false;
	}

	bool InputEventReceiverInterface::OnMouseMoveImpl(glm::vec2 const& delta)
	{
		return false;
	}

	bool InputEventReceiverInterface::OnMouseButtonImpl(MouseButtonEvent const& mouse_button_event)
	{
		return ProcessKeyAction(mouse_button_event);
	}

	bool InputEventReceiverInterface::OnKeyEventImpl(KeyEvent const& key_event)
	{
		return ProcessKeyAction(key_event);
	}

	bool InputEventReceiverInterface::ProcessKeyAction(KeyEventBase const& key_event)
	{
		KeyboardAndMouseState const* keyboard_and_mouse = KeyboardAndMouseState::GetInstance();
		if (keyboard_and_mouse == nullptr)
			return false;

		// XXX: do not use WindowApplication::consumption_cache
		//      we only want to register inside it the key for current key_event
		//      this is done inside OnKeyEventInputEventReceiverHierarchyTraverser
		InputConsumptionCache consumption_cache;
		OnKeyEventInputEventReceiverHierarchyTraverser traverser(key_event, keyboard_and_mouse, &consumption_cache);
		return traverser.Traverse(this);
	}

	bool InputEventReceiverInterface::OnMouseWheelImpl(double scroll_x, double scroll_y)
	{
		return false;
	}

	bool InputEventReceiverInterface::OnCharEventImpl(unsigned int c)
	{
		return false;
	}

	bool InputEventReceiverInterface::InvokeWithUpgradedInputDevice(InputDeviceInterface const * in_input_device, InvokeWithUpgradedInputDeviceFunction in_func)
	{
		return in_func(in_input_device); // by default, simple passthrough
	}

	bool InputEventReceiverInterface::EnumerateInputActions(InputActionEnumerator & in_action_enumerator, EnumerateInputActionContext in_context)
	{
		return false;
	}

	bool InputEventReceiverInterface::TraverseInputEventReceiverHierarchy(InputEventReceiverHierarchyTraverser & in_traverser)
	{
		return in_traverser.Process(this);
	}

}; // namespace chaos
