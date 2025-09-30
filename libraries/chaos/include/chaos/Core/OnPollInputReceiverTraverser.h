namespace chaos
{
#ifdef CHAOS_FORWARD_DECLARATION

	class OnPollInputReceiverTraverser;

#elif !defined CHAOS_TEMPLATE_IMPLEMENTATION

	/**
	 * OnPollInputReceiverTraverser: a traverser specialization dedicated to input polling
	 */

	class CHAOS_API OnPollInputReceiverTraverser : public InputReceiverTraverser
	{
	public:

		/** constructor */
		OnPollInputReceiverTraverser(InputDeviceInterface const* in_input_device, InputConsumptionCache* in_consumption_cache);

		/** override */
		virtual bool Process(InputReceiverInterface* in_input_receiver) override;
		/** override */
		virtual bool Traverse(InputReceiverInterface* in_input_receiver) override;

	protected:

		/** the input device used during the traversal */
		InputDeviceInterface const* input_device = nullptr;
		/** the consumption cache to avoid same input to be handled twice during the traversal */
		InputConsumptionCache* consumption_cache = nullptr;
	};


#endif

}; // namespace chaos