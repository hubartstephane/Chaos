namespace chaos
{
#ifdef CHAOS_FORWARD_DECLARATION

	class DelegateInputEventReceiverHierarchyTraverser;

#elif !defined CHAOS_TEMPLATE_IMPLEMENTATION

	using DelegateInputEventReceiverHierarchyTraverserFunction = LightweightFunction<bool(InputEventReceiverInterface*)>;

	/**
	* DelegateInputEventReceiverHierarchyTraverser: a specialisation that makes possible to use a delegate rather to have to override Process method
	*/

	class CHAOS_API DelegateInputEventReceiverHierarchyTraverser: public InputEventReceiverHierarchyTraverser
	{
	public:

		/** constructor */
		DelegateInputEventReceiverHierarchyTraverser(DelegateInputEventReceiverHierarchyTraverserFunction in_process_function):
			process_function(in_process_function)
			{}

		/** override */
		virtual bool Process(InputEventReceiverInterface * in_event_receiver) override;

	protected:

		/** the functor to be called */
		DelegateInputEventReceiverHierarchyTraverserFunction process_function;
	};

#endif

}; // namespace chaos