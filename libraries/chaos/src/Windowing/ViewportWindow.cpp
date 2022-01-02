#include <chaos/Chaos.h>

namespace chaos
{

	size_t ViewportWindow::GetViewportCount() const
	{
		return viewports.size();
	}

	AutoCastable<Viewport> ViewportWindow::GetViewport(size_t index)
	{
		return viewports[index].get();
	}

	AutoConstCastable<Viewport> ViewportWindow::GetViewport(size_t index) const
	{
		return viewports[index].get();
	}

	ViewportLayout* ViewportWindow::GetViewportLayout()
	{
		return viewport_layout.get();
	}

	ViewportLayout const* ViewportWindow::GetViewportLayout() const
	{
		return viewport_layout.get();
	}

}; // namespace chaos
