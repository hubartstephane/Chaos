namespace chaos
{
#ifdef CHAOS_FORWARD_DECLARATION

	class InputReceiverTraverser;

#elif !defined CHAOS_TEMPLATE_IMPLEMENTATION

	/**
	* InputReceiverTraverser: a class that is used to traverse an InputEventReceiverHierarchy
	*/

	class CHAOS_API InputReceiverTraverser
	{
	public:

		/** destructor */
		virtual ~InputReceiverTraverser() = default;

		/** enter an InputEventReceiver */
		virtual bool Traverse(InputReceiverInterface * in_input_receiver);
		/** process the InputEventReceiver */
		virtual bool Process(InputReceiverInterface * in_input_receiver);
	};

#endif

}; // namespace chaos