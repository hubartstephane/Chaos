namespace chaos
{
#ifdef CHAOS_FORWARD_DECLARATION

	class InputEventReceiverInterface;

	enum class EnumerateInputActionContext;

#elif !defined CHAOS_TEMPLATE_IMPLEMENTATION

	/**
	* Some aliases
	*/

	using InvokeWithUpgradedInputDeviceFunction = LightweightFunction<bool(InputDeviceInterface const *)>;

	/**
	* EnumerateInputActionContext: indicates when action processing occurs
	*/

	enum class CHAOS_API EnumerateInputActionContext : int
	{
		OnEvent,     // direct event (key down/up)
		OnPolling,   // per-tick polling
		OnQuery      // read-only inspection, no side-effects
	};

	/**
	* InputEventReceiverInterface: an interface for objects that can handle inputs
	*/

	class CHAOS_API InputEventReceiverInterface
	{

	public:
	
		/** destructor */
		virtual ~InputEventReceiverInterface() = default;

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

		/** process an incomming event using the known KeyActions */
		bool ProcessKeyActions(KeyEventBase const & key_event);








		/** traverse all receivers hierarchy and call a functor on all elements */
		virtual bool TraverseInputEventReceiverHierarchy(InputEventReceiverHierarchyTraverser & in_traverser);

		/** enumerate some declared key actions */
		virtual bool EnumerateInputActions(InputActionEnumerator & in_action_enumerator, EnumerateInputActionContext in_context);

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

		/** internal method to check whether a button is pressed */
		virtual bool DoCheckKeyDown(Key key) const;

	protected:

		/** the current input mode */
		InputMode input_mode = InputMode::KEYBOARD;
	};

#endif

}; // namespace chaos