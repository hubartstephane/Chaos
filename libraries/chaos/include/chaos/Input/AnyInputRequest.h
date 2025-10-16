namespace chaos
{
#ifdef CHAOS_FORWARD_DECLARATION

	class KeyRequest;

#elif !defined CHAOS_TEMPLATE_IMPLEMENTATION

	/**
	* AnyInputRequest: a request that consume any input
	*/

	class AnyInputRequest : public InputRequestBase
	{

	public:

		/** override */
		virtual InputRequestDebugInfo GetDebugInfo() const override;
		/** override */
		virtual InputRequestResult Check(InputReceiverInterface const* in_input_receiver, KeyEventBase const& in_key_event, InputDeviceInterface const* in_input_device, InputConsumptionCache & in_consumption_cache) const override;
		/** override */
		virtual InputRequestResult Check(InputReceiverInterface const* in_input_receiver, InputDeviceInterface const* in_input_device, InputConsumptionCache & in_consumption_cache) const override;
	};

#endif

}; // namespace chaos