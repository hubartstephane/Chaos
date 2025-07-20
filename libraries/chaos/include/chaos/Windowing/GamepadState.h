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

		/** returns true whether there is any actioned button */
		bool IsAnyButtonAction() const;
		/** returns true whether there is any axis in use */
		bool IsAnyAxisAction() const;
		/** returns true whenever a buttons is pressed or an axis is in action */
		bool IsAnyAction() const;

		/** returns true whether there is a button that just has become pressed */
		bool IsAnyButtonJustPressed() const;





#if 0


		/* returns a status giving the change of button relative to previous frame */
		ButtonStateChange GetButtonStateChange(GamepadButton button) const;
		/** returns the button state */
		bool IsButtonPressed(GamepadButton button) const;
		/** returns the button state */
		float GetAxisValue(GamepadAxis axis) const;

		/** returns true whether there is any pressed button */
		bool IsAnyButtonPressed() const;
		/** returns true whether there is any axis in use */
		bool IsAnyAxisAction() const;
		/** returns true whenever a buttons is pressed or an axis is in action */
		bool IsAnyAction() const;


		/** returns the direction of one stick (a combinaison of 2 axis) */
		glm::vec2 GetStickValue(GamepadStick stick) const;

#endif




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