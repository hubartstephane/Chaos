#include "chaos/ChaosPCH.h"
#include "chaos/ChaosInternals.h"

namespace chaos
{
	AutoCastable<Widget> Widget::GetParentWidget()
	{
		return parent;
	}

	AutoConstCastable<Widget> Widget::GetParentWidget() const
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

		result.size.x = std::max(0, in_placement.size.x - layout.padding.left - layout.padding.right);
		result.size.y = std::max(0, in_placement.size.y - layout.padding.top - layout.padding.bottom);

		result.position.x = (result.size.x == 0)?
			0:
			in_placement.position.x + layout.padding.left;

		result.position.y = (result.size.y == 0) ?
			0 :
			in_placement.position.y + layout.padding.top;
		
		if (layout.aspect_ratio >= 0.0f)
			result = ApplyAspectRatioConstraint(result, layout.aspect_ratio);

		return result;
	}

	size_t Widget::GetChildWidgetCount() const
	{
		return child_widgets.size();
	}

	AutoCastable<Widget> Widget::GetChildWidget(size_t index)
	{
		return child_widgets[index].get();
	}

	AutoConstCastable<Widget> Widget::GetChildWidget(size_t index) const
	{
		return child_widgets[index].get();
	}

	AutoCastable<Widget> Widget::FindChildWidget(ObjectRequest request)
	{
		return request.FindObject(child_widgets);
	}

	AutoConstCastable<Widget> Widget::FindChildWidget(ObjectRequest request) const
	{
		return request.FindObject(child_widgets);
	}

	AutoCastable<Widget> Widget::FindDescendantWidget(ObjectRequest request)
	{
		if (Widget* result = FindChildWidget(request))
			return result;
		for (auto& child : child_widgets)
			if (child != nullptr)
				if (Widget* result = child->FindDescendantWidget(request))
					return result;
		return nullptr;
	}
	
	AutoConstCastable<Widget> Widget::FindDescendantWidget(ObjectRequest request) const
	{
		if (Widget const* result = FindChildWidget(request))
			return result;
		for (auto& child : child_widgets)
			if (child != nullptr)
				if (Widget* result = child->FindDescendantWidget(request))
					return result;
		return nullptr;
	}

	bool Widget::OnDraw(GPURenderer* renderer, WindowDrawParams const& draw_params, GPUProgramProviderInterface const* uniform_provider)
	{
		for (auto& child : child_widgets)
			child->OnDraw(renderer, draw_params, uniform_provider);
		return true;
	}

	bool Widget::DoTick(float delta_time)
	{
		bool result = false;
		for (auto& child : child_widgets)
			result |= child->Tick(delta_time);
		return result;
	}

}; // namespace chaos
