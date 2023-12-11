namespace chaos
{
#ifdef CHAOS_FORWARD_DECLARATION

	class ImGuiWindowInformationObject;


#elif !defined CHAOS_TEMPLATE_IMPLEMENTATION

	/**
	* ImGuiWindowInformationObject: a drawable that displays some window informations
	*/

	class ImGuiWindowInformationObject : public ImGuiObject
	{
	public:

		CHAOS_DECLARE_OBJECT_CLASS(ImGuiWindowInformationObject, ImGuiObject);

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