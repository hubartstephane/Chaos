namespace chaos
{
#ifdef CHAOS_FORWARD_DECLARATION

	class ConsoleWindow;

#elif !defined CHAOS_TEMPLATE_IMPLEMENTATION

	/**
	 * ConsoleWindow: a window that deserves to display log information
	 **/
	class CHAOS_API ConsoleWindow : public Window
	{
		CHAOS_DECLARE_OBJECT_CLASS(ConsoleWindow, Window);

	public:

		/** constructor */
		ConsoleWindow();

	protected:

		/** override */
		virtual void OnDrawWindowImGuiContent() override;
		/** override */
		virtual void OnDrawWindowImGuiMenu() override;

	protected:

		/** ImGui information */
		LoggerImGuiContent console_content;
	};

#endif

}; // namespace chaos