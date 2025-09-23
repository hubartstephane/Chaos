namespace chaos
{
#ifdef CHAOS_FORWARD_DECLARATION

	class OnPollInputEventReceiverHierarchyTraverser;

#elif !defined CHAOS_TEMPLATE_IMPLEMENTATION

	/**
	 * OnPollInputEventReceiverHierarchyTraverser: a traverser specialization dedicated to input polling
	 */

	class CHAOS_API OnPollInputEventReceiverHierarchyTraverser : public InputEventReceiverHierarchyTraverser
	{
	public:

		/** constructor */
		OnPollInputEventReceiverHierarchyTraverser(InputDeviceInterface const* in_input_device, InputConsumptionCache* in_consumption_cache);

		/** override */
		virtual bool Process(InputEventReceiverInterface* in_event_receiver) override;
		/** override */
		virtual bool Traverse(InputEventReceiverInterface* in_event_receiver) override;

	protected:

		/** the input device used during the traversal */
		InputDeviceInterface const* input_device = nullptr;
		/** the consumption cache to avoid same input to be handled twice during the traversal */
		InputConsumptionCache* consumption_cache = nullptr;
	};


#endif

}; // namespace chaos