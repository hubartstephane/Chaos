#include "chaos/ChaosPCH.h"

#include "chaos/ChaosInternals.h"

namespace chaos
{
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
		for (size_t i = 0; keys[i].IsValid(); ++i)
			if (CheckKeyDown(keys[i]))
				return true;
		return false;
	}

	bool InputReceiverInterface::CheckKeyDown(Key key)
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

	bool InputReceiverInterface::DoCheckKeyDown(Key key) const
	{
		if (KeyboardAndMouseState const * keyboard_and_mouse_state = KeyboardAndMouseState::GetInstance())
			if (KeyState const* state = keyboard_and_mouse_state->GetInputState(key))
				return state->IsDown();
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
		return ProcessKeyAction(mouse_button_event);
	}

	bool InputReceiverInterface::OnKeyEventImpl(KeyEvent const& key_event)
	{
		return ProcessKeyAction(key_event);
	}

	bool InputReceiverInterface::ProcessKeyAction(KeyEventBase const& key_event)
	{
		// XXX: do not use WindowApplication::consumption_cache
		//      we only want to register inside it the key for current key_event
		//      this is done inside OnKeyEventInputReceiverTraverser
		InputConsumptionCache consumption_cache;

		auto process_function = [&key_event, &consumption_cache](InputReceiverInterface* in_input_receiver, InputDeviceInterface const* in_input_device) // XXX: mandatory to have a VARIABLE lambda so that the underlying DelegateTraverser's LightweightFunction does not point on a deleted object
		{
			OnKeyEventInputActionEnumerator action_enumerator(in_input_receiver, key_event, in_input_device, &consumption_cache);
			if (in_input_receiver->EnumerateInputActions(action_enumerator, EnumerateInputActionContext::OnEvent))
			{
				// XXX: prevent the key to be handled in poll event has well
				//      this is the right place to do this because we have the proper input_device
				if (WindowApplication* window_application = Application::GetInstance())
					window_application->GetInputConsumptionCache().TryConsumeInput(key_event.key, in_input_device);
				return true;
			}
			return false;
		};
		DelegateInputReceiverTraverser traverser(process_function);

		return traverser.Traverse(this);
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
