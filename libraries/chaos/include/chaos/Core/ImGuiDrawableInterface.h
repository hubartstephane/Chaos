namespace chaos
{
#ifdef CHAOS_FORWARD_DECLARATION

	class ImGuiDrawableInterface;
	class ImGuiDrawableObject;

	enum class ImGuiDrawMenuMode;

#elif !defined CHAOS_TEMPLATE_IMPLEMENTATION

	enum class ImGuiDrawMenuMode : int
	{
		FullWindow,
		ImGuiWindow
	};

	class CHAOS_API ImGuiDrawableInterface
	{
	public:

		/** destructor */
		virtual ~ImGuiDrawableInterface() = default;
		/** draw both ImGui and Menu */
		virtual void DrawImGui(ImGuiDrawMenuMode menu_mode);

		/** conditionally detect whether a flag for window menu is necessary */
		static int GetConditionalMainMenuFlag(ImGuiDrawMenuMode menu_mode);

		/** start a fullscreen window */
		static void FullscreenWindow(char const* name, bool menu_bar, LightweightFunction<void()> content_func);

		/** try start a menu (maybe FullWindow, maybe ImGuiWindow, maybe failure if ImGUI mode is not active) */
		static bool MenuBar(ImGuiDrawMenuMode menu_mode, LightweightFunction<void()> func);

		/** request to close this interface */
		void RequestClosing()
		{
			closing_request = true;
		}
		/** check whether closing is requested */
		bool IsClosingRequested() const
		{
			return closing_request;
		}
	
	protected:

		/** indicates whether closing this UI is requested */
		bool closing_request = false;
	};

	class CHAOS_API ImGuiDrawableObject : public Object, public ImGuiDrawableInterface
	{
	public:

		CHAOS_DECLARE_OBJECT_CLASS(ImGuiDrawableObject, Object);
	};

#endif

}; // namespace chaos