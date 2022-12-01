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

		/** override */
		virtual void SetViewportPlacement(ViewportPlacement const& in_placement) override;

		/** insert a viewport inside the composer */
		void AddViewport(Viewport* viewport, bool update_placement_hierarchy = true);
		/** remove viewport inside the composer */
		void RemoveViewport(Viewport* viewport, bool keep_empty_place, bool update_placement_hierarchy = true);

		/** gets the viewport layout */
		AutoCastable<ViewportLayout> GetViewportLayout();
		/** gets the viewport layout */
		AutoConstCastable<ViewportLayout> GetViewportLayout() const;

		/** change the viewport layout */
		void SetViewportLayout(ViewportLayout* in_viewport_layout, bool update_placement_hierarchy = true);

		/** update the surface allocation for all viewports */
		void ComputeViewportPlacements(glm::ivec2 size);

	protected:

		/** the viewport layout */
		shared_ptr<ViewportLayout> viewport_layout;
	};

#endif

}; // namespace chaos