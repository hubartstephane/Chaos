namespace chaos
{
#ifdef CHAOS_FORWARD_DECLARATION

	class ImGuiAtlasObject;

#elif !defined CHAOS_TEMPLATE_IMPLEMENTATION

	/**
	 * ImGuiAtlasObject: an object to display ImGui atlas
	 */
	class CHAOS_API ImGuiAtlasObject : public ImGuiObject
	{
	public:

		CHAOS_DECLARE_OBJECT_CLASS(ImGuiAtlasObject, ImGuiObject);

	protected:

		/** override */
		virtual void OnDrawImGuiContent(Window* window) override;
	};

#endif

}; // namespace chaos