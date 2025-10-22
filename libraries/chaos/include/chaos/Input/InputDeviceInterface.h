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
		KeyState const * GetInputState(Key input) const;
		/** gets one input1D state */
		Input1DState const * GetInputState(Input1D input) const;
		/** gets one input2D state */
		Input2DState const * GetInputState(Input2D input) const;

		/** gets one key state */
		KeyState * GetInputState(Key input);
		/** gets one input1D state */
		Input1DState * GetInputState(Input1D input);
		/** gets one input2D state */
		Input2DState * GetInputState(Input2D input);

		/** enumerate keys */
		bool ForAllKeys(ForAllKeysFunction func) const;
		/** enumerate input1D */
		bool ForAllInput1D(ForAllInput1DFunction func) const;
		/** enumerate input2D */
		bool ForAllInput2D(ForAllInput2DFunction func) const;

		/** get the key value */
		bool GetInputValue(Key input) const;
		/** get the axis value */
		float GetInputValue(Input1D input) const;
		/** get the stick value */
		glm::vec2 GetInputValue(Input2D input) const;

		/** get the key state change */
		InputStatus GetInputStatus(Key key) const;
		/** get the input1D state change */
		InputStatus GetInputStatus(Input1D input) const;
		/** get the input2D state change */
		InputStatus GetInputStatus(Input2D input) const;

		/** returns true whether there is any k active */
		bool IsAnyKeyActive() const;
		/** returns true whether there is any input1D active */
		bool IsAnyInput1DActive() const;
		/** returns true whether there is any input2D active */
		bool IsAnyInput2DActive() const;
		/** returns true whenever any key, input1D or input2D is active */
		bool IsAnyInputActive() const;

		/** returns true whether there is a key that just has become pressed */
		bool HasAnyKeyJustBecameActive() const;

	protected:

		/** gets one key state */
		virtual KeyState const * DoGetInputState(Key input) const;
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