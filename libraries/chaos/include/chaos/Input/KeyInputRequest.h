namespace chaos
{
#ifdef CHAOS_FORWARD_DECLARATION

	class KeyInputRequest;

#elif !defined CHAOS_TEMPLATE_IMPLEMENTATION

	/**
	* KeyInputRequest: a specialization of InputRequestBase for a single key
	*/

	class CHAOS_API KeyInputRequest : public InputRequestBase
	{

	public:

		using InputRequestBase::IsRequestRelatedTo; // mandatory elsewhere IsRequestRelatedTo(input1D/2D) will be hidden by the single override method

		/** default constructor */
		KeyInputRequest() = default;
		/** copy constructor */
		KeyInputRequest(KeyInputRequest const & src) = default;
		/** constructor with initialization */
		KeyInputRequest(Key const & in_key, KeyActionMask in_action_mask = KeyActionMask::Press):
			key(in_key),
			action_mask(in_action_mask)
		{}

		/** override */
		virtual InputRequestResult Check(InputReceiverInterface const* in_input_receiver, InputDeviceInterface const* in_input_device, InputConsumptionCache & in_consumption_cache) const override;
		/** override */
		virtual bool IsRequestRelatedTo(Key in_input) const override;

		/** override */
		virtual InputRequestDebugInfo GetDebugInfo() const override;

	public:

		/** the concerned key */
		Key key;
		/** the required state of the key */
		KeyActionMask action_mask = KeyActionMask::Press;
	};

	/**
	* Some request getters
	**/

	CHAOS_API KeyInputRequest KeyDown(Key key);

	CHAOS_API KeyInputRequest KeyPressed(Key key);

	CHAOS_API KeyInputRequest KeyRepeat(Key key);

	CHAOS_API KeyInputRequest KeyReleased(Key key);

#endif

}; // namespace chaos