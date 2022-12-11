#include "chaos/ChaosPCH.h"
#include "chaos/ChaosInternals.h"

namespace chaos
{

	SingleDirectionBoxWidgetInsertData::SingleDirectionBoxWidgetInsertData(SingleDirectionBoxWidgetInsertType in_type):
		type(in_type)
	{}

	SingleDirectionBoxWidgetInsertData::SingleDirectionBoxWidgetInsertData(size_t in_position):
		type(SingleDirectionBoxWidgetInsertType::POSITIONAL),
		position(in_position)
	{}

	// =====================================================================
	// SingleDirectionBoxWidget implementation
	// =====================================================================

	void SingleDirectionBoxWidget::AddChildWidget(Widget* widget, SingleDirectionBoxWidgetInsertData insert_data, bool immediate_update)
	{
		assert(widget != nullptr);
		assert(widget->GetParentWidget() == nullptr);
		assert(dynamic_cast<WindowRootWidget*>(widget) == nullptr);

		if (insert_data.type == SingleDirectionBoxWidgetInsertType::END)
		{
			child_widgets.push_back(widget);
		}
		else if (insert_data.type == SingleDirectionBoxWidgetInsertType::START)
		{
			child_widgets.insert(child_widgets.begin(), widget);
		}
		else
		{
			if (insert_data.position >= child_widgets.size())
			{
				child_widgets.push_back(widget);
			}
			else
			{
				if (insert_data.type == SingleDirectionBoxWidgetInsertType::POSITIONAL)
				{
					child_widgets.insert(child_widgets.begin() + insert_data.position, widget);
				}
				else if (insert_data.type == SingleDirectionBoxWidgetInsertType::POSITIONAL_REPLACE)
				{
					// we want to always keep list of children coherent
					// that's why we have a
					//   child_widgets[insert_data.position] = nullptr
					// before
					//   child_widgets[insert_data.position] = widget;
					shared_ptr<Widget> prevent_destruction = child_widgets[insert_data.position];
					child_widgets[insert_data.position] = nullptr;
					if (prevent_destruction != nullptr)
						DetachChild(prevent_destruction.get(), immediate_update);
					child_widgets[insert_data.position] = widget;
				}
			}
		}
		AttachChild(widget, immediate_update);
	}

	void SingleDirectionBoxWidget::RemoveChildWidget(Widget* widget, bool immediate_update)
	{
		assert(widget != nullptr);
		assert(widget->GetParentWidget() == this);

		shared_ptr<Widget> prevent_destruction = widget;

		auto it = std::find(child_widgets.begin(), child_widgets.end(), widget);
		child_widgets.erase(it);

		DetachChild(widget, immediate_update);
	}

	// =====================================================================
	// OverlayWidget implementation
	// =====================================================================

	void OverlayWidget::SetPlacement(aabox2 const& in_placement)
	{
		Widget::SetPlacement(in_placement);
		for (auto& child : child_widgets)
			if (child != nullptr)
				child->SetPlacement(placement); // XXX: use member instead of incoming parameter, because the member take into account the overlay layout
	}

}; // namespace chaos
