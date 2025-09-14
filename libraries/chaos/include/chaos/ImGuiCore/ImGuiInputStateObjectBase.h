namespace chaos
{
#ifdef CHAOS_FORWARD_DECLARATION

	class ImGuiInputStateObjectBase;

#elif !defined CHAOS_TEMPLATE_IMPLEMENTATION

	/**
	* ImGuiInputStateObjectBase: an ImGUIObject base object to display some inputs
	*/

	class ImGuiInputStateObjectBase : public ImGuiObject
	{
	public:

		CHAOS_DECLARE_OBJECT_CLASS(ImGuiInputStateObjectBase, ImGuiObject);

	protected:

		/** display for all keys matching certain criteria */
		void DisplayKeysInfo(char const * title, InputDeviceInterface const * in_input_device, ForAllKeysFunction filter_func = ConstantFunction(true)) const;
		/** display a single key info */
		void DisplayKeyInfo(Key key, KeyState const & state) const;
		/** display info for all axes matching certain criteria */
		void DisplayAxesInfo(char const * title, InputDeviceInterface const * in_input_device, ForAllAxesFunction filter_func = ConstantFunction(true)) const;
		/** display a single axis info */
		void DisplayAxisInfo(GamepadAxis axis, AxisState const & state) const;
		/** display info for all sticks matching certain criteria */
		void DisplaySticksInfo(char const * title, InputDeviceInterface const * in_input_device, ForAllSticksFunction filter_func = ConstantFunction(true)) const;
		/** display a single stick info */
		void DisplayStickInfo(GamepadStick stick, StickState const & state) const;
	};

#endif

}; // namespace chaos