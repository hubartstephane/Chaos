#include "chaos/ChaosPCH.h"
#include "chaos/ChaosInternals.h"

namespace chaos
{
	// =====================================================================
	// SingleDirectionBoxWidget implementation
	// =====================================================================

	void SingleDirectionBoxWidget::AddChildWidget(Widget* widget)
	{
		assert(widget != nullptr);
		assert(widget->GetParentWidget() == nullptr);
		assert(dynamic_cast<WindowRootWidget*>(widget) == nullptr);

		child_widgets.push_back(widget);
		AttachChild(widget);
	}

	void SingleDirectionBoxWidget::RemoveChildWidget(Widget* widget)
	{
		assert(widget != nullptr);
		assert(widget->GetParentWidget() == this);

	//auto it = std::find(child_widgets.begin(), child_widgets.end(), widget);
	//	child_widgets.erase(std::remove(child_widgets.begin(), child_widgets.end(), it), child_widgets.end());

		DetachChild(widget);

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
