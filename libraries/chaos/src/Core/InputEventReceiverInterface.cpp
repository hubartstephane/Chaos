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
			if (KeyState const* state = keyboard_and_mouse_state->GetKeyState(key))
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

	bool InputEventReceiverInterface::OnMouseButtonImpl(MouseButtonEvent const &mouse_button_event)
	{
		return false;
	}

	bool InputEventReceiverInterface::OnMouseWheelImpl(double scroll_x, double scroll_y)
	{
		return false;
	}

	bool InputEventReceiverInterface::OnKeyEventImpl(KeyEvent const & key_event)
	{
		return ProcessKeyActions(key_event);
	}

	bool InputEventReceiverInterface::OnCharEventImpl(unsigned int c)
	{
		return false;
	}

	bool InputEventReceiverInterface::ProcessKeyActions(KeyEvent const & key_event)
	{
		class ProcessKeyActionEnumerator : public KeyActionEnumerator
		{
		public:

			/** constructor */
			ProcessKeyActionEnumerator(KeyEvent const & in_key_event):
				key_event(in_key_event)
			{}

			/** override */
			virtual bool operator () (KeyRequest const & in_request, char const * in_title, bool in_enabled, LightweightFunction<void()> in_key_action) override
			{
				if (in_enabled && in_request.MatchEvent(key_event))
				{
					in_key_action();
					return true;
				}
				return false; 
			}

		protected:

			/** the event to check */
			KeyEvent key_event;
		};

		ProcessKeyActionEnumerator action_enumerator(key_event);
		return TraverseInputEventReceiverHierarchy([this, &action_enumerator](InputEventReceiverInterface * in_event_receiver)
		{
			return in_event_receiver->EnumerateKeyActions(action_enumerator);
		});
	}

	bool InputEventReceiverInterface::EnumerateKeyActions(KeyActionEnumerator & in_action_enumerator)
	{
		return false;
	}

	bool InputEventReceiverInterface::TraverseInputEventReceiverHierarchy(LightweightFunction<bool(InputEventReceiverInterface*)> event_func)
	{
		return event_func(this);
	}

}; // namespace chaos
