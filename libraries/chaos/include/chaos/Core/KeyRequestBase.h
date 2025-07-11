namespace chaos
{
#ifdef CHAOS_FORWARD_DECLARATION

	template<typename T>
	class KeyRequestBase;

#elif !defined CHAOS_TEMPLATE_IMPLEMENTATION

	/**
	* KeyRequestBase: a request for key event
	*/

	template<typename T>
	class KeyRequestBase
	{
		using key_type = T;

	public:

		/** the concerned key */
		key_type key = DefaultKeyValue<key_type>::value;
		/** the required state of the key */
		KeyActionMask action_mask = KeyActionMask::None;
		/** some mandatory modifiers */
		KeyModifier required_modifiers = KeyModifier::None;
		/** some forbidden modifiers */
		KeyModifier forbidden_modifiers = KeyModifier::None;
	};

#endif

}; // namespace chaos