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

		/** constructor */
		ImGuiWindow();

		/** get the content */
		AutoCastable<ImGuiObject> GetContent() { return content.get(); }
		/** get the content */
		AutoConstCastable<ImGuiObject> GetContent() const { return content.get(); }

		/** change the content */
		void SetContent(ImGuiObject* in_content);

		/** create a ImGui window with a given type */
		template<typename CONTENT_TYPE>
		static ImGuiWindow* CreateImGuiWindow()
		{
			if (ImGuiWindow* result = new ImGuiWindow)
			{
				result->SetContent(new CONTENT_TYPE);
				return result;
			}
			return nullptr;
		}

		/** override */
		virtual void DrawImGui() override;

	protected:

		/** override */
		virtual bool EnumerateKnownImGuiObjects(EnumerateKnownImGuiObjectFunc func) override;

	protected:

		/** ImGui information */
		shared_ptr<ImGuiObject> content;
	};

#endif

}; // namespace chaos