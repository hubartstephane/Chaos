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

	void ViewportGridLayout::SetMaxViewportCount(size_t in_size, bool update_placements)
	{
		size = in_size;
		if (update_placements)
			UpdateWindowViewportPlacements();
	}

	void ViewportGridLayout::SetOrientation(Orientation in_orientation, bool update_placements)
	{
		orientation = in_orientation;
		if (update_placements)
			UpdateWindowViewportPlacements();
	}

	void ViewportGridLayout::SetHorizontalFillMode(ViewportGridHorizontalFillMode in_mode, bool update_placements)
	{
		horizontal_fill_mode = in_mode;
		if (update_placements)
			UpdateWindowViewportPlacements();
	}

	void ViewportGridLayout::SetVerticalFillMode(ViewportGridVerticalFillMode in_mode, bool update_placements)
	{
		vertical_fill_mode = in_mode;
		if (update_placements)
			UpdateWindowViewportPlacements();
	}

	ViewportPlacement ViewportGridLayout::ComputeViewportPlacement(Viewport* viewport, glm::ivec2 const& window_size, size_t viewport_index, size_t viewport_count) const
	{
		ViewportPlacement result;

		glm::ivec2 window_size_copy = window_size;
		if (orientation == Orientation::VERTICAL)
			std::swap(window_size_copy.x, window_size_copy.y);

		glm::vec2 ws = auto_cast_vector(window_size_copy);

		bool reverse_horizontal = (horizontal_fill_mode == ViewportGridHorizontalFillMode::RIGHT_TO_LEFT);
		bool reverse_vertical = (vertical_fill_mode == ViewportGridVerticalFillMode::TOP_TO_BOTTOM);

		if (orientation == Orientation::VERTICAL)
			std::swap(reverse_horizontal, reverse_vertical);

		// compute the result as if the orientation was horizontal
		if (size <= 0 || viewport_count <= size) // a single line/row
		{
			size_t x = viewport_index;
			if (reverse_horizontal)
				x = (viewport_count - 1) - x;

			float p = float(x);
			float next_p = float(x + 1);

			float viewport_size = ws.x / float(viewport_count);

			int position = int(p * viewport_size);
			int next_position = int(next_p * viewport_size);

			result.size.y = window_size_copy.y;
			result.size.x = next_position - position;

			result.position.y = 0;
			result.position.x = position;
		}
		else
		{
			size_t line_count = (viewport_count + size - 1) / size;
			size_t x = viewport_index % size;
			size_t y = viewport_index / size;

			if (reverse_horizontal)
				x = (size - 1) - x;
			if (reverse_vertical)
				y = (line_count - 1) - y;

			size_t viewport_on_line = (y != line_count - 1)? // the number of viewports for this line
				size :
				viewport_count - y * size;

			glm::vec2 p      = {float(x), float(y)};
			glm::vec2 next_p = {float(x + 1), float(y + 1) };
			glm::vec2 grid   = {float(size), float(line_count)};

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
