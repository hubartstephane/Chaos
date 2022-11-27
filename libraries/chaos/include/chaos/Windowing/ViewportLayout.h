namespace chaos
{
#ifdef CHAOS_FORWARD_DECLARATION

	class ViewportLayout;

#elif !defined CHAOS_TEMPLATE_IMPLEMENTATION

	/**
	 * The layout is responsible for computing the surface to reserve for all viewports owned by a ViewportWindow
	 */

	class CHAOS_API ViewportLayout : public Object
	{
		CHAOS_DECLARE_OBJECT_CLASS(ViewportLayout, Object);

		friend class ViewportServerWindow;

	public:

		/** compute the position of all viewport in their viewport window */
		virtual ViewportPlacement ComputeViewportPlacement(Viewport* viewport, glm::ivec2 const & window_size, size_t viewport_index, size_t viewport_count) const;

		/** gets the window using the layout */
		AutoCastable<ViewportServerWindow> GetViewportServerWindow() { return viewport_server; }
		/** gets the window using the layout */
		AutoConstCastable<ViewportServerWindow> GetViewportServerWindow() const { return viewport_server; }

		/** Update the window placements */
		void UpdateWindowViewportPlacements();

	protected:

		/** the viewport owning this layout */
		ViewportServerWindow* viewport_server = nullptr;
	};

#endif

}; // namespace chaos