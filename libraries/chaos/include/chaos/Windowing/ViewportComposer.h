namespace chaos
{
#ifdef CHAOS_FORWARD_DECLARATION

	class ViewportComposer;

#elif !defined CHAOS_TEMPLATE_IMPLEMENTATION

	/**
	* ViewportComposer : An object that is both a Viewport an a ViewportServerInterface
	*/

	class CHAOS_API ViewportComposer : public Viewport, public ViewportServerInterface
	{
	public:

		/** insert a viewport inside the composer */
		void AddViewport(Viewport* viewport, bool compute_viewport_surfaces);
		/** remove viewport inside the composer */
		void RemoveViewport(Viewport* viewport, bool compute_viewport_surfaces, bool keep_empty_place);

		/** gets the viewport layout */
		AutoCastable<ViewportLayout> GetViewportLayout();
		/** gets the viewport layout */
		AutoConstCastable<ViewportLayout> GetViewportLayout() const;

		/** change the viewport layout */
		void SetViewportLayout(ViewportLayout* in_viewport_layout);

		/** update the surface allocation for all viewports */
		void ComputeViewportPlacements(glm::ivec2 size);

	protected:

		/** the viewport layout */
		shared_ptr<ViewportLayout> viewport_layout;
	};

#endif

}; // namespace chaos