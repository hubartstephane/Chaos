namespace chaos
{
#ifdef CHAOS_FORWARD_DECLARATION

	class InputEventReceiverHierarchyTraverser;

#elif !defined CHAOS_TEMPLATE_IMPLEMENTATION

	/**
	* InputEventReceiverHierarchyTraverser: a class that is used to traverse an InputEventReceiverHierarchy
	*/

	class CHAOS_API InputEventReceiverHierarchyTraverser
	{
	public:

		/** destructor */
		virtual ~InputEventReceiverHierarchyTraverser() = default;

		/** enter an InputEventReceiver */
		virtual bool Traverse(InputEventReceiverInterface * in_event_receiver);
		/** process the InputEventReceiver */
		virtual bool Process(InputEventReceiverInterface * in_event_receiver);
	};

#endif

}; // namespace chaos