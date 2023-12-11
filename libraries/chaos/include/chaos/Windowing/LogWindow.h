namespace chaos
{
#ifdef CHAOS_FORWARD_DECLARATION

	class LogWindow;

#elif !defined CHAOS_TEMPLATE_IMPLEMENTATION

	/**
	 * LogWindow: a window that deserves to display log information
	 **/
	class CHAOS_API LogWindow : public Window
	{
		CHAOS_DECLARE_OBJECT_CLASS(LogWindow, Window);

	public:

		/** constructor */
		LogWindow();

	protected:

		/** override */
		virtual void OnDrawWindowImGuiContent() override;
		/** override */
		virtual void OnDrawWindowImGuiMenu() override;

	protected:

		/** ImGui information */
		ImGuiLogObject console_content;
	};

#endif

}; // namespace chaos