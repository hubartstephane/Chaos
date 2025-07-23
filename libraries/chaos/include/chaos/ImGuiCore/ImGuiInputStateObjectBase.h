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
		void DisplayKeysInfo(char const * title, InputDeviceUserInterface const * in_device_user, LightweightFunction<bool(Key, KeyState const &)> filter_func = ConstantFunction(true)) const;
		/** display a single key info */
		void DisplayKeyInfo(Key key, KeyState const & state) const;
		/** display info for all axes matching certain criteria */
		void DisplayAxesInfo(char const * title, InputDeviceUserInterface const * in_device_user, LightweightFunction<bool(GamepadAxis, AxisState const &)> filter_func = ConstantFunction(true)) const;
		/** display a single axis info */
		void DisplayAxisInfo(GamepadAxis axis, AxisState const & state) const;
		/** display info for all sticks matching certain criteria */
		void DisplaySticksInfo(char const * title, InputDeviceUserInterface const * in_device_user, LightweightFunction<bool(GamepadStick, StickState const &)> filter_func = ConstantFunction(true)) const;
		/** display a single stick info */
		void DisplayStickInfo(GamepadStick stick, StickState const & state) const;
	};

#endif

}; // namespace chaos