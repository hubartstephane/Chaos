namespace chaos
{
#ifdef CHAOS_FORWARD_DECLARATION

	class ImGuiWindow;

#elif !defined CHAOS_TEMPLATE_IMPLEMENTATION

	/**
	 * ImGuiWindow: a window that deserves to display imgui content
	 **/
	class CHAOS_API ImGuiWindow : public Window
	{
		CHAOS_DECLARE_OBJECT_CLASS(ImGuiWindow, Window);

	public:

		/** override */
		virtual WindowCategory GetWindowCategory() const override;

	protected:

		/** override */
		virtual void OnDrawWindowImGuiContent() override;
		/** override */
		virtual void OnDrawWindowImGuiMenu() override;

	protected:

		/** ImGui information */
		shared_ptr<ImGuiObject> content;
	};

#endif

}; // namespace chaos