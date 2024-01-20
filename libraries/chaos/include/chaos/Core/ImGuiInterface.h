namespace chaos
{
#ifdef CHAOS_FORWARD_DECLARATION

	class ImGuiInterface;
	class ImGuiObject;

	enum class ImGuiDrawFlags;

#elif !defined CHAOS_TEMPLATE_IMPLEMENTATION

	/**
	 * ImGuiDrawFlags: some flags concerning ImGui rendering
	 */

	enum class ImGuiDrawFlags : int
	{
		None = 0,
		FullWindow = 1,
		FloatingImGuiWindow = 2,
		UseFullWindowMenu = 4
	};

	CHAOS_DECLARE_ENUM_BITMASK_METHOD(ImGuiDrawFlags, CHAOS_API);

	/**
	 * ImGuiInterface: an interface to display an ImGui window
	 */
	class CHAOS_API ImGuiInterface
	{
	public:

		using DrawImGuiMenuFunc = LightweightFunction<void(LightweightFunction<void()>)>;


		/** destructor */
		virtual ~ImGuiInterface() = default;

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
		virtual void OnDrawImGuiMenu(DrawImGuiMenuFunc func);

		/** check whether a context is valid for having a menubar and call FullScreen or Floating window menu bar method */
		void ConditionalMenuBar(ImGuiDrawFlags flags, int imgui_window_flags);
		/** update the imgui window flags according to use flags */
		int UpdateWindowFlagsForMenu(ImGuiDrawFlags flags, int imgui_window_flags);

	protected:

		/** indicates whether closing this UI is requested */
		bool closing_request = false;
	};

	class CHAOS_API ImGuiObject : public Object, public ImGuiInterface
	{
	public:

		CHAOS_DECLARE_OBJECT_CLASS(ImGuiObject, Object);
	};

#endif

}; // namespace chaos