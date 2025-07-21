namespace chaos
{
#ifdef CHAOS_FORWARD_DECLARATION

	class InputDeviceUserInterface;

#elif !defined CHAOS_TEMPLATE_IMPLEMENTATION

	/**
	* InputDeviceUserInterface: a class to make request for some device state
	*/

	class CHAOS_API InputDeviceUserInterface
	{

	public:

		/** gets one button state */
		virtual ButtonState const * GetButtonState(GamepadButton button) const;
		/** gets one axis state */
		virtual AxisState const * GetAxisState(GamepadAxis axis) const;
		/** gets one stick state */
		virtual StickState const * GetStickState(GamepadStick stick) const;

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


		#if 0
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
#endif

	};

#endif

}; // namespace chaos