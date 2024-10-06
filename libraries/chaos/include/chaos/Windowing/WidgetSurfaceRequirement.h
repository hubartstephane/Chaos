namespace chaos
{
#ifdef CHAOS_FORWARD_DECLARATION

	class WidgetSurfaceRequirement;

#elif !defined CHAOS_TEMPLATE_IMPLEMENTATION

	/**
	 * the surface requirement for this widget
	 */
	class CHAOS_API WidgetSurfaceRequirement
	{
	public:

		/** the size along x the widget wants */
		std::optional<float> wanted_size_x;
		/** the size along y the widget wants */
		std::optional<float> wanted_size_y;
		/** whether the widget wants as much space as possible */
		bool fill_x = false;
		/** whether the widget wants as much space as possible */
		bool fill_y = false;
	};

#endif

}; // namespace chaos