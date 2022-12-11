namespace chaos
{
#ifdef CHAOS_FORWARD_DECLARATION

	class SingleDirectionBoxWidget;
	class HorizontalBoxWidget;
	class VerticalBoxWidget;
	class OverlayWidget;
	class GridBoxWidget;
	class SingleDirectionBoxWidgetInsertData;

	enum class SingleDirectionBoxWidgetInsertType;

#elif !defined CHAOS_TEMPLATE_IMPLEMENTATION

	/**
	* SingleDirectionBoxWidgetInsertType: how a widget must be inserted in its parent
	*/

	enum class SingleDirectionBoxWidgetInsertType : int
	{
		START,               // insert at the beginning of the children list
		END,                 // insert at the end of the children list
		POSITIONAL,          // insert at position (all elements after are pushed toward the end)
		POSITIONAL_REPLACE   // insert at position (if an element already exists at this position it is being removed from the children list)
	};

	/**
	* SingleDirectionBoxWidgetInsertData: a base widget for with some basic insertion method
	*/

	class SingleDirectionBoxWidgetInsertData
	{
	public:

		/** constructor */
		SingleDirectionBoxWidgetInsertData() = default;
		/** copy constructor */
		SingleDirectionBoxWidgetInsertData(SingleDirectionBoxWidgetInsertData const &) = default;
		/** move constructor */
		SingleDirectionBoxWidgetInsertData(SingleDirectionBoxWidgetInsertData&&) = default;
		/** constructor from type */
		SingleDirectionBoxWidgetInsertData(SingleDirectionBoxWidgetInsertType in_type);
		/** constructor from position */
		SingleDirectionBoxWidgetInsertData(size_t in_position);

	public:

		/** how the child must be inserted */
		SingleDirectionBoxWidgetInsertType type = SingleDirectionBoxWidgetInsertType::END;
		/** where the child must be inserted */
		size_t position = 0;
	};

	/**
	* SingleDirectionBoxWidget: a base widget for with some basic insertion method
	*/

	class CHAOS_API SingleDirectionBoxWidget : public Widget
	{

		CHAOS_DECLARE_OBJECT_CLASS(SingleDirectionBoxWidget, Widget);

	public:

		virtual void AddChildWidget(Widget* widget, SingleDirectionBoxWidgetInsertData insert_data = {}, bool immediate_update = true);

		virtual void RemoveChildWidget(Widget* widget, bool immediate_update = true);
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