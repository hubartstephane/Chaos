namespace chaos
{
#ifdef CHAOS_FORWARD_DECLARATION

	class OnKeyEventInputReceiverTraverser;

#elif !defined CHAOS_TEMPLATE_IMPLEMENTATION

	/**
	 * OnKeyEventInputReceiverTraverser: a traverser specialization dedicated to handling KeyEvents
	 */

	class CHAOS_API OnKeyEventInputReceiverTraverser : public InputReceiverTraverser
	{
	public:

		/** constructor */
		OnKeyEventInputReceiverTraverser(KeyEventBase const& in_key_event, InputDeviceInterface const* in_input_device, InputConsumptionCache* in_consumption_cache);

		/** override */
		virtual bool Process(InputReceiverInterface* in_input_receiver) override;
		/** override */
		virtual bool Traverse(InputReceiverInterface* in_input_receiver) override;

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