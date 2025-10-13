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
		void DisplayAllKeyInfo(char const * title, InputDeviceInterface const * in_input_device, ForAllKeysFunction filter_func = ConstantFunction(true)) const;
		/** display a single key info */
		void DisplayKeyInfo(Key key, KeyState const & state) const;
		/** display info for all axes matching certain criteria */
		void DisplayAllInput1DInfo(char const * title, InputDeviceInterface const * in_input_device, ForAllInput1DFunction filter_func = ConstantFunction(true)) const;
		/** display a single axis info */
		void DisplayInput1DInfo(Input1D input, Input1DState const & state) const;
		/** display info for all sticks matching certain criteria */
		void DisplayAllInput2DInfo(char const * title, InputDeviceInterface const * in_input_device, ForAllInput2DFunction filter_func = ConstantFunction(true)) const;
		/** display a single stick info */
		void DisplayInput2DInfo(Input2D input, Input2DState const & state) const;
	};

#endif

}; // namespace chaos