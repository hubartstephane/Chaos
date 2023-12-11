namespace chaos
{
#ifdef CHAOS_FORWARD_DECLARATION

	class ImGuiGlobalVariableDrawable;

#elif !defined CHAOS_TEMPLATE_IMPLEMENTATION

	/**
	* ImGuiGlobalVariableDrawable: a drawable that display the global variables
	*/

	class ImGuiGlobalVariableDrawable : public ImGuiObject
	{
	public:

		CHAOS_DECLARE_OBJECT_CLASS(ImGuiGlobalVariableDrawable, ImGuiObject);

		/** override */
		virtual void DrawImGui(ImGuiDrawMenuMode menu_mode) override;
	};

#endif

}; // namespace chaos