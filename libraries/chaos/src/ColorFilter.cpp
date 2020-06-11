#include <chaos/ColorFilter.h>
#include <chaos/JSONTools.h>

namespace chaos
{
	bool ColorFilter::Filter(glm::vec4 const& color) const
	{
#if 0
		// XXX : i do not use XOR in checks because EQUAL values are always in range (the opposite of greater is less_equal, that what i don t want)
		if (distance >= 0.0f)
		{
			float d2 = glm::distance2(color * color_mask, color_reference);
			if (!reverse_check)
			{
				if (d2 > distance * distance)
					return false;
			}
			else
			{
				if (d2 < distance * distance)
					return false;
			}
		}
#endif
		return true;
	}

	bool SaveIntoJSON(nlohmann::json& json_entry, ColorFilter const& src)
	{
		if (!json_entry.is_object())
			json_entry = nlohmann::json::object();
		JSONTools::SetAttribute(json_entry, "distance_operator", src.distance_operator);
		JSONTools::SetAttribute(json_entry, "distance", src.distance);
		JSONTools::SetAttribute(json_entry, "color_reference", src.color_reference);
		JSONTools::SetAttribute(json_entry, "color_mask", src.color_mask);
		return true;
	}

	bool LoadFromJSON(nlohmann::json const& json_entry, ColorFilter& dst)
	{
		if (!json_entry.is_object())
			return false;
		JSONTools::GetAttribute(json_entry, "distance_operator", dst.distance_operator);
		JSONTools::GetAttribute(json_entry, "distance", dst.distance);
		JSONTools::GetAttribute(json_entry, "color_reference", dst.color_reference);
		JSONTools::GetAttribute(json_entry, "color_mask", dst.color_mask);
		return true;
	}

}; // namespace chaos

