namespace chaos
{
#ifdef CHAOS_FORWARD_DECLARATION

	class WidgetLayout;

#elif !defined CHAOS_TEMPLATE_IMPLEMENTATION

	/**
	 * Some geometric restriction for a widget
	 */
	class CHAOS_API WidgetLayout
	{
	public:

		/** the padding of the widget */
		Padding padding;
		/** whether the aspect_ratio of the widget has to be forced (if > 0.0f) */
		float aspect_ratio = 0.0f;
	};

#endif

}; // namespace chaos