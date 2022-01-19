namespace chaos
{
#ifdef CHAOS_FORWARD_DECLARATION

	class ViewportLayout;

#elif !defined CHAOS_TEMPLATE_IMPLEMENTATION

	/**
	 * The layout is responsible for computing the surface to reserve for all viewports owned by a ViewportWindow
	 */

	class ViewportLayout : public Object
	{
		CHAOS_DECLARE_OBJECT_CLASS(ViewportLayout, Object);

		friend class ViewportWindow;

	public:

		/** compute the position of all viewport in their viewport window */
		virtual ViewportPlacement ComputeViewportPlacement(Viewport* viewport, glm::ivec2 const & window_size, size_t viewport_index, size_t viewport_count) const;

		/** gets the window using the layout */
		AutoCastable<ViewportWindow> GetWindow() { return window; }
		/** gets the window using the layout */
		AutoConstCastable<ViewportWindow> GetWindow() const { return window; }

		/** Update the window placements */
		void UpdateWindowViewportPlacements();

	protected:

		/** the viewport owning this layout */
		ViewportWindow* window = nullptr;
	};

	class ViewportGridLayout : public ViewportLayout
	{
		CHAOS_DECLARE_OBJECT_CLASS(ViewportGridLayout, ViewportLayout);

	public:

		/** override */
		virtual ViewportPlacement ComputeViewportPlacement(Viewport* viewport, glm::ivec2 const& window_size, size_t viewport_index, size_t viewport_count) const override;

		/** change the max number of viewport along the orientation */
		void SetMaxViewportCount(size_t in_max_viewport_count, bool update_placements = true);
		/** change the orientation */
		void SetOrientation(Orientation in_orientation, bool update_placements = true);

	protected:

		/** the orientation of the viewports */
		Orientation orientation = Orientation::HORIZONTAL;
		/** the max number of viewports along the orientation (0 for infinite) */
		size_t max_viewport_count = 0;
		/** some padding */
		ViewportPadding padding;
	};

#endif

}; // namespace chaos