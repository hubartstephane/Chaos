namespace chaos
{
#ifdef CHAOS_FORWARD_DECLARATION

	class LogWindow;

#elif !defined CHAOS_TEMPLATE_IMPLEMENTATION

	/**
	 * LogWindow: a window that deserves to display log information
	 **/
	class CHAOS_API LogWindow : public ImGuiWindow
	{
		CHAOS_DECLARE_OBJECT_CLASS(LogWindow, ImGuiWindow);

	public:

		/** constructor */
		LogWindow();
	};

#endif

}; // namespace chaos