namespace chaos
{
#ifdef CHAOS_FORWARD_DECLARATION

	class ImGuiWindowInformationDrawable;


#elif !defined CHAOS_TEMPLATE_IMPLEMENTATION

	/**
	* ImGuiWindowInformationDrawable: a drawable that displays some window informations
	*/

	class ImGuiWindowInformationDrawable : public ImGuiObject
	{
	public:

		CHAOS_DECLARE_OBJECT_CLASS(ImGuiWindowInformationDrawable, ImGuiObject);

		/** initialization function */
		void SetWindow(Window* in_window);

		/** override */
		virtual void DrawImGui(ImGuiDrawMenuMode menu_mode) override;

	protected:

		/** the target window */
		Window* window = nullptr;
	};

#endif

}; // namespace chaos