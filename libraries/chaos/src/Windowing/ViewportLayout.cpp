#include <chaos/Chaos.h>

namespace chaos
{
	void ViewportLayout::UpdateWindowViewportPlacements()
	{
		if (window != nullptr)
			window->ComputeViewportPlacements(window->GetWindowSize());
	}

	ViewportPlacement ViewportLayout::ComputeViewportPlacement(Viewport * viewport, glm::ivec2 const& window_size, size_t viewport_index, size_t viewport_count) const
	{
		assert(0);
		return {};
	}

	void ViewportGridLayout::SetMaxViewportCount(size_t in_max_viewport_count, bool update_placements)
	{
		max_viewport_count = in_max_viewport_count;
		if (update_placements)
			UpdateWindowViewportPlacements();
	}

	void ViewportGridLayout::SetOrientation(Orientation in_orientation, bool update_placements)
	{
		orientation = in_orientation;
		if (update_placements)
			UpdateWindowViewportPlacements();
	}

	ViewportPlacement ViewportGridLayout::ComputeViewportPlacement(Viewport* viewport, glm::ivec2 const& window_size, size_t viewport_index, size_t viewport_count) const
	{
		int major_axis = (orientation == Orientation::HORIZONTAL) ? 0 : 1;
		int minor_axis = 1 - major_axis;

		ViewportPlacement result;

		if (max_viewport_count <= 0 || viewport_count <= max_viewport_count) // a single line/row
		{
			result.size[minor_axis] = window_size[minor_axis];
			result.size[major_axis] = window_size[major_axis] / int(viewport_count);

			result.position[minor_axis] = 0;
			result.position[major_axis] = int(viewport_index) * result.size[major_axis];

			if (viewport_index == viewport_count - 1)
			{

			}


		}
		else
		{
			assert(0);
#if 0
;			int line_count = max_viewport_count;
			int row_count  = viewport_count / max_viewport_count;

			int X = viewport_index % line_count;
			int Y = viewport_index / line_count;




			result.position = window_size[]
			result.size[minor_axis] = window_size[minor_axis];

			float f = window_size[major_axis] / (float)viewport_count;

			window_size[major_axis] / viewport
#endif
		}




		return result;
	}

}; // namespace chaos
