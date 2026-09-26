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
		void DisplayKeyboardAndMouseKeyStates(KeyboardAndMouseDevice const* keyboard_and_mouse_device, char const* table_title, char const * title, bool ignore_unknown_keys, InputDeviceType key_type) const;

	protected:

		/** a map that stores the last time the input was active */
		mutable std::map<Key, double> last_active_key_times;

		/** whether cold keys are to be ignored */
		bool ignore_cold_keys = false;
	};

#endif

}; // namespace chaos