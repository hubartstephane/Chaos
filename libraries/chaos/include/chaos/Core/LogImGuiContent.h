namespace chaos
{
#ifdef CHAOS_FORWARD_DECLARATION

	class ImGuiDrawableInterface;

#elif !defined CHAOS_TEMPLATE_IMPLEMENTATION

	class CHAOS_API LogImGuiContent : public ImGuiDrawableInterface
	{

	public:

		/** initialize the object */
		void SetLog(Log const* in_log);
		/** override */
		virtual void DrawImGui() override;

	protected:

		/** the log object to display */
		Log const* log = nullptr;

		/** whether the messages are to be displayed */
		bool show_messages = true;
		/** whether the warnings are to be displayed */
		bool show_warnings = true;
		/** whether the errors are to be displayed */
		bool show_errors = true;
		/** whether consecutive identical messages are to be grouped alltogether */
		bool group_identical_lines = true;
	};

#endif

}; // namespace chaos