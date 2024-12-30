namespace chaos
{
#ifdef CHAOS_FORWARD_DECLARATION

	class ImGuiObject;

#elif !defined CHAOS_TEMPLATE_IMPLEMENTATION

	/**
	 * ImGuiObject: an interface to display an ImGui window
	 */
	class CHAOS_API ImGuiObject : public Object, public NamedInterface, public ImGuiInterface
	{
	public:

		CHAOS_DECLARE_OBJECT_CLASS(ImGuiObject, Object);

		/** override */
		virtual void DrawImGui(Window* window) override;

		/** set the flags for this object */
		void SetImGuiObjectFlags(ImGuiObjectFlags in_flags);
		/** request to close this interface */
		void RequestClosing();
		/** check whether closing is requested */
		bool IsClosingRequested() const;

	protected:

		/** get flags */
		virtual int GetImGuiWindowFlags() const;
		/** check whether a context is valid for having a menubar and call FullScreen or Floating window menu bar method */
		void OnDrawImGuiMenuConditional(Window* window, int imgui_window_flags);
		/** update the imgui window flags according to use flags */
		int UpdateWindowFlagsForMenu(Window* window, int imgui_window_flags);

	protected:

		/** the object flags */
		ImGuiObjectFlags imgui_object_flags = ImGuiObjectFlags::FLOATING_WINDOW;
		/** indicates whether closing this UI is requested */
		bool closing_request = false;
		/** indicates whether the popup is opened (only valid if flags has POPUP_WINDOW) */
		bool opened_popup = false;
	};

#endif

}; // namespace chaos