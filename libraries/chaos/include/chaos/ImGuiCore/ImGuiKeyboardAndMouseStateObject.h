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

		/** utility function to iterate over all enum possible values and display the key information */
		template<typename ENUM_TYPE, typename GET_KEY_STATE_FUNC>
		void DisplayKeyStates(char const * title, char const * table_title, GET_KEY_STATE_FUNC GetKeyStateFunc, bool hide_irrelevant_state);
	};

#endif

}; // namespace chaos