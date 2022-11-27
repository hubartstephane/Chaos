#include "chaos/ChaosPCH.h"
#include "chaos/ChaosInternals.h"

namespace chaos
{

	void ViewportComposer::SetViewportLayout(ViewportLayout* in_viewport_layout)
	{

#if 0

		if (viewport_layout != nullptr)
			viewport_layout->window = nullptr;
		viewport_layout = in_viewport_layout;
		if (viewport_layout != nullptr)
			viewport_layout->window = this;
		ComputeViewportPlacements(GetWindowSize());
#endif
	}

	void ViewportComposer::ComputeViewportPlacements(glm::ivec2 size)
	{
		if (viewport_layout != nullptr)
		{
			size_t count = viewports.size();
			for (size_t i = 0; i < count; ++i)
				if (Viewport* viewport = viewports[i].get())
					viewport->SetViewportPlacement(viewport_layout->ComputeViewportPlacement(viewport, size, i, count));
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




}; // namespace chaos
