namespace chaos
{
#ifdef CHAOS_FORWARD_DECLARATION

	class ImGuiHelpDrawable;

#elif !defined CHAOS_TEMPLATE_IMPLEMENTATION

	/**
	* ImGuiHelpDrawable: a drawable that displays some help for keys
	*/

	class ImGuiHelpDrawable : public ImGuiObject
	{
	public:

		CHAOS_DECLARE_OBJECT_CLASS(ImGuiHelpDrawable, ImGuiObject);

		/** override */
		virtual void DrawImGui(ImGuiDrawMenuMode menu_mode) override;
	};

#endif

}; // namespace chaos