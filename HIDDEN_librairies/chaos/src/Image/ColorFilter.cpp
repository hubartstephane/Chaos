#include "chaos/ChaosPCH.h"
#include "chaos/ChaosInternals.h"

namespace chaos
{
	bool ColorFilter::Filter(glm::vec4 const& color) const
	{
		float d2 = glm::distance2(color * color_mask, color_reference * color_mask);
		return Compare(distance_operator, d2, distance * distance);
	}

	bool DoSaveIntoJSON(nlohmann::json * json, ColorFilter const& src)
	{
		if (!PrepareSaveObjectIntoJSON(json))
			return false;
		JSONTools::SetAttribute(json, "distance_operator", src.distance_operator);
		JSONTools::SetAttribute(json, "distance", src.distance);
		JSONTools::SetAttribute(json, "color_reference", src.color_reference);
		JSONTools::SetAttribute(json, "color_mask", src.color_mask);
		return true;
	}

	bool DoLoadFromJSON(JSONReadConfiguration config, ColorFilter& dst)
	{
		JSONTools::GetAttribute(config, "distance_operator", dst.distance_operator);
		JSONTools::GetAttribute(config, "distance", dst.distance);
		JSONTools::GetAttribute(config, "color_reference", dst.color_reference);
		JSONTools::GetAttribute(config, "color_mask", dst.color_mask);
		return true;
	}

}; // namespace chaos

