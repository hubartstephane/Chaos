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

		/** Create an ImGui table to display inputs */
		static bool WithImGuiInputTable(char const * table_title, LightweightFunction<void()> func);
		/** Display information concerning some key */
		static bool DisplayImGuiKeyInfo(Key key, KeyState const * key_state);
	};

#endif

}; // namespace chaos