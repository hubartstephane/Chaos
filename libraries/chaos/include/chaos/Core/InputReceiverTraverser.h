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
		friend class InputReceiverInterface;

	public:

		/** destructor */
		virtual ~InputReceiverTraverser() = default;

		/** enter an InputEventReceiver */
		virtual bool Traverse(InputReceiverInterface * in_input_receiver, InputDeviceInterface const* in_input_device = KeyboardAndMouseState::GetInstance());

	protected:

		/** process the InputEventReceiver */
		virtual bool Process(InputReceiverInterface * in_input_receiver, InputDeviceInterface const* in_input_device);
	};

#endif

}; // namespace chaos