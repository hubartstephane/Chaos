namespace chaos
{
#ifdef CHAOS_FORWARD_DECLARATION

	class ViewportLayout;
	class ViewportPosition;

#elif !defined CHAOS_TEMPLATE_IMPLEMENTATION

	/**
	 * The viewport position and size in its ViewportWindow
	 */
	class ViewportSurface
	{
	public:

		/** the top/left position of the viewport */
		glm::ivec2 position = { 0, 0 };
		/** the size allocated for the viewport */
		glm::ivec2 size = { 0, 0 };
	};

	/**
	 * The layout is responsible for computing the surface to reserve for all viewports owned by a ViewportWindow
	 */

	class ViewportLayout : public Object
	{
	public:

		/** compute the position of all viewport in their viewport window */
		virtual ViewportSurface ComputeViewportSurface(glm::ivec2 const & window_size, size_t viewport_index, size_t viewport_count) const;

	};

#endif

}; // namespace chaos