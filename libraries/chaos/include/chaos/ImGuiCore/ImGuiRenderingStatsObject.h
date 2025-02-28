namespace chaos
{
#ifdef CHAOS_FORWARD_DECLARATION

	class ImGuiRenderingStatObject;
	class ImGuiRenderingFPSStatObject;
	class ImGuiRenderingDrawCallsStatObject;
	class ImGuiRenderingVerticesStatObject;

#elif !defined CHAOS_TEMPLATE_IMPLEMENTATION

	/**
	* ImGuiRenderingStatObject: a base class to display some stats
	*/

	class ImGuiRenderingStatObject : public ImGuiObject
	{
	public:

		CHAOS_DECLARE_OBJECT_CLASS(ImGuiRenderingStatObject, ImGuiObject);

		/** constructor */
		ImGuiRenderingStatObject();

	protected:

		/** a function that displays some stats */
		void DrawStat(char const* title, Window* window, LightweightFunction<float(GPURenderContextFrameStats const&)> stat_getter);
	};

	/**
	* ImGuiRenderingFPSStatObject: a drawable that displays some statistics about FPS
	*/

	class ImGuiRenderingFPSStatObject : public ImGuiRenderingStatObject
	{
	public:

		CHAOS_DECLARE_OBJECT_CLASS(ImGuiRenderingFPSStatObject, ImGuiRenderingStatObject);

	protected:

		/** override */
		virtual void OnDrawImGuiContent(Window* window) override;
	};

	/**
	* ImGuiRenderingDrawCallsStatObject: a drawable that displays some statistics about draw calls
	*/

	class ImGuiRenderingDrawCallsStatObject : public ImGuiRenderingStatObject
	{
	public:

		CHAOS_DECLARE_OBJECT_CLASS(ImGuiRenderingDrawCallsStatObject, ImGuiRenderingStatObject);

	protected:

		/** override */
		virtual void OnDrawImGuiContent(Window* window) override;
	};

	/**
	* ImGuiRenderingVerticesStatObject: a drawable that displays some statistics about vertices
	*/

	class ImGuiRenderingVerticesStatObject : public ImGuiRenderingStatObject
	{
	public:

		CHAOS_DECLARE_OBJECT_CLASS(ImGuiRenderingVerticesStatObject, ImGuiRenderingStatObject);

	protected:

		/** override */
		virtual void OnDrawImGuiContent(Window* window) override;
	};

#endif

}; // namespace chaos