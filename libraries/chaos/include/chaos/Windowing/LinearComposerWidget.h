namespace chaos
{
#ifdef CHAOS_FORWARD_DECLARATION

	class LinearComposerWidget;
	class LinearComposerWidgetInsertData;

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

#endif

}; // namespace chaos