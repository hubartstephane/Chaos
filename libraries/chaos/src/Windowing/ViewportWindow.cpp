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
		ComputeViewportPlacements(size);
	}

	void ViewportWindow::AddViewport(Viewport* viewport, bool compute_viewport_surfaces)
	{
		assert(viewport != nullptr);
		assert(viewport->window != nullptr);

		viewport->window = this;
		viewports.push_back(viewport);

		if (compute_viewport_surfaces)
			ComputeViewportPlacements(GetWindowSize());
	}

	void ViewportWindow::RemoveViewport(Viewport* viewport, bool compute_viewport_surfaces, bool keep_empty_place)
	{
		assert(viewport != nullptr);
		assert(viewport->window == this);

		viewport->window = nullptr;

		auto it = std::find(viewports.begin(), viewports.end(), viewport);
		if (keep_empty_place) // remove the element, but relace it with a empty zone
			*it = nullptr;
		else
			viewports.erase(it);

		if (compute_viewport_surfaces)
			ComputeViewportPlacements(GetWindowSize());
	}

	void ViewportWindow::ComputeViewportPlacements(glm::ivec2 size)
	{
		if (viewport_layout != nullptr)
		{
			size_t count = viewports.size();
			for (size_t i = 0; i < count; ++i)
				if (Viewport* viewport = viewports[i].get())
					viewport->SetViewportPlacement(viewport_layout->ComputeViewportPlacement(viewport, size, i, count));
		}
	}

	bool ViewportWindow::OnDraw(GPURenderer* renderer, box2 const& viewport, glm::ivec2 window_size)
	{
		for (auto& viewport : viewports)
		{
			if (viewport != nullptr)
			{


			}
		}
		return true;
	}

}; // namespace chaos
