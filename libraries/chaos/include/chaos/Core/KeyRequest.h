namespace chaos
{
#ifdef CHAOS_FORWARD_DECLARATION

	class KeyRequest;

#elif !defined CHAOS_TEMPLATE_IMPLEMENTATION

	/**
	* KeyRequest: a request for key event
	*/

	class KeyRequest
	{
	public:

		/** the concerned key */
		Key key;
		/** some mandatory modifiers */
		KeyModifier required_modifiers = KeyModifier::None;
		/** some forbidden modifiers */
		KeyModifier forbidden_modifiers = KeyModifier::None;
		/** the required state of the key */
		KeyActionMask action_mask = KeyActionMask::Press;
	};

#endif

}; // namespace chaos