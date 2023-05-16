namespace chaos
{
#ifdef CHAOS_FORWARD_DECLARATION

	class LoggerImGuiContent;

#elif !defined CHAOS_TEMPLATE_IMPLEMENTATION

	class CHAOS_API LoggerImGuiContent : public ImGuiDrawableInterface
	{

	public:

		/** initialize the object */
		void SetLogger(Logger * in_logger);
		/** override */
		virtual void DrawImGui() override;
		/** override */
		virtual void DrawImGuiMenu() override;

	protected:

		/** the logger object to display */
		Logger * logger = nullptr;

		/** whether the messages are to be displayed */
		bool show_messages = true;
		/** whether the warnings are to be displayed */
		bool show_warnings = true;
		/** whether the errors are to be displayed */
		bool show_errors = true;
		/** whether consecutive identical messages are to be grouped altogether */
		bool group_identical_lines = true;

		/** a filter for logs content */
		ImGuiTextFilter filter;

		/** whether some domains are to be visibles */
		std::map<char const*, bool> domain_visibilities;
	};

#endif

}; // namespace chaos