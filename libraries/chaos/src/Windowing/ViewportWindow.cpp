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

	AutoCastable<Viewport> ViewportWindow::FindViewport(ObjectRequest request)
	{
		return request.FindObject(viewports);
	}

	AutoConstCastable<Viewport> ViewportWindow::FindViewport(ObjectRequest request) const
	{
		return request.FindObject(viewports);
	}

	ViewportLayout* ViewportWindow::GetViewportLayout()
	{
		return viewport_layout.get();
	}

	ViewportLayout const* ViewportWindow::GetViewportLayout() const
	{
		return viewport_layout.get();
	}

	void ViewportWindow::OnWindowResize(glm::ivec2 size)
	{
		ComputeViewportSurfaces(size);
	}

	void ViewportWindow::AddViewport(Viewport* viewport, bool compute_viewport_surfaces)
	{
		assert(viewport != nullptr);
		assert(viewport->window != nullptr);

		viewport->window = this;
		viewports.push_back(viewport);

		if (compute_viewport_surfaces)
			ComputeViewportSurfaces(GetWindowSize());
	}

	void ViewportWindow::RemoveViewport(Viewport* viewport, bool compute_viewport_surfaces)
	{
		assert(viewport != nullptr);
		assert(viewport->window == this);

		viewport->window = nullptr;
		viewports.erase(std::find(viewports.begin(), viewports.end(), viewport));

		if (compute_viewport_surfaces)
			ComputeViewportSurfaces(GetWindowSize());
	}

	void ViewportWindow::ComputeViewportSurfaces(glm::ivec2 size)
	{
		if (viewport_layout != nullptr)
		{
			size_t count = viewports.size();
			for (size_t i = 0; i < count; ++i)
				viewport_layout->ComputeViewportSurface(size, i, count);
		}
	}

	bool ViewportWindow::OnDraw(GPURenderer* renderer, box2 const& viewport, glm::ivec2 window_size)
	{



		return true;
	}

}; // namespace chaos
