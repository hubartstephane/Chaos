namespace chaos
{
#ifdef CHAOS_FORWARD_DECLARATION

	class ImGuiObject;

	enum class ImGuiDrawFlags;

#elif !defined CHAOS_TEMPLATE_IMPLEMENTATION

	/**
	 * ImGuiDrawFlags: some flags concerning ImGui rendering
	 */

	enum class ImGuiDrawFlags : int
	{
		NONE = 0,
		FULL_WINDOW = 1,           // the ImGui (pseudo) window fills the whole GLFW window
		FLOATING_IMGUI_WINDOW = 2, // this is a standart ImGui (pseudo) window
		USE_FULL_WINDOW_MENU = 4   // menu items are stored inside the full GLFW window's menu instead of the ImGui (pseudo) window's menu
	};

	CHAOS_DECLARE_ENUM_BITMASK_METHOD(ImGuiDrawFlags, CHAOS_API);

	using BeginImGuiMenuFunc = LightweightFunction<void(LightweightFunction<void()>)>;

	/**
	 * ImGuiObject: an interface to display an ImGui window
	 */
	class CHAOS_API ImGuiObject : public Object, public NamedInterface
	{
	public:

		CHAOS_DECLARE_OBJECT_CLASS(ImGuiObject, Object);

		/** main method to display both menu and content */
		virtual void DrawImGui(char const * title, ImGuiDrawFlags flags);

		/** request to close this interface */
		void RequestClosing();
		/** check whether closing is requested */
		bool IsClosingRequested() const;

		/** create a fullscreen window and fill with content (returns true if the window must be kept alive) */
		static bool FullscreenWindow(char const* title, int imgui_window_flags, LightweightFunction<void()> content_func);
		/** create a floating window and fill with content (returns true if the window must be kept alive) */
		static bool FloatingWindow(char const* title, int imgui_window_flags, LightweightFunction<void()> content_func);

	protected:

		/** get flags */
		virtual int GetImGuiWindowFlags() const;
		/** method to override to display content */
		virtual void OnDrawImGuiContent();
		/** method to override to display menu */
		virtual void OnDrawImGuiMenu(BeginImGuiMenuFunc begin_menu_func);

		/** check whether a context is valid for having a menubar and call FullScreen or Floating window menu bar method */
		void OnDrawImGuiMenuConditional(ImGuiDrawFlags flags, int imgui_window_flags);
		/** update the imgui window flags according to use flags */
		int UpdateWindowFlagsForMenu(ImGuiDrawFlags flags, int imgui_window_flags);

	protected:

		/** indicates whether closing this UI is requested */
		bool closing_request = false;
	};

#endif

}; // namespace chaos