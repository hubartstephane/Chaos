namespace chaos
{
#ifdef CHAOS_FORWARD_DECLARATION

	template<typename T>
	class KeyEventBase;

#elif !defined CHAOS_TEMPLATE_IMPLEMENTATION

	/**
	* KeyEventBase: a generic key event
	*/

	template<typename T>
	class KeyEventBase
	{
		using key_type = T;

	public:

		/** check whether this is a key press or repeat event */
		bool IsKeyDownEvent() const
		{
			return (action == KeyAction::Press || action == KeyAction::Repeat);
		}
		/** check whether this is a key press event */
		bool IsKeyPressedEvent() const
		{
			return (action == KeyAction::Press);
		}
		/** check whether this is a key repeat event */
		bool IsKeyRepeatEvent() const
		{
			return (action == KeyAction::Repeat);
		}
		/** check whether this is a key release event */
		bool IsKeyReleasedEvent() const
		{
			return (action == KeyAction::Release);
		}

		/** check whether this is a key press event */
		bool IsKeyPressed(key_type in_key, KeyModifier in_required_modifiers = KeyModifier::None, KeyModifier in_forbidden_modifiers = KeyModifier::None) const
		{
			return MatchRequest({in_key, in_required_modifiers, in_forbidden_modifiers, KeyActionMask::Press});
		}
		/** check whether this is a key release event */
		bool IsKeyReleased(key_type in_key, KeyModifier in_required_modifiers = KeyModifier::None, KeyModifier in_forbidden_modifiers = KeyModifier::None) const
		{
			return MatchRequest({in_key, in_required_modifiers, in_forbidden_modifiers, KeyActionMask::Release});
		}
		/** check whether this is a key repeat event */
		bool IsKeyRepeat(key_type in_key, KeyModifier in_required_modifiers = KeyModifier::None, KeyModifier in_forbidden_modifiers = KeyModifier::None) const
		{
			return MatchRequest({in_key, in_required_modifiers, in_forbidden_modifiers, KeyActionMask::Repeat});
		}
		/** check whether this is a key press or repeat event */
		bool IsKeyDown(key_type in_key, KeyModifier in_required_modifiers = KeyModifier::None, KeyModifier in_forbidden_modifiers = KeyModifier::None) const
		{
			return MatchRequest({in_key, in_required_modifiers, in_forbidden_modifiers, KeyActionMask::Down});
		}

		/** check whether the key event is for considered key */
		bool IsKeyEvent(key_type in_key, KeyModifier in_required_modifiers = KeyModifier::None, KeyModifier in_forbidden_modifiers = KeyModifier::None) const
		{
			if (key != in_key)
				return false;
			if (in_required_modifiers != KeyModifier::None && !HasAllFlags(modifiers, in_required_modifiers))
				return false;
			if (in_forbidden_modifiers != KeyModifier::None && HasAnyFlags(modifiers, in_forbidden_modifiers))
				return false;
			return true;
		}

		/** check whether the event matches the request */
		bool MatchRequest(KeyRequestBase<key_type> const & in_request) const
		{
			if (!IsKeyEvent(in_request.key, in_request.required_modifiers, in_request.forbidden_modifiers))
				return false;

			switch (action)
			{
			case KeyAction::Release:
				if (!HasAnyFlags(in_request.action_mask, KeyActionMask::Release))
					return false;
				break;

			case KeyAction::Press:
				if (!HasAnyFlags(in_request.action_mask, KeyActionMask::Press))
					return false;
				break;

			case KeyAction::Repeat:
				if (!HasAnyFlags(in_request.action_mask, KeyActionMask::Repeat))
					return false;
				break;

			default:
				assert(0);
			}

			return true;
		}

	public:

		/** the concerned key */
		key_type key = DefaultKeyValue<key_type>::value;
		/** some special key modifiers like shift */
		KeyModifier modifiers = KeyModifier::None;
		/** pressed or release */
		KeyAction action = KeyAction::Press;
	};

#endif

}; // namespace chaos