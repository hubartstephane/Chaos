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

		/** gets one key state */
		virtual KeyState const * GetKeyState(Key key) const;
		/** gets one axis state */
		virtual AxisState const * GetAxisState(GamepadAxis axis) const;
		/** gets one stick state */
		virtual StickState const * GetStickState(GamepadStick stick) const;

		/** enumerate keys */
		virtual bool ForAllKeys(LightweightFunction<bool(Key, KeyState const &)> func) const;
		/** enumerate axes */
		virtual bool ForAllAxes(LightweightFunction<bool(GamepadAxis, AxisState const &)> func) const;
		/** enumerate sticks */
		virtual bool ForAllSticks(LightweightFunction<bool(GamepadStick, StickState const &)> func) const;

		/** get the key value */
		bool GetKeyValue(Key key) const;
		/** get the key state change */
		KeyStatus GetKeyStatus(Key key) const;

		/** whether the key is up (press or repeat) */
		bool IsKeyDown(Key key) const;
		/** whether the key is up (released) */
		bool IsKeyUp(Key key) const;
		/** whether the key has just been pressed */
		bool IsKeyJustPressed(Key key) const;
		/** whether the key has just been released */
		bool IsKeyJustReleased(Key key) const;

		/** get the axis value */
		float GetAxisValue(GamepadAxis axis) const;

		/** get the stick value */
		glm::vec2 GetStickValue(GamepadStick stick) const;

		/** returns true whether there is any actioned key */
		bool IsAnyKeyAction() const;
		/** returns true whether there is any axis in use */
		bool IsAnyAxisAction() const;
		/** returns true whenever a key is pressed or an axis is in action */
		bool IsAnyAction() const;

		/** returns true whether there is a key that just has become pressed */
		bool IsAnyKeyJustPressed() const;
	};

#endif

}; // namespace chaos