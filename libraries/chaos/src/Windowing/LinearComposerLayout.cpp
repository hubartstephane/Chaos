#include "chaos/ChaosPCH.h"
#include "chaos/ChaosInternals.h"

namespace chaos
{
	// =====================================================================
	// LinearComposerLayout implementation
	// =====================================================================

	aabox2 LinearComposerLayout::ComputePlacement(aabox2 const& placement, size_t index, size_t count) const
	{
		aabox2 result;

		bool reverse_horizontal = (horizontal_mode == LinearComposerLayoutMode::REVERSED);
		bool reverse_vertical = (vertical_mode == LinearComposerLayoutMode::REVERSED);

		aabox2 placement_copy = placement;
		if (orientation == Orientation::VERTICAL)
		{
			std::swap(placement_copy.position.x, placement_copy.position.y);
			std::swap(placement_copy.size.x, placement_copy.size.y);
			std::swap(reverse_horizontal, reverse_vertical);
		}

		// compute the result as if the orientation was horizontal
		if (max_count <= 0) // a single line/row
		{
			if (reverse_horizontal)
				index = (count - 1) - index;

			float cell_size = placement_copy.size.x / float(count);

			result.size.x = cell_size;
			result.size.y = placement_copy.size.y;

			result.position.x = float(index) * cell_size;
			result.position.y = 0;

			result.position += placement_copy.position;
		}
		else
		{
#if 0
			size_t line_count = (count + max_count - 1) / max_count;
			size_t x = index % max_count;
			size_t y = index / max_count;

			size_t cell_on_line = (y == line_count - 1) ? // the number of cells for this line
				count - y * max_count :
				max_count;

			glm::vec2 cell_size = { 0.0f, 0.0f };
			int offset = 0;

			if (fill_mode == LinearComposerLayoutFillMode::EXPANDED)
			{
				cell_size = placement_copy.size / glm::vec2(float(cell_on_line), float(line_count));
			}
			else
			{
				cell_size = placement_copy.size / glm::vec2(float(max_count), float(line_count));
				if (fill_mode == LinearComposerLayoutFillMode::UNIFORM_CENTERED)
					offset = int((placement_copy.size.x - cell_size.x * cell_on_line) * 0.5f);
			}

			glm::vec2 p = { float(x), float(y) };
			glm::ivec2 position = auto_cast_vector(p * cell_size);
			glm::ivec2 next_position = auto_cast_vector(p * cell_size + cell_size);

			if (reverse_horizontal)
			{
				std::swap(position.x, next_position.x);
				position.x = placement_copy.size.x - position.x;
				next_position.x = placement_copy.size.x - next_position.x;
				offset = -offset;
			}

			if (reverse_vertical)
			{
				std::swap(position.y, next_position.y);
				position.y = placement_copy.size.y - position.y;
				next_position.y = placement_copy.size.y - next_position.y;
			}

			result.size = next_position - position;
			result.position = position + glm::ivec2(offset, 0);

#endif
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
