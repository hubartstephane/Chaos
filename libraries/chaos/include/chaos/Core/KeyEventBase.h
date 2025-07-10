namespace chaos
{
#ifdef CHAOS_FORWARD_DECLARATION

	template<typename T, T>
	class KeyEventBase;

#elif !defined CHAOS_TEMPLATE_IMPLEMENTATION

	/**
	* KeyEventBase: a generic key event
	*/

	template<typename T, T DefaultKeyValue>
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
		/** check whether this is a key press event */
		bool IsKeyPressed(key_type in_key, KeyModifier in_required_modifiers = KeyModifier::None, KeyModifier in_forbidden_modifiers = KeyModifier::None) const
		{
			if (IsKeyPressedEvent() && IsKeyEvent(in_key, in_required_modifiers, in_forbidden_modifiers))
				return true;
			return false;
		}
		/** check whether this is a key release event */
		bool IsKeyReleased(key_type in_key, KeyModifier in_required_modifiers = KeyModifier::None, KeyModifier in_forbidden_modifiers = KeyModifier::None) const
		{
			if (IsKeyReleasedEvent() && IsKeyEvent(in_key, in_required_modifiers, in_forbidden_modifiers))
				return true;
			return false;
		}
		/** check whether this is a key repeat event */
		bool IsKeyRepeat(key_type in_key, KeyModifier in_required_modifiers = KeyModifier::None, KeyModifier in_forbidden_modifiers = KeyModifier::None) const
		{
			if (IsKeyRepeatEvent() && IsKeyEvent(in_key, in_required_modifiers, in_forbidden_modifiers))
				return true;
			return false;
		}
		/** check whether this is a key press or repeat event */
		bool IsKeyDown(key_type in_key, KeyModifier in_required_modifiers = KeyModifier::None, KeyModifier in_forbidden_modifiers = KeyModifier::None) const
		{
			if (IsKeyDownEvent() && IsKeyEvent(in_key, in_required_modifiers, in_forbidden_modifiers))
				return true;
			return false;
		}

	public:

		/** the concerned key */
		key_type key = DefaultKeyValue;
		/** pressed or release */
		KeyAction action = KeyAction::Press;
		/** some special key modifiers like shift */
		KeyModifier modifiers = KeyModifier::None;
	};

#endif

}; // namespace chaos