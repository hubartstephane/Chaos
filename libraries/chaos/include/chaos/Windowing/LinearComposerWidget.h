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

	protected:

		template<bool ADD_X, bool ADD_Y> 
		WidgetSurfaceRequirement ComputeChildrenSurfaceRequirement() const
		{
			auto get_optional_value = [](std::optional<float> const& opt)
			{
				return opt.has_value() ? opt.value() : 0.0f;
			};

			WidgetSurfaceRequirement result;

			for (auto& child : child_widgets)
			{
				if (child != nullptr)
				{
					WidgetSurfaceRequirement child_requirement = child->GetSurfaceRequirement();

					if (child_requirement.wanted_size_x.has_value())
					{
						if constexpr (ADD_X)
							result.wanted_size_x = get_optional_value(result.wanted_size_x) + child_requirement.wanted_size_x.value();
						else
							result.wanted_size_x = std::max(get_optional_value(result.wanted_size_x), child_requirement.wanted_size_x.value());
					}
					if (child_requirement.wanted_size_y.has_value())
					{
						if constexpr (ADD_Y)
							result.wanted_size_y = get_optional_value(result.wanted_size_y) + child_requirement.wanted_size_y.value();
						else
							result.wanted_size_y = std::max(get_optional_value(result.wanted_size_y), child_requirement.wanted_size_y.value());
					}

					result.fill_x |= child_requirement.fill_x;
					result.fill_y |= child_requirement.fill_y;
				}
			}
			return result;
		}
	};

#endif

}; // namespace chaos