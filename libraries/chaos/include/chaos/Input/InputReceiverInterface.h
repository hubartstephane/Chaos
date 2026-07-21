namespace chaos
{
#ifdef CHAOS_FORWARD_DECLARATION

	class InputReceiverInterface;

	enum class EnumerateInputActionContext;

#elif !defined CHAOS_TEMPLATE_IMPLEMENTATION

	/**
	* Some aliases
	*/

	using InvokeWithUpgradedInputDeviceFunction = LightweightFunction<bool(InputDeviceInterface const *)>;

	/**
	* EnumerateInputActionContext: indicates when action processing occurs
	*/

	enum class EnumerateInputActionContext : int
	{
		OnEvent,     // direct event (key down/up)
		OnPolling,   // per-tick polling
		OnQuery      // read-only inspection, no side-effects
	};

	/**
	* InputReceiverInterface: an interface for objects that can handle inputs
	*/

	class CHAOS_API InputReceiverInterface
	{

	public:
	
		/** destructor */
		virtual ~InputReceiverInterface() = default;

		/** get a name for the receiver */
		virtual char const* GetInputReceiverName() const;

		/** change the current input mode */
		void SetInputMode(InputMode new_mode);
		/* get the current input mode */
		InputMode GetInputMode() const { return input_mode; }

		/** check whether a key is down. Change input mode according to success */
		bool CheckKeyDown(Key key);
		/** check whether a key in a set is down */
		bool CheckKeyDown(Key const* keys);

		/** called whenever the mouse is moved */
		bool OnMouseMove(glm::vec2 const & delta);
		/** called whenever the mouse button is down / up */
		bool OnMouseButton(MouseButtonEvent const &mouse_button_event);
		/** called whenever the mouse wheel is changed */
		bool OnMouseWheel(double scroll_x, double scroll_y);
		/** called whenever a key is pressed */
		bool OnKeyEvent(KeyEvent const& key_event);
		/** called whenever a char is generated */
		bool OnCharEvent(unsigned int c);

		/** traverse all receivers hierarchy and call a functor on all elements */
		virtual bool TraverseInputReceiver(InputReceiverTraverser & in_traverser, InputDeviceInterface const* in_input_device = KeyboardAndMouseDevice::GetInstance());
		/** enumerate some declared key actions */
		virtual bool EnumerateInputActions(InputActionProcessor & in_action_processor, EnumerateInputActionContext in_context);
		/** upgrade the input device and call some functor */
		virtual bool InvokeWithUpgradedInputDevice(InputDeviceInterface const * in_input_device, InvokeWithUpgradedInputDeviceFunction in_func);

	protected:

		/** called whenever the mouse is moved */
		virtual bool OnMouseMoveImpl(glm::vec2 const& delta);
		/** called whenever the mouse button is down / up */
		virtual bool OnMouseButtonImpl(MouseButtonEvent const &mouse_button_event);
		/** called whenever the mouse wheel is changed */
		virtual bool OnMouseWheelImpl(double scroll_x, double scroll_y);
		/** called whenever a key is pressed */
		virtual bool OnKeyEventImpl(KeyEvent const& key_event);
		/** called whenever a char is generated */
		virtual bool OnCharEventImpl(unsigned int c);

		/** the user callback called when current input mode changes */
		virtual void OnInputModeChanged(InputMode new_mode, InputMode old_mode);

		/** handle some event action */
		template<InputType INPUT_TYPE>
		bool ProcessInputEvent(INPUT_TYPE in_input);

		/** mark the input as consumed inside the WindowApplication ConsumCache */
		template<InputType INPUT_TYPE>
		static void MarkInputConsumedInApplicationCache(INPUT_TYPE in_input, InputDeviceInterface const* in_input_device = KeyboardAndMouseDevice::GetInstance());

		/** internal method to check whether a button is pressed */
		virtual bool DoCheckKeyDown(Key key) const;

	protected:

		/** the current input mode */
		InputMode input_mode = InputMode::Keyboard;
	};

#else // defined CHAOS_TEMPLATE_IMPLEMENTATION

	template<InputType INPUT_TYPE>
	bool InputReceiverInterface::ProcessInputEvent(INPUT_TYPE in_input)
	{
		KeyboardAndMouseDevice * keyboard_and_mouse_device = KeyboardAndMouseDevice::GetInstance();

		return InvokeWithUpgradedInputDevice(keyboard_and_mouse_device, [this, in_input](InputDeviceInterface const* in_input_device)
		{
			// XXX: do not use WindowApplication::consumption_cache on purpose
			//
			//      the traversal with InputConsumptionCache consumes a lot of inputs and we only want to consume the ones relative with incomming event
			//      elsewhere we would end up locking inputs for the Poll step to come later in the timeline
			//
			//      that's why we are using a temporary InputConsumptionCache and manually lock inputs in application InputConsumptionCache
			//      only if the inputs are consummed for real

			InputConsumptionCache consumption_cache;

			OnEventInputActionProcessor<INPUT_TYPE> action_processor(this, in_input_device, in_input, &consumption_cache);
			if (EnumerateInputActions(action_processor, EnumerateInputActionContext::OnEvent))
			{
				MarkInputConsumedInApplicationCache(in_input, in_input_device);
				return true;
			}
			return false;
		});
	}

	template<InputType INPUT_TYPE>
	void InputReceiverInterface::MarkInputConsumedInApplicationCache(INPUT_TYPE in_input, InputDeviceInterface const* in_input_device)
	{
		if (in_input_device != nullptr)
			if (WindowApplication* window_application = Application::GetInstance())
				window_application->GetInputConsumptionCache().QueryInputState(in_input, nullptr, in_input_device); // this consume the input until next frame
	}

#endif

}; // namespace chaos