namespace chaos
{
#ifdef CHAOS_FORWARD_DECLARATION

	class GamepadState;

#elif !defined CHAOS_TEMPLATE_IMPLEMENTATION

	/**
	* GamepadState: the gamepad buttons and axes states
	*/

	class CHAOS_API GamepadState : public InputDeviceUserInterface
	{
		/** number of axes in a gamepad */
		static constexpr size_t AXIS_COUNT = sizeof(meta::FakeInstance<GLFWgamepadstate>().axes) / sizeof(meta::FakeInstance<GLFWgamepadstate>().axes[0]);
		/** number of buttons in a gamepad (beware this does not take TRIGGER_LEFT/TRIGGER_RIGHT virtual buttons into account) */
		static constexpr size_t BUTTON_COUNT = sizeof(meta::FakeInstance<GLFWgamepadstate>().buttons) / sizeof(meta::FakeInstance<GLFWgamepadstate>().buttons[0]);
		/** number of sticks in gamepad */
		static constexpr size_t STICK_COUNT = 2; // see GamepadStick

	public:

		/** override */
		virtual KeyState const * GetKeyState(Key key) const override;
		/** override */
		virtual AxisState const * GetAxisState(GamepadAxis axis) const override;
		/** override */
		virtual StickState const * GetStickState(GamepadStick stick) const override;

		/** override */
		virtual bool ForAllKeys(LightweightFunction<bool(Key, KeyState const &)> func) const override;
		/** override */
		virtual bool ForAllAxes(LightweightFunction<bool(GamepadAxis, AxisState const &)> func) const override;
		/** override */
		virtual bool ForAllSticks(LightweightFunction<bool(GamepadStick, StickState const &)> func) const override;

		/** returns the number of buttons */
		size_t GetButtonCount() const;
		/** returns the number of axis */
		size_t GetAxisCount() const;

		/** update all the values for the axis and buttons */
		void UpdateAxisAndButtons(int stick_index, float dead_zone);
		/** reset the content of the object */
		void Clear();

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