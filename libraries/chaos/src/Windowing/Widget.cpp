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

	void Widget::SetLayout(WidgetLayout const& in_layout, bool immediate_update)
	{
		layout = in_layout;
		UpdatePlacementHierarchy(immediate_update);
	}

	void Widget::UpdatePlacementHierarchy(bool immediate_update)
	{
		if (immediate_update)
		{
			if (Window* window = GetWindow())
				window->UpdateWidgetPlacementHierarchy();
		}
		else
		{
			Widget* widget = this;
			while (widget != nullptr && !widget->placement_hierarchy_update_required)
			{
				widget->placement_hierarchy_update_required = true;
				widget = widget->GetParentWidget();
			}
		}
	}

	void Widget::SetPlacement(aabox2 const& in_placement)
	{
		placement = ApplyModifiersToPlacement(in_placement);
		placement_hierarchy_update_required = false;
	}

	aabox2 Widget::ApplyModifiersToPlacement(aabox2 const& in_placement) const
	{
		aabox2 result;

		result.size.x = std::max(0.0f, in_placement.size.x - (float)layout.padding.left - (float)layout.padding.right);
		result.size.y = std::max(0.0f, in_placement.size.y - (float)layout.padding.top - (float)layout.padding.bottom);

		result.position.x = (result.size.x == 0)?
			0:
			in_placement.position.x + (float)layout.padding.left;

		result.position.y = (result.size.y == 0) ?
			0 :
			in_placement.position.y + (float)layout.padding.bottom;

		if (layout.aspect_ratio > 0.0f)
			result = SetBoxAspect(result, layout.aspect_ratio, SetBoxAspectMethod::SHRINK_BOX);

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

	bool Widget::OnDraw(GPURenderContext* render_context, GPUProgramProviderInterface const* uniform_provider, WindowDrawParams const& draw_params)
	{
		for (auto& child : child_widgets)
			child->OnDraw(render_context, uniform_provider, draw_params);
		return true;
	}

	bool Widget::DoTick(float delta_time)
	{
		bool result = false;
		for (auto& child : child_widgets)
			result |= child->Tick(delta_time);
		return result;
	}

	void Widget::AttachChild(Widget* widget, bool immediate_update)
	{
		assert(widget != nullptr);
		assert(widget->GetParentWidget() == nullptr);
		assert(dynamic_cast<WindowRootWidget*>(widget) == nullptr);

		widget->parent = this;
		widget->OnAttachedToParent(this);

		UpdatePlacementHierarchy(immediate_update);
	}

	void Widget::DetachChild(Widget* widget, bool immediate_update)
	{
		assert(widget != nullptr);
		assert(widget->GetParentWidget() == this);

		widget->parent = nullptr;
		widget->OnDetachedFromParent(this);

		UpdatePlacementHierarchy(immediate_update);
	}

	Widget* Widget::GetChildWidgetUnderMouse(glm::vec2 const& position)
	{
		if (IsPointInside(position, placement))
		{
			for (auto& child : child_widgets)
				if (child != nullptr)
					if (Widget* result = child->GetChildWidgetUnderMouse(position))
						return result;
			return this;
		}
		return nullptr;
	}

	Widget const* Widget::GetChildWidgetUnderMouse(glm::vec2 const& position) const
	{
		if (IsPointInside(position, placement))
		{
			for (auto& child : child_widgets)
				if (child != nullptr)
					if (Widget const* result = child->GetChildWidgetUnderMouse(position))
						return result;
			return this;
		}
		return nullptr;
	}

	WidgetSurfaceRequirement Widget::GetSurfaceRequirement() const
	{
		WidgetSurfaceRequirement result;

#if 0
		WidgetDesiredPlacementInfo result;
		(WidgetDesiredSize&)result = GetDesiredSize();
		result.fill_x = (layout.fill_mode_x == WidgetFillMode::FILL);
		result.fill_y = (layout.fill_mode_y == WidgetFillMode::FILL);
#endif
		return result;
	}

}; // namespace chaos
