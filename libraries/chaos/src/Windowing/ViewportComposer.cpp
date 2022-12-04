#include "chaos/ChaosPCH.h"
#include "chaos/ChaosInternals.h"

namespace chaos
{

	void ViewportComposer::SetViewportLayout(ViewportLayout* in_viewport_layout, bool update_placement_hierarchy)
	{
		if (in_viewport_layout != viewport_layout)
		{
			if (viewport_layout != nullptr)
				viewport_layout->viewport_composer = nullptr;
			viewport_layout = in_viewport_layout;
			if (viewport_layout != nullptr)
				viewport_layout->viewport_composer = this;

			if (update_placement_hierarchy)
				UpdatePlacementHierarchy();
		}
	}

	AutoCastable<ViewportLayout> ViewportComposer::GetViewportLayout()
	{
		return viewport_layout.get();
	}

	AutoConstCastable<ViewportLayout> ViewportComposer::GetViewportLayout() const
	{
		return viewport_layout.get();
	}

	void ViewportComposer::AddViewport(Viewport* viewport, bool update_placement_hierarchy)
	{
		assert(viewport != nullptr);
		assert(viewport->GetViewportServer() == nullptr); // not already in use

		AttachViewport(viewport);
		if (update_placement_hierarchy)
			UpdatePlacementHierarchy();
	}

	void ViewportComposer::RemoveViewport(Viewport* viewport, bool keep_empty_place, bool update_placement_hierarchy)
	{
		assert(viewport != nullptr);
		assert(viewport->GetViewportServer() == this); // used by this

		auto it = std::find(viewports.begin(), viewports.end(), viewport);
		if (keep_empty_place) // remove the element, but relace it with a empty zone
			*it = nullptr;
		else
			viewports.erase(it);

		DetachViewport(viewport);
		if (update_placement_hierarchy)
			UpdatePlacementHierarchy();
	}

	void ViewportComposer::SetViewportPlacement(WidgetPlacement const& in_placement)
	{
		Viewport::SetViewportPlacement(in_placement);
		if (viewport_layout != nullptr)
			viewport_layout->SetAllViewportPlacements(placement, viewports);
	}

}; // namespace chaos
