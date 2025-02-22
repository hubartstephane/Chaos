namespace chaos
{
#ifdef CHAOS_FORWARD_DECLARATION

	class ImGuiImPlotDemoObject;

#elif !defined CHAOS_TEMPLATE_IMPLEMENTATION

	/**
	 * ImGuiImPlotDemoObject: ImGui content for displaying ImPlot demo window
	 **/

	class CHAOS_API ImGuiImPlotDemoObject : public ImGuiObject
	{
	public:

		CHAOS_DECLARE_OBJECT_CLASS(ImGuiImPlotDemoObject, ImGuiObject);

	protected:

		/** override */
		virtual void OnDrawImGuiContent(Window* window) override;
	};

#endif

}; // namespace chaos