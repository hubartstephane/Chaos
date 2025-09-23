namespace chaos
{
#ifdef CHAOS_FORWARD_DECLARATION

	class KeyRequest;

#elif !defined CHAOS_TEMPLATE_IMPLEMENTATION

	/**
	* KeyRequest: a specialization of InputRequestBase for a single key
	*/

	class KeyRequest : public InputRequestBase
	{

	public:

		/** default constructor */
		KeyRequest() = default;
		/** copy constructor */
		KeyRequest(KeyRequest const & src) = default;
		/** constructor with initialization */
		KeyRequest(Key const & in_key, KeyActionMask in_action_mask = KeyActionMask::Press):
			key(in_key),
			action_mask(in_action_mask)
		{}

		/** gets an equivalent request by updating required_modifiers */
		KeyRequest RequireModifiers(KeyModifier in_modifiers) const;
		/** gets an equivalent request by updating forbidden_modifiers */
		KeyRequest ForbidModifiers(KeyModifier in_modifiers) const;

		/** check against a KeyEvent. No other consideration */
		bool CheckAgainst(KeyEventBase const& in_key_event);

		/** override */
		virtual bool Check(KeyEventBase const& in_key_event, InputDeviceInterface const* in_input_device, InputConsumptionCache & in_consumption_cache) const override;
		/** override */
		virtual bool Check(InputDeviceInterface const* in_input_device, InputConsumptionCache & in_consumption_cache) const override;

	public:

		/** the concerned key */
		Key key;
		/** the required state of the key */
		KeyActionMask action_mask = KeyActionMask::Press;
		/** some mandatory modifiers */
		KeyModifier required_modifiers = KeyModifier::None;
		/** some forbidden modifiers */
		KeyModifier forbidden_modifiers = KeyModifier::None;
	};

	/**
	* Some request getters
	**/

	CHAOS_API KeyRequest RequestKeyDown(Key key);

	CHAOS_API KeyRequest RequestKeyPressed(Key key);

	CHAOS_API KeyRequest RequestKeyRepeat(Key key);

	CHAOS_API KeyRequest RequestKeyReleased(Key key);

#endif

}; // namespace chaos