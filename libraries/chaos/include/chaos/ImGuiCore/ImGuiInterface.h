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
		virtual void OnDrawImGuiMenu(Window * window, ImGuiMenuBuilder const & menu_builder);
	};

#endif

}; // namespace chaos