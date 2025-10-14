namespace chaos
{
#ifdef CHAOS_FORWARD_DECLARATION

	class ImGuiKeyboardAndMouseDeviceObject;

#elif !defined CHAOS_TEMPLATE_IMPLEMENTATION

	/**
	* ImGuiKeyboardAndMouseDeviceObject: an ImGUIObject that display the state of keyboard and mouse
	*/

	class ImGuiKeyboardAndMouseDeviceObject : public ImGuiInputStateObjectBase
	{
	public:

		CHAOS_DECLARE_OBJECT_CLASS(ImGuiKeyboardAndMouseDeviceObject, ImGuiInputStateObjectBase);

	protected:

		/** override */
		virtual void OnDrawImGuiContent(Window * window) override;

		/** display all keyboard or mouse keys */
		void DisplayKeyboardAndMouseKeyStates(KeyboardAndMouseDevice const* keyboard_and_mouse_device, char const* table_title, char const * title, bool hide_cold_keys, InputDeviceType key_type) const;
	};

#endif

}; // namespace chaos