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
		virtual InputRequestResult Check(InputReceiverInterface const* in_input_receiver, InputDeviceInterface const* in_input_device, InputConsumptionCache & in_consumption_cache) const override;
		/** override */
		virtual bool IsRequestRelatedTo(Key in_input) const override;
		/** override */
		virtual bool IsRequestRelatedTo(Input1D in_input) const override;
		/** override */
		virtual bool IsRequestRelatedTo(Input2D in_input) const override;
		/** override */
		virtual InputRequestDebugInfo GetDebugInfo() const override;
	};

#endif

}; // namespace chaos