namespace chaos
{
#ifdef CHAOS_FORWARD_DECLARATION

	class ImGuiRenderingStatsObject;

#elif !defined CHAOS_TEMPLATE_IMPLEMENTATION

	/**
	* ImGuiRenderingStatsObject: a drawable that displays some statistics about rendering
	*/

	class ImGuiRenderingStatsObject : public ImGuiObject
	{
	public:

		CHAOS_DECLARE_OBJECT_CLASS(ImGuiRenderingStatsObject, ImGuiObject);

		/** constructor */
		ImGuiRenderingStatsObject();

	protected:

		/** override */
		virtual void OnDrawImGuiContent(Window* window) override;
	};

#endif

}; // namespace chaos