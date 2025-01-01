namespace chaos
{
#ifdef CHAOS_FORWARD_DECLARATION

	class ImGuiInterface;

#elif !defined CHAOS_TEMPLATE_IMPLEMENTATION

	using BeginImGuiMenuFunc = LightweightFunction<void(LightweightFunction<void()>)>;

	/**
	 * ImGuiInterface: an interface to display some ImGui content
	 */
	class CHAOS_API ImGuiInterface
	{
	public:

		/** the entry point to draw the whole ImGui object */
		virtual void DrawImGui(Window * window);
		/** method to override to display content */
		virtual void OnDrawImGuiContent(Window * window);
		/** method to override to display menu */
		virtual void OnDrawImGuiMenu(Window * window, BeginImGuiMenuFunc begin_menu_func);

		/** create a fullscreen window and fill with content (returns true if the window must be kept alive) */
		static bool FullscreenWindow(char const* title, int imgui_window_flags, LightweightFunction<void()> content_func);
		/** create a floating window and fill with content (returns true if the window must be kept alive) */
		static bool FloatingWindow(char const* title, int imgui_window_flags, LightweightFunction<void()> content_func);
		/** create a popup window and fill with content (returns true if the window must be kept alive) */
		static bool PopupWindow(char const* title, int imgui_window_flags, LightweightFunction<void()> content_func);
		/** create a modal popup window and fill with content (returns true if the window must be kept alive) */
		static bool PopupModalWindow(char const* title, int imgui_window_flags, LightweightFunction<void()> content_func);
	};

#endif

}; // namespace chaos