#include "chaos/ChaosPCH.h"
#include "chaos/ChaosInternals.h"

namespace chaos
{
	AutoCastable<Widget> Widget::GetParent()
	{
		return parent;
	}

	AutoConstCastable<Widget> Widget::GetParent() const
	{
		return parent;
	}

	void Widget::OnDetachedFromParent(Widget* in_parent)
	{
	}

	void Widget::OnAttachedToParent(Widget* in_parent)
	{
	}

	AutoCastable<Window> Widget::GetWindow()
	{
		Widget* widget = this; // do not point to parent so that this function is working for WindowRootWidget 
		while (widget != nullptr)
		{
			if (WindowRootWidget* root_widget = auto_cast(widget))
				return root_widget->window;
			widget = widget->parent;
		}
		return nullptr;
	}

	AutoConstCastable<Window> Widget::GetWindow() const
	{
		Widget const* widget = this; // do not point to parent so that this function is working for WindowRootWidget 
		while (widget != nullptr)
		{
			if (WindowRootWidget const* root_widget = auto_cast(widget))
				return root_widget->window;
			widget = widget->parent;
		}
		return nullptr;
	}

	WidgetLayout const& Widget::GetLayout() const
	{
		return layout;
	}

	void Widget::SetLayout(WidgetLayout const& in_layout, bool update_placement_hierarchy)
	{
		layout = in_layout;
		if (update_placement_hierarchy)
			UpdatePlacementHierarchy();
	}

	void Widget::UpdatePlacementHierarchy()
	{
		if (Window* window = GetWindow())
			window->UpdateWidgetPlacementHierarchy();
	}

	void Widget::SetPlacement(WidgetPlacement const& in_placement)
	{
		placement = ApplyModifiersToPlacement(in_placement);
	}

	WidgetPlacement Widget::ApplyModifiersToPlacement(WidgetPlacement const& in_placement)
	{
		WidgetPlacement result;

		result.size.x = std::max(0, in_placement.size.x - properties.padding.left - properties.padding.right);
		result.size.y = std::max(0, in_placement.size.y - properties.padding.top - properties.padding.bottom);

		result.position.x = (result.size.x == 0)?
			0:
			in_placement.position.x + properties.padding.left;

		result.position.y = (result.size.y == 0) ?
			0 :
			in_placement.position.y + properties.padding.top;
		
		if (properties.aspect_ratio >= 0.0f)
			result = ApplyAspectRatioConstraint(result, properties.aspect_ratio);

		return result;
	}

}; // namespace chaos
