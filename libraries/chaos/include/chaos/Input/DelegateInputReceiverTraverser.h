namespace chaos
{
#ifdef CHAOS_FORWARD_DECLARATION

	class DelegateInputReceiverTraverser;

#elif !defined CHAOS_TEMPLATE_IMPLEMENTATION

	using DelegateInputReceiverTraverserFunction = LightweightFunction<bool(InputReceiverInterface*, InputDeviceInterface const*)>;

	/**
	* DelegateInputReceiverTraverser: a specialisation that makes possible to use a delegate rather to have to override Process method
	*/

	class CHAOS_API DelegateInputReceiverTraverser: public InputReceiverTraverser
	{
	public:

		/** constructor */
		DelegateInputReceiverTraverser(DelegateInputReceiverTraverserFunction in_process_function);

		/** override */
		virtual bool Process(InputReceiverInterface * in_input_receiver, InputDeviceInterface const* in_input_device) override;

	protected:

		/** the functor to be called */
		DelegateInputReceiverTraverserFunction process_function;
	};

#endif

}; // namespace chaos