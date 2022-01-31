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
		ViewportPlacement result;

		glm::vec2 ws = auto_cast_vector(window_size);
		if (orientation == Orientation::VERTICAL)
			std::swap(ws.x, ws.y);

		// compute the result as if the orientation was horizontal
		if (max_viewport_count <= 0 || viewport_count <= max_viewport_count) // a single line/row
		{
			float p = float(viewport_index);
			float next_p = float(viewport_index + 1);

			float viewport_size = ws.x / float(viewport_count);

			int position = int(p * viewport_size);
			int next_position = int(next_p * viewport_size);

			result.size.y = int(ws.y);
			result.size.x = next_position - position;

			result.position.y = 0;
			result.position.x = position;
		}
		else
		{
			size_t line_count = (viewport_count + max_viewport_count - 1) / max_viewport_count;
			size_t x = viewport_index % max_viewport_count;
			size_t y = viewport_index / max_viewport_count;

			size_t viewport_on_line = (y != line_count - 1)? // the number of viewports for this line
				max_viewport_count :
				viewport_count - y * max_viewport_count;

			glm::vec2 p      = {float(x), float(y)};
			glm::vec2 next_p = {float(x + 1), float(y + 1) };
			glm::vec2 grid   = {float(max_viewport_count), float(line_count)};

			glm::vec2 viewport_size = ws / grid;

			glm::ivec2 position = auto_cast_vector(p * viewport_size);
			glm::ivec2 next_position = auto_cast_vector(next_p * viewport_size);

			result.position = position;
			result.size = next_position - position;


		}

		// correct orientation
		if (orientation == Orientation::VERTICAL)
		{
			std::swap(result.position.x, result.position.y);
			std::swap(result.size.x, result.size.y);
		}
		return result;
	}

}; // namespace chaos
