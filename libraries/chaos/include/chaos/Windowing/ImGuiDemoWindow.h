namespace chaos
{
#ifdef CHAOS_FORWARD_DECLARATION

	class ImGuiDemoWindow;

#elif !defined CHAOS_TEMPLATE_IMPLEMENTATION

	/**
	 * ImGuiDemoWindow: a window that deserves displaying ImGui demos
	 **/
	class CHAOS_API ImGuiDemoWindow : public Window
	{
		CHAOS_DECLARE_OBJECT_CLASS(ImGuiDemoWindow, Window);

	protected:

		/** override */
		virtual void OnDrawWindowImGuiContent() override;
		/** override */
		virtual void OnDrawWindowImGuiMenu() override;

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