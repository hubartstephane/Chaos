#include "chaos/ChaosPCH.h"
#include "chaos/ChaosInternals.h"

namespace chaos
{
	static chaos::EnumTools::EnumMetaData<Hotpoint> const Hotpoint_metadata =
	{
		{ Hotpoint::TOP, "top" },
		{ Hotpoint::BOTTOM, "bottom" },
		{ Hotpoint::LEFT, "left" },
		{ Hotpoint::RIGHT, "right" },
		{ Hotpoint::TOP_LEFT, "top-left" },
		{ Hotpoint::TOP_RIGHT, "top-right" },
		{ Hotpoint::BOTTOM_LEFT, "bottom-left" },
		{ Hotpoint::BOTTOM_RIGHT, "bottom-right" },
		{ Hotpoint::CENTER, "center" }
	};

	CHAOS_IMPLEMENT_ENUM_METHOD(Hotpoint, Hotpoint_metadata);
	CHAOS_IMPLEMENT_ENUM_FLAG_METHOD(Hotpoint);

	glm::vec2 ConvertHotpoint(glm::vec2 const & pt, glm::vec2 const & size, Hotpoint initial_hotpoint, Hotpoint final_hotpoint)
	{
		if (initial_hotpoint == final_hotpoint)
			return pt;

		static constexpr float offset_factor[] = { -0.5f, 0.0f, -1.0f, 0.0f };

		int initial_h_part = (int(initial_hotpoint) >> 0) & 3;
		int initial_v_part = (int(initial_hotpoint) >> 2) & 3;

		int final_h_part = (int(final_hotpoint) >> 0) & 3;
		int final_v_part = (int(final_hotpoint) >> 2) & 3;

		assert(initial_h_part != 3); // not both LEFT and RIGHT, nor TOP and BOTTOM at the same time
		assert(initial_v_part != 3);
		assert(final_h_part != 3); // not both LEFT and RIGHT, nor TOP and BOTTOM at the same time
		assert(final_v_part != 3);

		glm::vec2 result = pt;
		result.x += size.x * (offset_factor[initial_h_part] - offset_factor[final_h_part]);
		result.y += size.y * (offset_factor[initial_v_part] - offset_factor[final_v_part]);
		return result;

	}

	glm::vec2 ConvertHotpointToBottomLeft(glm::vec2 const & pt, glm::vec2 const & size, Hotpoint hotpoint)
	{
		static constexpr float offset_factor[] = { -0.5f, 0.0f, -1.0f, 0.0f };

		int h_part = (int(hotpoint) >> 0) & 3;
		int v_part = (int(hotpoint) >> 2) & 3;

		assert(h_part != 3); // not both LEFT and RIGHT, nor TOP and BOTTOM at the same time
		assert(v_part != 3);

		glm::vec2 result = pt;
		result.x += size.x * offset_factor[h_part];
		result.y += size.y * offset_factor[v_part];
		return result;
	}

}; // namespace chaos