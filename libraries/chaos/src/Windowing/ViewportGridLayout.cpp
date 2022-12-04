#include "chaos/ChaosPCH.h"
#include "chaos/ChaosInternals.h"

namespace chaos
{
	void ViewportGridLayout::SetSettings(ViewportGridLayoutSettings const& in_settings, bool update_placement_hierarchy)
	{
		settings = in_settings;
		if (update_placement_hierarchy)
			UpdatePlacementHierarchy();
	}

	void ViewportGridLayout::SetMaxViewportCount(size_t in_max_count, bool update_placement_hierarchy)
	{
		settings.max_count = in_max_count;
		if (update_placement_hierarchy)
			UpdatePlacementHierarchy();
	}

	void ViewportGridLayout::SetOrientation(Orientation in_orientation, bool update_placement_hierarchy)
	{
		settings.orientation = in_orientation;
		if (update_placement_hierarchy)
			UpdatePlacementHierarchy();
	}

	void ViewportGridLayout::SetHorizontalInsertionMode(ViewportGridInsertionMode in_insertion_mode, bool update_placement_hierarchy)
	{
		settings.horizontal_insertion_mode = in_insertion_mode;
		if (update_placement_hierarchy)
			UpdatePlacementHierarchy();
	}

	void ViewportGridLayout::SetVerticalInsertionMode(ViewportGridInsertionMode in_insertion_mode, bool update_placement_hierarchy)
	{
		settings.vertical_insertion_mode = in_insertion_mode;
		if (update_placement_hierarchy)
			UpdatePlacementHierarchy();
	}

	void ViewportGridLayout::SetFillMode(ViewportGridFillMode in_fill_mode, bool update_placement_hierarchy)
	{
		settings.fill_mode = in_fill_mode;
		if (update_placement_hierarchy)
			UpdatePlacementHierarchy();
	}



#if 0

	aabox2 ViewportGridLayout::ComputeViewportPlacement(Viewport* viewport, glm::ivec2 const& window_size, size_t viewport_index, size_t viewport_count) const
	{
		aabox2 result;

		glm::ivec2 window_size_copy = window_size;
		if (settings.orientation == Orientation::VERTICAL)
			std::swap(window_size_copy.x, window_size_copy.y);

		glm::vec2 ws = auto_cast_vector(window_size_copy);

		bool reverse_horizontal = (settings.horizontal_insertion_mode == ViewportGridHorizontalFillMode::RIGHT_TO_LEFT);
		bool reverse_vertical = (settings.vertical_insertion_mode == ViewportGridVerticalFillMode::TOP_TO_BOTTOM);

		if (settings.orientation == Orientation::VERTICAL)
			std::swap(reverse_horizontal, reverse_vertical);

		// compute the result as if the orientation was horizontal
		if (settings.size <= 0) // a single line/row
		{
			size_t x = viewport_index;
			if (reverse_horizontal)
				x = (viewport_count - 1) - x;

			float p = float(x);
			float next_p = float(x + 1);

			float cell_size = ws.x / float(viewport_count);

			int position = int(p * cell_size);
			int next_position = int(next_p * cell_size);

			result.size.y = window_size_copy.y;
			result.size.x = next_position - position;

			result.position.y = 0;
			result.position.x = position;
		}
		else
		{
			size_t line_count = (viewport_count + settings.size - 1) / settings.size;
			size_t x = viewport_index % settings.size;
			size_t y = viewport_index / settings.size;

			size_t cell_on_line = (y == line_count - 1) ? // the number of cells for this line
				viewport_count - y * settings.size :
				settings.size;

			glm::vec2 cell_size = { 0.0f, 0.0f };
			int offset = 0;

			if (settings.fill_mode == ViewportGridFillMode::EXPANDED)
			{
				cell_size = ws / glm::vec2(float(cell_on_line), float(line_count));
			}
			else
			{
				cell_size = ws / glm::vec2(float(settings.size), float(line_count));
				if (settings.fill_mode == ViewportGridFillMode::UNIFORM_CENTERED)
					offset = int((ws.x - cell_size.x * cell_on_line) * 0.5f);
			}

			glm::vec2 p = { float(x), float(y) };
			glm::ivec2 position = auto_cast_vector(p * cell_size);
			glm::ivec2 next_position = auto_cast_vector(p * cell_size + cell_size);

			if (reverse_horizontal)
			{
				std::swap(position.x, next_position.x);
				position.x = window_size_copy.x - position.x;
				next_position.x = window_size_copy.x - next_position.x;
				offset = -offset;
			}

			if (reverse_vertical)
			{
				std::swap(position.y, next_position.y);
				position.y = window_size_copy.y - position.y;
				next_position.y = window_size_copy.y - next_position.y;
			}

			result.size = next_position - position;
			result.position = position + glm::ivec2(offset, 0);
		}

		// correct orientation
		if (settings.orientation == Orientation::VERTICAL)
		{
			std::swap(result.position.x, result.position.y);
			std::swap(result.size.x, result.size.y);
		}
		return result;
	}

#endif

}; // namespace chaos
