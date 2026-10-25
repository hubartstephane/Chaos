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
		virtual InputConditionResult Check(InputConditionCheckParams const& in_params) const override;
		/** override */
		virtual bool IsRequestRelatedTo(Key in_input) const override;
		/** override */
		virtual bool IsRequestRelatedTo(Input1D in_input) const override;
		/** override */
		virtual bool IsRequestRelatedTo(Input2D in_input) const override;
		/** override */
		virtual char const * GetDebugInfo(InputConditionDebugInfoStorage& debug_info_storage) const override;
	};

#endif

}; // namespace chaos