namespace chaos
{
#ifdef CHAOS_FORWARD_DECLARATION

	class OnKeyEventInputEventReceiverHierarchyTraverser;

#elif !defined CHAOS_TEMPLATE_IMPLEMENTATION

	/**
	 * OnKeyEventInputEventReceiverHierarchyTraverser: a traverser specialization dedicated to handling KeyEvents
	 */

	class CHAOS_API OnKeyEventInputEventReceiverHierarchyTraverser : public InputEventReceiverHierarchyTraverser
	{
	public:

		/** constructor */
		OnKeyEventInputEventReceiverHierarchyTraverser(KeyEventBase const& in_key_event, InputDeviceInterface const* in_input_device, InputConsumptionCache* in_consumption_cache);

		/** override */
		virtual bool Process(InputEventReceiverInterface* in_event_receiver) override;
		/** override */
		virtual bool Traverse(InputEventReceiverInterface* in_event_receiver) override;

	protected:

		/** the key event being handled */
		KeyEventBase const& key_event;
		/** the input device used during the traversal */
		InputDeviceInterface const* input_device = nullptr;
		/** the consumption cache to avoid same input to be handled twice during the traversal */
		InputConsumptionCache* consumption_cache = nullptr;
	};

#endif

}; // namespace chaos