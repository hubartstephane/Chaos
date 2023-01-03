namespace chaos
{
#ifdef CHAOS_FORWARD_DECLARATION

	enum class WidgetFillMode;

	class WidgetLayout;

#elif !defined CHAOS_TEMPLATE_IMPLEMENTATION

	/**
	 * how widget is placed inside the area that is given to it
	 */
	enum class WidgetFillMode
	{
		CENTERED,       // take only space given to widget. do not want more
		PACKED_NORMAL,  // the widget is packed to the LEFT or BOTTOM of the available area
		PACKED_REVERSE, // the widget is packed to the RIGHT or TOP of the available area
		FILL,           // take as much size as possible
	};

	/**
	 * Some geometric information to how to place each widget inside its given area
	 */
	class CHAOS_API WidgetLayout
	{
	public:

		/** the padding of the widget */
		Padding padding;
		/** whether the aspect_ratio of the widget has to be forced (if > 0.0f) */
		float aspect_ratio = 0.0f;
		/** how to place the widget inside the area that is being given to it */
		WidgetFillMode fill_mode_x = WidgetFillMode::CENTERED;
		/** how to place the widget inside the area that is being given to it */
		WidgetFillMode fill_mode_y = WidgetFillMode::CENTERED;
	};

#endif

}; // namespace chaos