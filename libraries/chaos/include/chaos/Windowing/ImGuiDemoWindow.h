namespace chaos
{
#ifdef CHAOS_FORWARD_DECLARATION

	class ImGuiDemoWindow;

#elif !defined CHAOS_TEMPLATE_IMPLEMENTATION

	/**
	 * ImGuiDemoWindow: a window that deserves displaying ImGui demo
	 **/
	class CHAOS_API ImGuiDemoWindow : public ImGuiWindow
	{
		CHAOS_DECLARE_OBJECT_CLASS(ImGuiDemoWindow, ImGuiWindow);

	public:

		/** constructor */
		ImGuiDemoWindow();
	};

#endif

}; // namespace chaos