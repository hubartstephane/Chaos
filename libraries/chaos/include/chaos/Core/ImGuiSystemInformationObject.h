namespace chaos
{
#ifdef CHAOS_FORWARD_DECLARATION

	class ImGuiSystemInformationObject;

#elif !defined CHAOS_TEMPLATE_IMPLEMENTATION

	/**
	* ImGuiSystemInformationObject: a drawable that displays some system informations
	*/

	class ImGuiSystemInformationObject : public ImGuiObject
	{
	public:

		CHAOS_DECLARE_OBJECT_CLASS(ImGuiSystemInformationObject, ImGuiObject);

		/** override */
		virtual void DrawImGui(ImGuiDrawMenuMode menu_mode) override;
	};

#endif

}; // namespace chaos