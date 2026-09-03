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

		/** constructor */
		InputReceiverTraverser(InputDeviceInterface const* in_input_device):
			input_device(in_input_device){}

		/** destructor */
		virtual ~InputReceiverTraverser() = default;

		/** enter an InputEventReceiver */
		virtual bool Traverse(InputReceiverInterface * in_input_receiver);

		/** get the input device */
		InputDeviceInterface const* GetInputDevice() const;

	protected:

		/** process the InputEventReceiver */
		virtual bool Process(InputReceiverInterface * in_input_receiver);

	protected:

		/** the initial InputDevice */
		InputDeviceInterface const* input_device = nullptr;
	};

#endif

}; // namespace chaos