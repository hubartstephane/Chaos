#include "chaos/ChaosPCH.h"
#include "chaos/ChaosInternals.h"

namespace chaos
{
	void ViewportGridLayout::SetSettings(ViewportGridLayoutSettings const& in_settings, bool update_placements)
	{
		settings = in_settings;
		if (update_placements)
			UpdateWindowViewportPlacements();
	}

	void ViewportGridLayout::SetMaxViewportCount(size_t in_size, bool update_placements)
	{
		settings.size = in_size;
		if (update_placements)
			UpdateWindowViewportPlacements();
	}

	void ViewportGridLayout::SetOrientation(Orientation in_orientation, bool update_placements)
	{
		settings.orientation = in_orientation;
		if (update_placements)
			UpdateWindowViewportPlacements();
	}

	void ViewportGridLayout::SetHorizontalFillMode(ViewportGridHorizontalFillMode in_mode, bool update_placements)
	{
		settings.horizontal_fill_mode = in_mode;
		if (update_placements)
			UpdateWindowViewportPlacements();
	}

	void ViewportGridLayout::SetVerticalFillMode(ViewportGridVerticalFillMode in_mode, bool update_placements)
	{
		settings.vertical_fill_mode = in_mode;
		if (update_placements)
			UpdateWindowViewportPlacements();
	}

	void ViewportGridLayout::SetMode(ViewportGridMode in_mode, bool update_placements)
	{
		settings.mode = in_mode;
		if (update_placements)
			UpdateWindowViewportPlacements();
	}

	ViewportPlacement ViewportGridLayout::ComputeViewportPlacement(Viewport* viewport, glm::ivec2 const& window_size, size_t viewport_index, size_t viewport_count) const
	{
		ViewportPlacement result;

		glm::ivec2 window_size_copy = window_size;
		if (settings.orientation == Orientation::VERTICAL)
			std::swap(window_size_copy.x, window_size_copy.y);

		glm::vec2 ws = auto_cast_vector(window_size_copy);

		bool reverse_horizontal = (settings.horizontal_fill_mode == ViewportGridHorizontalFillMode::RIGHT_TO_LEFT);
		bool reverse_vertical = (settings.vertical_fill_mode == ViewportGridVerticalFillMode::TOP_TO_BOTTOM);

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

			if (settings.mode == ViewportGridMode::EXPANDED)
			{
				cell_size = ws / glm::vec2(float(cell_on_line), float(line_count));
			}
			else
			{
				cell_size = ws / glm::vec2(float(settings.size), float(line_count));
				if (settings.mode == ViewportGridMode::UNIFORM_CENTERED)
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

}; // namespace chaos
