namespace chaos
{
#ifdef CHAOS_FORWARD_DECLARATION

	class ImGuiInterface;

	enum class ImGuiObjectFlags;

#elif !defined CHAOS_TEMPLATE_IMPLEMENTATION

	/**
	 * ImGuiObjectFlags: some flags concerning ImGui rendering
	 */

	enum class ImGuiObjectFlags : int
	{
		NONE = 0,
		FULL_WINDOW = 1,           // the ImGui (pseudo) window fills the whole GLFW window
		FLOATING_WINDOW = 2,       // this is a standart ImGui (pseudo) window
		POPUP_WINDOW = 4,          // the object behaves like a popup
		USE_FULL_WINDOW_MENU = 8   // menu items are stored inside the full GLFW window's menu instead of the ImGui (pseudo) window's menu
	};

	CHAOS_DECLARE_ENUM_BITMASK_METHOD(ImGuiObjectFlags, CHAOS_API);

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
	};

#endif

}; // namespace chaos