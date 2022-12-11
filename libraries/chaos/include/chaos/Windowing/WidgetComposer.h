namespace chaos
{
#ifdef CHAOS_FORWARD_DECLARATION

	class LinearComposerWidget;
	class HorizontalBoxWidget;
	class VerticalBoxWidget;
	class OverlayWidget;
	class GridBoxWidget;
	class LinearComposerWidgetInsertData;
	class LinearComposerLayoutWidget;

	enum class LinearComposerWidgetInsertType;

#elif !defined CHAOS_TEMPLATE_IMPLEMENTATION

	/**
	* LinearComposerWidgetInsertType: how a widget must be inserted in its parent
	*/

	enum class CHAOS_API LinearComposerWidgetInsertType : int
	{
		START,               // insert at the beginning of the children list
		END,                 // insert at the end of the children list
		POSITIONAL,          // insert at position (all elements after are pushed toward the end)
		POSITIONAL_REPLACE   // insert at position (if an element already exists at this position it is being removed from the children list)
	};

	/**
	* LinearComposerWidgetInsertData: a base widget for with some basic insertion method
	*/

	class LinearComposerWidgetInsertData
	{
	public:

		/** constructor */
		LinearComposerWidgetInsertData() = default;
		/** copy constructor */
		LinearComposerWidgetInsertData(LinearComposerWidgetInsertData const &) = default;
		/** move constructor */
		LinearComposerWidgetInsertData(LinearComposerWidgetInsertData&&) = default;
		/** constructor from type */
		LinearComposerWidgetInsertData(LinearComposerWidgetInsertType in_type);
		/** constructor from position */
		LinearComposerWidgetInsertData(size_t in_position);

	public:

		/** how the child must be inserted */
		LinearComposerWidgetInsertType type = LinearComposerWidgetInsertType::END;
		/** where the child must be inserted */
		size_t position = 0;
	};

	/**
	* LinearComposerWidget: a base widget for with some basic insertion method
	*/

	class CHAOS_API LinearComposerWidget : public Widget
	{

		CHAOS_DECLARE_OBJECT_CLASS(LinearComposerWidget, Widget);

	public:

		/** insert a child inside the widget */
		virtual void AddChildWidget(Widget* widget, LinearComposerWidgetInsertData insert_data = {}, bool immediate_update = true);
		/** remove a child from the widget */
		virtual void RemoveChildWidget(Widget* widget, bool immediate_update = true);
	};

	/**
	* OverlayWidget: a container whose children are one over the other
	*/

	class CHAOS_API OverlayWidget : public LinearComposerWidget
	{
		CHAOS_DECLARE_OBJECT_CLASS(OverlayWidget, LinearComposerWidget);

	public:

		/** override */
		virtual void SetPlacement(aabox2 const& in_placement) override;
	};

	/**
	* LinearComposerLayoutWidget: a composer that use the span layout to compute child placement
	*/

	class CHAOS_API LinearComposerLayoutWidget : public LinearComposerWidget
	{
	protected:

		/** override */
		virtual void SetPlacement(aabox2 const& in_placement) override;

		/** gets the layout for placement computation */
		virtual LinearComposerLayout GetComposerLayout() { return {}; }
	};


	/**
	* HorizontalBoxWidget: a container whose children are horizontally aligned
	*/

	class CHAOS_API HorizontalBoxWidget : public LinearComposerLayoutWidget
	{
		CHAOS_DECLARE_OBJECT_CLASS(HorizontalBoxWidget, LinearComposerWidget);

	public:



	};

	/**
	* VerticalBoxWidget: a container whose children are vertically aligned
	*/

	class CHAOS_API VerticalBoxWidget : public LinearComposerLayoutWidget
	{
		CHAOS_DECLARE_OBJECT_CLASS(VerticalBoxWidget, LinearComposerWidget);

	public:

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