namespace chaos
{
#ifdef CHAOS_FORWARD_DECLARATION

	class MouseButtonEvent;

#elif !defined CHAOS_TEMPLATE_IMPLEMENTATION

	/**
	* MouseButtonEvent: a mouse button event
	*/

	class CHAOS_API MouseButtonEvent
	{
	public:

		/** check whether this is a mouse button press event */
		bool IsButtonPressedEvent() const;
		/** check whether this is a mouse button repeat event */
		bool IsButtonRepeatEvent() const;
		/** check whether this is a mouse button release event */
		bool IsButtonReleasedEvent() const;
		/** check whether this is a mouse button press or repeat event */
		bool IsButtonDownEvent() const;

		/** check whether the mouse button event is for considered button */
		bool IsButtonEvent(int in_button, KeyModifier in_required_modifiers = KeyModifier::None, KeyModifier in_forbidden_modifiers = KeyModifier::None) const;

		/** check whether this is a mouse button press or repeat event */
		bool IsButtonDown(int in_button, KeyModifier in_required_modifiers = KeyModifier::None, KeyModifier in_forbidden_modifiers = KeyModifier::None) const;
		/** check whether this is a mouse button press event */
		bool IsButtonPressed(int in_button, KeyModifier in_required_modifiers = KeyModifier::None, KeyModifier in_forbidden_modifiers = KeyModifier::None) const;
		/** check whether this is a mouse button release event */
		bool IsButtonReleased(int in_button, KeyModifier in_required_modifiers = KeyModifier::None, KeyModifier in_forbidden_modifiers = KeyModifier::None) const;
		/** check whether this is a mouse button repeat event */
		bool IsButtonRepeat(int in_button, KeyModifier in_required_modifiers = KeyModifier::None, KeyModifier in_forbidden_modifiers = KeyModifier::None) const;

	public:

		/** the mouse button index */
		int button = 0;
		/** pressed or release */
		int action = 0;
		/** some special key modifiers like shift */
		KeyModifier key_modifiers = KeyModifier::None;
	};

#endif

}; // namespace chaos