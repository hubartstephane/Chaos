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

	public:

		/** compute the position of all viewport in their viewport window */
		virtual ViewportPlacement ComputeViewportPlacement(Viewport* viewport, glm::ivec2 const & window_size, size_t viewport_index, size_t viewport_count) const;
	};

	class ViewportGridLayout : public ViewportLayout
	{
		CHAOS_DECLARE_OBJECT_CLASS(ViewportGridLayout, ViewportLayout);

	public:

		/** override */
		virtual ViewportPlacement ComputeViewportPlacement(Viewport* viewport, glm::ivec2 const& window_size, size_t viewport_index, size_t viewport_count) const override;

	public:

		/** the orientation of the viewports */
		Orientation orientation = Orientation::HORIZONTAL;
		/** the max number of viewports along the orientation (0 for infinite) */
		size_t max_viewport_count = 0;
		/** some padding */

	};

#endif

}; // namespace chaos