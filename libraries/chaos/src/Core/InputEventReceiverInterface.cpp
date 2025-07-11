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

	bool InputEventReceiverInterface::CheckKeyPressed(Key const* keys, bool previous_frame)
	{
		// early exit
		if (keys == nullptr)
			return false;
		// iteration
		for (size_t i = 0; keys[i].IsValid(); ++i)
			if (CheckKeyPressed(keys[i], previous_frame))
				return true;
		return false;
	}

	bool InputEventReceiverInterface::CheckKeyPressed(Key key, bool previous_frame)
	{
		if (DoCheckKeyPressed(key, previous_frame))
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

	bool InputEventReceiverInterface::DoCheckKeyPressed(Key key, bool previous_frame)
	{
		if (ButtonState const* state = KeyboardState::GetKeyState(key))
			return state->IsPressed(previous_frame);
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
				if (in_enabled && key_event.MatchRequest(in_request))
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
		return EnumerateKeyActions(action_enumerator);
	}

	bool InputEventReceiverInterface::EnumerateKeyActions(KeyActionEnumerator & in_action_enumerator)
	{
		return false;
	}

	bool InputEventReceiverInterface::DispatchEventToHierarchy(LightweightFunction<bool(InputEventReceiverInterface*)> event_func)
	{
		return event_func(this);
	}

}; // namespace chaos
