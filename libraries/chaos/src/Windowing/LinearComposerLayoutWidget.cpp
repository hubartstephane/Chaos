#include "chaos/ChaosPCH.h"
#include "chaos/ChaosInternals.h"

namespace chaos
{
	void LinearComposerLayoutWidget::SetPlacement(aabox2 const& in_placement)
	{
		LinearComposerWidget::SetPlacement(in_placement);

		size_t count = child_widgets.size();
		if (count > 0)
		{
			LinearComposerLayout linear_layout = GetComposerLayout();
			for (size_t i = 0; i < count; ++i)
				if (Widget* child = child_widgets[i].get())
					child->SetPlacement(linear_layout.ComputePlacement(placement, i, count));
		}
	}

}; // namespace chaos
