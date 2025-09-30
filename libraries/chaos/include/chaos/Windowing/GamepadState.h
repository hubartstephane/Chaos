namespace chaos
{
#ifdef CHAOS_FORWARD_DECLARATION

	class GamepadState;

#elif !defined CHAOS_TEMPLATE_IMPLEMENTATION

	/**
	* GamepadState: the gamepad buttons and axes states
	*/

	class CHAOS_API GamepadState : public InputDeviceInterface
	{
		/** number of axes in a gamepad */
		static constexpr size_t AXIS_COUNT = sizeof(meta::FakeInstance<GLFWgamepadstate>().axes) / sizeof(meta::FakeInstance<GLFWgamepadstate>().axes[0]);
		/** number of buttons in a gamepad (beware this does not take TRIGGER_LEFT/TRIGGER_RIGHT virtual buttons into account) */
		static constexpr size_t BUTTON_COUNT = sizeof(meta::FakeInstance<GLFWgamepadstate>().buttons) / sizeof(meta::FakeInstance<GLFWgamepadstate>().buttons[0]);
		/** number of sticks in gamepad */
		static constexpr size_t STICK_COUNT = 2; // see GamepadStick

	public:

		/** returns the number of buttons */
		size_t GetButtonCount() const;
		/** returns the number of axis */
		size_t GetAxisCount() const;

		/** update all the values for the axis and buttons */
		void UpdateAxisAndButtons(int stick_index, float dead_zone);
		/** reset the content of the object */
		void Clear();

	protected:

		/** override */
		virtual KeyState const * DoGetInputState(Key key) const override;
		/** override */
		virtual AxisState const * DoGetInputState(GamepadAxis axis) const override;
		/** override */
		virtual StickState const * DoGetInputState(GamepadStick stick) const override;

		/** override */
		virtual bool DoForAllKeys(ForAllKeysFunction func) const override;
		/** override */
		virtual bool DoForAllAxes(ForAllAxesFunction func) const override;
		/** override */
		virtual bool DoForAllSticks(ForAllSticksFunction func) const override;

	protected:

		/** the state for axis */
		std::array<AxisState, AXIS_COUNT> axes;
		/** the state for buttons */
		std::array<KeyState, BUTTON_COUNT + 2> buttons; // +2 for LEFT_TRIGGER/RIGHT_TRIGGER virtual buttons
		/** the state for sticks */
		std::array<StickState, STICK_COUNT> sticks;
	};

#endif

}; // namespace chaos