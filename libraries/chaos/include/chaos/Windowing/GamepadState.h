namespace chaos
{
#ifdef CHAOS_FORWARD_DECLARATION

	class GamepadState;

#elif !defined CHAOS_TEMPLATE_IMPLEMENTATION

	/**
	* GamepadState: the gamepad buttons and axes states
	*/

	class CHAOS_API GamepadState
	{
		/** number of axis in a gamepad */
		static constexpr size_t AXIS_COUNT = sizeof(meta::FakeInstance<GLFWgamepadstate>().axes) / sizeof(meta::FakeInstance<GLFWgamepadstate>().axes[0]);
		/** number of button in a gamepad */
		static constexpr size_t BUTTON_COUNT = sizeof(meta::FakeInstance<GLFWgamepadstate>().buttons) / sizeof(meta::FakeInstance<GLFWgamepadstate>().buttons[0]);

	public:

		/** gets one button state */
		ButtonState GetButtonState(GamepadButton button) const;
		/** gets one axis state */
		AxisState GetAxisState(GamepadAxis axis) const;
		/** gets one stick state */
		StickState GetStickState(GamepadStick stick) const;

		/** get the button value */
		bool GetButtonValue(GamepadButton button) const;
		/** get the button state change */
		ButtonStatus GetButtonStatus(GamepadButton button) const;

		/** whether the button is up (press or repeat) */
		bool IsButtonDown(GamepadButton button) const;
		/** whether the button is up (released) */
		bool IsButtonUp(GamepadButton button) const;
		/** whether the button has just been pressed */
		bool IsButtonJustPressed(GamepadButton button) const;
		/** whether the button has just been released */
		bool IsButtonJustReleased(GamepadButton button) const;

		/** get the axis value */
		float GetAxisValue(GamepadAxis axis) const;

		/** get the stick value */
		glm::vec2 GetStickValue(GamepadStick stick) const;

		/** returns true whether there is any actioned button */
		bool IsAnyButtonAction() const;
		/** returns true whether there is any axis in use */
		bool IsAnyAxisAction() const;
		/** returns true whenever a buttons is pressed or an axis is in action */
		bool IsAnyAction() const;

		/** returns true whether there is a button that just has become pressed */
		bool IsAnyButtonJustPressed() const;

		/** returns the number of buttons */
		size_t GetButtonCount() const;
		/** returns the number of axis */
		size_t GetAxisCount() const;

		/** update all the values for the axis and buttons */
		void UpdateAxisAndButtons(int stick_index, float dead_zone);
		/** reset the content of the object */
		void Clear();

	protected:

		/** the value for axis */
		std::array<AxisState, AXIS_COUNT> axes;
		/** the value for buttons */
		std::array<ButtonState, BUTTON_COUNT> buttons;
	};

#endif

}; // namespace chaos