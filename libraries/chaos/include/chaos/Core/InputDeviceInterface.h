namespace chaos
{
#ifdef CHAOS_FORWARD_DECLARATION

	class InputDeviceInterface;

#elif !defined CHAOS_TEMPLATE_IMPLEMENTATION

	/**
	* Some aliases
	*/

	using ForAllKeysFunction = LightweightFunction<bool(Key, KeyState const &)>;

	using ForAllInput1DFunction = LightweightFunction<bool(Input1D, Input1DState const &)>;

	using ForAllInput2DFunction = LightweightFunction<bool(Input2D, Input2DState const &)>;

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
		/** gets one input1D state */
		Input1DState const * GetInputState(Input1D input) const;
		/** gets one input2D state */
		Input2DState const * GetInputState(Input2D input) const;

		/** enumerate keys */
		bool ForAllKeys(ForAllKeysFunction func) const;
		/** enumerate input1D */
		bool ForAllInput1D(ForAllInput1DFunction func) const;
		/** enumerate input2D */
		bool ForAllInput2D(ForAllInput2DFunction func) const;

		/** get the key value */
		bool GetInputValue(Key key) const;
		/** get the axis value */
		float GetInputValue(Input1D input) const;
		/** get the stick value */
		glm::vec2 GetInputValue(Input2D input) const;


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
		/** gets one input1D state */
		virtual Input1DState const * DoGetInputState(Input1D input) const;
		/** gets one input2D state */
		virtual Input2DState const * DoGetInputState(Input2D input) const;

		/** enumerate keys */
		virtual bool DoForAllKeys(ForAllKeysFunction func) const;
		/** enumerate input1D */
		virtual bool DoForAllInput1D(ForAllInput1DFunction func) const;
		/** enumerate input2D */
		virtual bool DoForAllInput2D(ForAllInput2DFunction func) const;
	};

#endif

}; // namespace chaos