namespace chaos
{
#ifdef CHAOS_FORWARD_DECLARATION

	class KeyEventBase;

#elif !defined CHAOS_TEMPLATE_IMPLEMENTATION

	/**
	* KeyEventBase: a generic key event
	*/

	class KeyEventBase
	{

	public:

		/** check whether this is a key press or repeat event */
		bool IsKeyDownEvent() const;
		/** check whether this is a key press event */
		bool IsKeyPressedEvent() const;
		/** check whether this is a key repeat event */
		bool IsKeyRepeatEvent() const;
		/** check whether this is a key release event */
		bool IsKeyReleasedEvent() const;

		/** generic check function */
		bool Check(Key in_key, KeyActionMask in_action_mask, KeyModifier in_required_modifiers = KeyModifier::None, KeyModifier in_forbidden_modifiers = KeyModifier::None) const;

		/** check whether this is a key press event */
		bool IsKeyPressed(Key in_key, KeyModifier in_required_modifiers = KeyModifier::None, KeyModifier in_forbidden_modifiers = KeyModifier::None) const;
		/** check whether this is a key release event */
		bool IsKeyReleased(Key in_key, KeyModifier in_required_modifiers = KeyModifier::None, KeyModifier in_forbidden_modifiers = KeyModifier::None) const;
		/** check whether this is a key repeat event */
		bool IsKeyRepeat(Key in_key, KeyModifier in_required_modifiers = KeyModifier::None, KeyModifier in_forbidden_modifiers = KeyModifier::None) const;
		/** check whether this is a key press or repeat event */
		bool IsKeyDown(Key in_key, KeyModifier in_required_modifiers = KeyModifier::None, KeyModifier in_forbidden_modifiers = KeyModifier::None) const;

	public:

		/** the concerned key */
		Key key;
		/** some special key modifiers like shift */
		KeyModifier modifiers = KeyModifier::None;
		/** pressed or release */
		KeyAction action = KeyAction::Press;
	};

#endif

}; // namespace chaos