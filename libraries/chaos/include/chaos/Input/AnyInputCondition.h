namespace chaos
{
#ifdef CHAOS_FORWARD_DECLARATION

	class AnyInputCondition;

#elif !defined CHAOS_TEMPLATE_IMPLEMENTATION

	/**
	* AnyInputCondition: a request that consume any input
	*/

	class AnyInputCondition : public InputConditionBase
	{

	public:

		/** override */
		virtual InputConditionResult Check(InputReceiverInterface const* in_input_receiver, InputDeviceInterface const* in_input_device, InputConsumptionCache & in_consumption_cache) const override;
		/** override */
		virtual bool IsRequestRelatedTo(Key in_input) const override;
		/** override */
		virtual bool IsRequestRelatedTo(Input1D in_input) const override;
		/** override */
		virtual bool IsRequestRelatedTo(Input2D in_input) const override;
		/** override */
		virtual char const * GetDebugInfo(InputRequestDebugInfoStorage& debug_info_storage) const override;
	};

#endif

}; // namespace chaos