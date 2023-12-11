namespace chaos
{
#ifdef CHAOS_FORWARD_DECLARATION

	class ImGuiHelpObject;

#elif !defined CHAOS_TEMPLATE_IMPLEMENTATION

	/**
	* ImGuiHelpObject: a drawable that displays some help for keys
	*/

	class ImGuiHelpObject : public ImGuiObject
	{
	public:

		CHAOS_DECLARE_OBJECT_CLASS(ImGuiHelpObject, ImGuiObject);

		/** override */
		virtual void DrawImGui(ImGuiDrawMenuMode menu_mode) override;
	};

#endif

}; // namespace chaos