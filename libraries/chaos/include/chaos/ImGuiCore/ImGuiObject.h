namespace chaos
{
#ifdef CHAOS_FORWARD_DECLARATION

	enum class ImGuiObjectFlags;

	class ImGuiObject;

#elif !defined CHAOS_TEMPLATE_IMPLEMENTATION

	/**
	 * ImGuiObjectFlags: some flags concerning ImGui rendering
	 */

	enum class ImGuiObjectFlags : int
	{
		None = 0,
		FullViewport     = (1 << 0), // the ImGui (pseudo) window fills the whole GLFW window
		FloatingWindow   = (1 << 1), // this is a standart ImGui (pseudo) window
		PopupWindow      = (1 << 2), // the object behaves like a popup
		PopupModalWindow = (1 << 3), // the object behaves like a modal popup
		CloseWithCross   = (1 << 4), // the window has a cross to close it (valid for floating window and modal)
		CloseWithEscape  = (1 << 5), // the window can be closed with escaped (not valid for non modal popup)
		UseViewportMenu  = (1 << 6)  // menu items are stored inside the full GLFW window's menu instead of the ImGui (pseudo) window's menu
	};

	CHAOS_DECLARE_ENUM_BITMASK_METHOD(ImGuiObjectFlags, CHAOS_API);

	/**
	 * ImGuiObject: an interface to display an ImGui window
	 */
	class CHAOS_API ImGuiObject : public Object, public NamedInterface, public ImGuiInterface
	{
		friend class Window;

	public:

		CHAOS_DECLARE_OBJECT_CLASS(ImGuiObject, Object);

		/** override */
		virtual void DrawImGui(Window* window) override;

		/** get the flags for this object */
		ImGuiObjectFlags GetImGuiObjectFlags() const;
		/** set the flags for this object */
		void SetImGuiObjectFlags(ImGuiObjectFlags in_flags);

		/** get imgui flags */
		int GetImGuiWindowFlags() const;
		/** set the imgui flags */
		void SetImGuiWindowFlags(int in_imgui_flags);

		/** request to close this interface */
		void RequestClosing();
		/** check whether closing is requested */
		bool IsClosingRequested() const;

		/** set the window placement */
		void SetWindowPlacement(std::optional<ImGuiWindowPlacement> in_placement);

	protected:

		/** called whenever the object is being added to the window */
		virtual void OnAddedToWindow(Window* in_window);
		/** called whenever the object is being removed from the window */
		virtual void OnRemovedFromWindow(Window* in_window);

		/** check whether a context is valid for having a menubar and call FullScreen or Floating window menu bar method */
		void OnDrawImGuiMenuConditional(Window* window, int imgui_window_flags);
		/** update the imgui window flags according to use flags */
		int UpdateWindowFlagsForMenu(Window* window, int imgui_window_flags);

		/** called whenever escape is pressed for closing */
		virtual void OnCloseWithEscape();
		/** called whenever cross is pressed for closing */
		virtual void OnCloseWithCross();

	protected:

		/** the object flags */
		ImGuiObjectFlags imgui_object_flags = ImGuiObjectFlags::FloatingWindow;
		/** the imgui flags */
		int imgui_flags = ImGuiWindowFlags_NoCollapse;
		/** indicates whether closing this UI is requested */
		bool closing_request = false;
		/** indicates whether the popup is opened (only used if the object is a popup) */
		bool opened_popup = false;
		/** the placement */
		std::optional<ImGuiWindowPlacement> placement;
	};

#endif

}; // namespace chaos