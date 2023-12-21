namespace chaos
{
#ifdef CHAOS_FORWARD_DECLARATION

	class GlobalVariablesWindow;

#elif !defined CHAOS_TEMPLATE_IMPLEMENTATION

	/**
	 * GlobalVariablesWindow: a window that deserves to display global variables
	 **/
	class CHAOS_API GlobalVariablesWindow : public ImGuiWindow
	{
		CHAOS_DECLARE_OBJECT_CLASS(GlobalVariablesWindow, ImGuiWindow);

	public:

		/** constructor */
		GlobalVariablesWindow();
	};

#endif

}; // namespace chaos