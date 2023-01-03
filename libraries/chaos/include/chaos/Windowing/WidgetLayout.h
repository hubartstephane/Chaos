namespace chaos
{
#ifdef CHAOS_FORWARD_DECLARATION

	class WidgetDesiredSize;
	class WidgetDesiredPlacementInfo;
	class WidgetLayout;

	enum class WidgetFillMode;

#elif !defined CHAOS_TEMPLATE_IMPLEMENTATION

	/**
	 * some data that the widget declares as wanted to its parent
	 */
	class CHAOS_API WidgetDesiredSize
	{
	public:

		/** the size along x the widget wants */
		std::optional<float> wanted_size_x;
		/** the size along y the widget wants */
		std::optional<float> wanted_size_y;
	};

	/**
	 * some data that the widget declares as wanted to its parent
	 */
	class CHAOS_API WidgetDesiredPlacementInfo : public WidgetDesiredSize
	{
	public:

		/** whether the widget wants as much space as possible */
		bool fill_x = false;
		/** whether the widget wants as much space as possible */
		bool fill_y = false;
	};

	/**
	 * how widget is placed inside the area that is given to it
	 */
	enum class WidgetFillMode
	{
		CENTER,            // take only space given to widget. do not want more
		NORMAL_ALIGNMENT,  // the widget is aligned to LEFT or BOTTOM
		REVERSE_ALIGNMENT, // the widget is aligned to RIGHT or TOP
		FILL,              // take as much size as possible
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
		WidgetFillMode fill_mode_x = WidgetFillMode::CENTER;
		/** how to place the widget inside the area that is being given to it */
		WidgetFillMode fill_mode_y = WidgetFillMode::CENTER;
	};

#endif

}; // namespace chaos