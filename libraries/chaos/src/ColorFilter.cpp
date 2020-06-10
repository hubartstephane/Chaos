#include <chaos/ColorFilter.h>
#include <chaos/JSONTools.h>

namespace chaos
{
	bool SaveIntoJSON(nlohmann::json& json_entry, ColorFilter const& src)
	{
		if (!json_entry.is_object())
			json_entry = nlohmann::json::object();
		JSONTools::SetAttribute(json_entry, "reverse_range_check", src.reverse_range_check);
		JSONTools::SetAttribute(json_entry, "min_color_range", src.min_color_range);
		JSONTools::SetAttribute(json_entry, "max_color_range", src.max_color_range);
		JSONTools::SetAttribute(json_entry, "reverse_distance_check", src.reverse_distance_check);
		JSONTools::SetAttribute(json_entry, "reference_color", src.reference_color);
		JSONTools::SetAttribute(json_entry, "reference_distance", src.reference_distance);
		return true;
	}

	bool LoadFromJSON(nlohmann::json const& json_entry, ColorFilter& dst)
	{
		if (!json_entry.is_object())
			return false;
		JSONTools::GetAttribute(json_entry, "reverse_range_check", dst.reverse_range_check);
		JSONTools::GetAttribute(json_entry, "min_color_range", dst.min_color_range);
		JSONTools::GetAttribute(json_entry, "max_color_range", dst.max_color_range);
		JSONTools::GetAttribute(json_entry, "reverse_distance_check", dst.reverse_distance_check);
		JSONTools::GetAttribute(json_entry, "reference_color", dst.reference_color);
		JSONTools::GetAttribute(json_entry, "reference_distance", dst.reference_distance);
		return true;
	}

}; // namespace chaos

