namespace chaos
{
#ifdef CHAOS_FORWARD_DECLARATION

	class ImGuiLogObject;

#elif !defined CHAOS_TEMPLATE_IMPLEMENTATION

	class CHAOS_API ImGuiLogObject : public ImGuiInterface
	{

	public:

		/** initialize the object */
		void SetLogger(Logger * in_logger);

		/** override */
		virtual void DrawImGui(ImGuiDrawMenuMode menu_mode) override;

	protected:

		/** draw the menu */
		void OnDrawImGuiMenu(ImGuiDrawMenuMode menu_mode);
		/** draw the content */
		void OnDrawImGuiContent(ImGuiDrawMenuMode menu_mode);

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