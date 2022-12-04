namespace chaos
{
#ifdef CHAOS_FORWARD_DECLARATION

	class WindowRootWidget;

#elif !defined CHAOS_TEMPLATE_IMPLEMENTATION

	/**
	* WindowRootWidget : this widget is only here to make link between window and widget
	*/

	class CHAOS_API WindowRootWidget : public Widget
	{
		friend class Window;
		friend class Widget;

		CHAOS_DECLARE_OBJECT_CLASS(WindowRootWidget, Widget);

	protected:

		/** the owning widget */
		Window* window = nullptr;
	};

#endif

}; // namespace chaos