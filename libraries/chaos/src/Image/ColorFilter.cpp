#include "chaos/Chaos.h"

namespace chaos
{
	bool ColorFilter::Filter(glm::vec4 const& color) const
	{
		float d2 = glm::distance2(color * color_mask, color_reference * color_mask);
		return Compare(distance_operator, d2, distance * distance);
	}

	bool SaveIntoJSON(nlohmann::json& json, ColorFilter const& src)
	{
		if (!json.is_object())
			json = nlohmann::json::object();
		JSONTools::SetAttribute(json, "distance_operator", src.distance_operator);
		JSONTools::SetAttribute(json, "distance", src.distance);
		JSONTools::SetAttribute(json, "color_reference", src.color_reference);
		JSONTools::SetAttribute(json, "color_mask", src.color_mask);
		return true;
	}

	bool LoadFromJSON(nlohmann::json const& json, ColorFilter& dst)
	{
		if (!json.is_object())
			return false;
		JSONTools::GetAttribute(json, "distance_operator", dst.distance_operator);
		JSONTools::GetAttribute(json, "distance", dst.distance);
		JSONTools::GetAttribute(json, "color_reference", dst.color_reference);
		JSONTools::GetAttribute(json, "color_mask", dst.color_mask);
		return true;
	}

}; // namespace chaos

