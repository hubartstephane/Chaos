namespace chaos
{
#ifdef CHAOS_FORWARD_DECLARATION

	class ImGuiDemoObject;

#elif !defined CHAOS_TEMPLATE_IMPLEMENTATION

	/**
	 * ImGuiDemoObject: ImGui content for displaying ImGui demos windows
	 **/

	class CHAOS_API ImGuiDemoObject : public ImGuiObject
	{
	public:

		CHAOS_DECLARE_OBJECT_CLASS(ImGuiDemoObject, ImGuiObject);

	protected:

		/** override */
		virtual void OnDrawImGuiContent(Window* window) override;
		/** override */
		virtual void OnDrawImGuiMenu(Window* window, ImGuiMenuBuilder const & menu_builder) override;

	protected:

		bool show_demo = true;
		bool show_metrics = false;
		bool show_debug_log = false;
		bool show_stack_tool = false;
		bool show_about = false;
		bool show_style_editor = false;
		bool show_user_guide = false;
	};

#endif

}; // namespace chaos