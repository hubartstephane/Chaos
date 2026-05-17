namespace chaos
{
#ifdef CHAOS_FORWARD_DECLARATION

	class ImGuiApplication;
	class ImGuiApplicationData;

#elif !defined CHAOS_TEMPLATE_IMPLEMENTATION

	class CHAOS_API ImGuiApplicationData : public WindowApplicationData
	{
		CHAOS_DECLARE_OBJECT_CLASS(GameApplicationData, WindowApplicationData);

	public:

		SubClassOf<ImGuiObject> imgui_object_class;
	};

	/**
	 * ImGuiApplication: an application that creates a window to display imgui content
	 **/

	class CHAOS_API ImGuiApplication : public WindowApplication
	{
	public:

		/** override */
		virtual Window* CreateMainWindow() override;

	protected:

		/** override */
		virtual bool IsApplicationDataValid(ApplicationData const * in_application_data) const override;
	};

#endif

}; // namespace chaos