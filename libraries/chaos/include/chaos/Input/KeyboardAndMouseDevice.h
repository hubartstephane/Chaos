namespace chaos
{
#ifdef CHAOS_FORWARD_DECLARATION

	class KeyboardAndMouseDevice;

#elif !defined CHAOS_TEMPLATE_IMPLEMENTATION

	/**
	* KeyboardAndMouseDevice: A singleton that contains the state of keyboard and mouse buttons
	*/

	class CHAOS_API KeyboardAndMouseDevice : public Singleton<KeyboardAndMouseDevice>, public InputDeviceInterface
	{

	public:

		/** change the state of a key (notification from a window) */
		void SetInputValue(Key input, bool value);
		/** change the state of a key (notification from a window) */
		void SetInputValue(Input1D input, float value);
		/** change the state of a key (notification from a window) */
		void SetInputValue(Input2D input, glm::vec2 const & value);

		/** some states are accumulators (mouse wheel, delta). Reset them */
		void ResetCumulatedInputs();

	protected:

		/** override */
		virtual std::optional<KeyState> DoGetInputState(Key input) const override;
		/** override */
		virtual std::optional<Input1DState> DoGetInputState(Input1D input) const override;
		/** override */
		virtual std::optional<Input2DState> DoGetInputState(Input2D input) const override;
		/** override */
		virtual bool DoForAllKeys(ForAllKeysFunction func) const override;
		/** override */
		virtual bool DoForAllInput1D(ForAllInput1DFunction func) const override;
		/** override */
		virtual bool DoForAllInput2D(ForAllInput2DFunction func) const override;

	protected:

		/** the state of all keyboard buttons */
		std::array<KeyState, size_t(Key::KEYBOARD_LAST) - size_t(Key::KEYBOARD_FIRST) + 1> keyboard_key_state;
		/** the state of all mouse buttons */
		std::array<KeyState, size_t(Key::MOUSE_LAST) - size_t(Key::MOUSE_FIRST) + 1> mouse_key_state;
		/** the state of all mouse input1D (basically the wheel) */
		std::array<Input1DState, size_t(Input1D::MOUSE_LAST) - size_t(Input1D::MOUSE_FIRST) + 1> mouse_input1D_state;
		/** the state of all mouse input2D (basically the delta position) */
		std::array<Input2DState, size_t(Input2D::MOUSE_LAST) - size_t(Input2D::MOUSE_FIRST) + 1> mouse_input2D_state;
	};

#endif

}; // namespace chaos