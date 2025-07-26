namespace chaos
{
#ifdef CHAOS_FORWARD_DECLARATION

	class KeyRequestBase;
	class KeyRequest;

#elif !defined CHAOS_TEMPLATE_IMPLEMENTATION

	/**
	* KeyRequestBase: a request for key event
	*/

	class KeyRequestBase
	{
	public:

		/** check whether the request matches a given key event */
		virtual bool MatchEvent(KeyEventBase const & key_event) const
		{
			return false;
		}
	};

	/**
	* KeyRequest: a specialization of KeyRequestBase for a single key
	*/

	class KeyRequest : public KeyRequestBase
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

		/** override */
		virtual bool MatchEvent(KeyEventBase const & key_event) const override;

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

	CHAOS_API KeyRequest IsKeyDown(Key key);

	CHAOS_API KeyRequest IsKeyPressed(Key key);

	CHAOS_API KeyRequest IsKeyRepeat(Key key);

	CHAOS_API KeyRequest IsKeyReleased(Key key);

#endif

}; // namespace chaos