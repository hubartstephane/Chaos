namespace chaos
{
#ifdef CHAOS_FORWARD_DECLARATION

	class ImGuiKeyboardAndMouseStateObject;

#elif !defined CHAOS_TEMPLATE_IMPLEMENTATION

	/**
	* ImGuiKeyboardAndMouseStateObject: an ImGUIObject that display the state of keyboard and mouse
	*/

	class ImGuiKeyboardAndMouseStateObject : public ImGuiInputStateObjectBase
	{
	public:

		CHAOS_DECLARE_OBJECT_CLASS(ImGuiKeyboardAndMouseStateObject, ImGuiInputStateObjectBase);

	protected:

		/** override */
		virtual void OnDrawImGuiContent(Window * window) override;

		/** display all keyboard or mouse keys */
		void DisplayKeyboardAndMouseKeyStates(char const * title, char const * table_title, bool hide_cold_keys, KeyType key_type) const;
	};

#endif

}; // namespace chaos