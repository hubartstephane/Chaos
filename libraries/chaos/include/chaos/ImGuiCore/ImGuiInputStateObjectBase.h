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
		void DisplayAllKeyInfo(InputDeviceInterface const* in_input_device, char const* table_title, char const * title, ForAllKeysFunction filter_func = ConstantFunction(true)) const;
		/** display a single key info */
		void DisplayKeyInfo(Key input, KeyState const & state) const;
		/** display info for all axes matching certain criteria */
		void DisplayAllInput1DInfo(InputDeviceInterface const* in_input_device, char const* table_title, char const * title, ForAllInput1DFunction filter_func = ConstantFunction(true)) const;
		/** display a single axis info */
		void DisplayInput1DInfo(Input1D input, Input1DState const & state) const;
		/** display info for all sticks matching certain criteria */
		void DisplayAllInput2DInfo(InputDeviceInterface const* in_input_device, char const* table_title, char const * title, ForAllInput2DFunction filter_func = ConstantFunction(true)) const;
		/** display a single stick info */
		void DisplayInput2DInfo(Input2D input, Input2DState const & state) const;
	};

#endif

}; // namespace chaos