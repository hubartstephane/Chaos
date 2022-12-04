namespace chaos
{
#ifdef CHAOS_FORWARD_DECLARATION

	class SingleDirectionBoxWidget;
	class HorizontalBoxWidget;
	class VerticalBoxWidget;
	class OverlayWidget;
	class GridBoxWidget;

#elif !defined CHAOS_TEMPLATE_IMPLEMENTATION

	/**
	* ViewportComposer : An object that is both a Viewport an a ViewportServerInterface
	*/

	class CHAOS_API SingleDirectionBoxWidget : public Widget
	{

		CHAOS_DECLARE_OBJECT_CLASS(SingleDirectionBoxWidget, Widget);

	public:

		virtual void AddChildWidget(Widget* widget);

		virtual void RemoveChildWidget(Widget* widget);
	};

	class CHAOS_API HorizontalBoxWidget : public SingleDirectionBoxWidget
	{
		CHAOS_DECLARE_OBJECT_CLASS(HorizontalBoxWidget, SingleDirectionBoxWidget);

	public:



	};

	class CHAOS_API VerticalBoxWidget : public SingleDirectionBoxWidget
	{
		CHAOS_DECLARE_OBJECT_CLASS(VerticalBoxWidget, SingleDirectionBoxWidget);

	public:

	};

	class CHAOS_API OverlayWidget : public SingleDirectionBoxWidget
	{
		CHAOS_DECLARE_OBJECT_CLASS(OverlayWidget, SingleDirectionBoxWidget);

	public:

	};

	class CHAOS_API GridBoxWidget : public Widget
	{

	public:


	};













#endif

}; // namespace chaos