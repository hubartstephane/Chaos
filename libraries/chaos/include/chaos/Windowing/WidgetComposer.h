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
	* SingleDirectionBoxWidget: a base widget for with some basic insertion method
	*/

	class CHAOS_API SingleDirectionBoxWidget : public Widget
	{

		CHAOS_DECLARE_OBJECT_CLASS(SingleDirectionBoxWidget, Widget);

	public:

		virtual void AddChildWidget(Widget* widget, bool update_placement_hierarchy = true);

		virtual void RemoveChildWidget(Widget* widget, bool update_placement_hierarchy = true);
	};

	/**
	* HorizontalBoxWidget: a container whose children are horizontally aligned
	*/

	class CHAOS_API HorizontalBoxWidget : public SingleDirectionBoxWidget
	{
		CHAOS_DECLARE_OBJECT_CLASS(HorizontalBoxWidget, SingleDirectionBoxWidget);

	public:



	};

	/**
	* VerticalBoxWidget: a container whose children are vertically aligned
	*/

	class CHAOS_API VerticalBoxWidget : public SingleDirectionBoxWidget
	{
		CHAOS_DECLARE_OBJECT_CLASS(VerticalBoxWidget, SingleDirectionBoxWidget);

	public:

	};

	/**
	* OverlayWidget: a container whose children are one over the other
	*/

	class CHAOS_API OverlayWidget : public SingleDirectionBoxWidget
	{
		CHAOS_DECLARE_OBJECT_CLASS(OverlayWidget, SingleDirectionBoxWidget);

	public:

		/** override */
		virtual void SetPlacement(aabox2 const& in_placement) override;
	};

	/**
	* GridBoxWidget: a container whose children are dispatched on a grid
	*/

	class CHAOS_API GridBoxWidget : public Widget
	{

	public:


	};













#endif

}; // namespace chaos