#include <chaos/Hotpoint.h>

namespace chaos
{

	std::vector<std::pair<HotpointType, char const *>> const Hotpoint::hotpoint_encoding =
	{
		{ HotpointType::TOP, "top" },
		{ HotpointType::BOTTOM, "bottom" },
		{ HotpointType::LEFT, "left" },
		{ HotpointType::RIGHT, "right" },
		{ HotpointType::TOP_LEFT, "top-left" },
		{ HotpointType::TOP_RIGHT, "top-right" },
		{ HotpointType::BOTTOM_LEFT, "bottom-left" },
		{ HotpointType::BOTTOM_RIGHT, "bottom-right" },
		{ HotpointType::CENTER, "center" }
	};

	glm::vec2 Hotpoint::Convert(glm::vec2 const & hotpoint, glm::vec2 const & size, HotpointType initial_hotpoint_type, HotpointType final_hotpoint_type)
	{
		if (initial_hotpoint_type == final_hotpoint_type)
			return hotpoint;

		static float const offset_factor[] = { -0.5f, 0.0f, -1.0f, 0.0f };

		int initial_h_part = ((int)initial_hotpoint_type >> 0) & 3;
		int initial_v_part = ((int)initial_hotpoint_type >> 2) & 3;

		int final_h_part = ((int)final_hotpoint_type >> 0) & 3;
		int final_v_part = ((int)final_hotpoint_type >> 2) & 3;

		assert(initial_h_part != 3); // not both LEFT and RIGHT, nor TOP and BOTTOM at the same time
		assert(initial_v_part != 3);
		assert(final_h_part != 3); // not both LEFT and RIGHT, nor TOP and BOTTOM at the same time
		assert(final_v_part != 3);

		glm::vec2 result = hotpoint;
		result.x += size.x * (offset_factor[initial_h_part] - offset_factor[final_h_part]);
		result.y += size.y * (offset_factor[initial_v_part] - offset_factor[final_v_part]);
		return result;

	}

	glm::vec2 Hotpoint::ConvertToBottomLeft(glm::vec2 const & hotpoint, glm::vec2 const & size, HotpointType hotpoint_type)
	{
		static float const offset_factor[] = { -0.5f, 0.0f, -1.0f, 0.0f };

		int h_part = ((int)hotpoint_type >> 0) & 3;
		int v_part = ((int)hotpoint_type >> 2) & 3;

		assert(h_part != 3); // not both LEFT and RIGHT, nor TOP and BOTTOM at the same time
		assert(v_part != 3);

		glm::vec2 result = hotpoint;
		result.x += size.x * offset_factor[h_part];
		result.y += size.y * offset_factor[v_part];
		return result;
	}
}; // namespace chaos