namespace chaos
{
#ifdef CHAOS_FORWARD_DECLARATION

	class InputDeviceInterface;

#elif !defined CHAOS_TEMPLATE_IMPLEMENTATION

	/**
	* Some aliases
	*/

	using ForAllKeysFunction = LightweightFunction<bool(Key, KeyState const &)>;

	using ForAllAxesFunction = LightweightFunction<bool(GamepadAxis, AxisState const &)>;

	using ForAllSticksFunction = LightweightFunction<bool(GamepadStick, StickState const &)>;

	using EnumerateDeviceHierarchyFunction = LightweightFunction<bool(InputDeviceInterface const*)>;

	/**
	* InputDeviceInterface: a class to make request for some device state
	*/

	class CHAOS_API InputDeviceInterface
	{

	public:

		/** call a function on all devices handled by this whole hierarchy (composition pattern) */
		virtual bool EnumerateDeviceHierarchy(EnumerateDeviceHierarchyFunction func) const;

		/** gets one key state */
		KeyState const * GetInputState(Key key) const;
		/** gets one axis state */
		AxisState const * GetInputState(GamepadAxis axis) const;
		/** gets one stick state */
		StickState const * GetInputState(GamepadStick stick) const;

		/** enumerate keys */
		bool ForAllKeys(ForAllKeysFunction func) const;
		/** enumerate axes */
		bool ForAllAxes(ForAllAxesFunction func) const;
		/** enumerate sticks */
		bool ForAllSticks(ForAllSticksFunction func) const;

		/** get the key value */
		bool GetInputValue(Key key) const;
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
		float GetInputValue(GamepadAxis axis) const;

		/** get the stick value */
		glm::vec2 GetInputValue(GamepadStick stick) const;

		/** returns true whether there is any actioned key */
		bool IsAnyKeyAction() const;
		/** returns true whether there is any axis in use */
		bool IsAnyAxisAction() const;
		/** returns true whenever a key is pressed or an axis is in action */
		bool IsAnyAction() const;

		/** returns true whether there is a key that just has become pressed */
		bool IsAnyKeyJustPressed() const;

	protected:

		/** gets one key state */
		virtual KeyState const * DoGetInputState(Key key) const;
		/** gets one axis state */
		virtual AxisState const * DoGetInputState(GamepadAxis axis) const;
		/** gets one stick state */
		virtual StickState const * DoGetInputState(GamepadStick stick) const;

		/** enumerate keys */
		virtual bool DoForAllKeys(ForAllKeysFunction func) const;
		/** enumerate axes */
		virtual bool DoForAllAxes(ForAllAxesFunction func) const;
		/** enumerate sticks */
		virtual bool DoForAllSticks(ForAllSticksFunction func) const;
	};

#endif

}; // namespace chaos