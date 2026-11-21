#include "chaos/ChaosPCH.h"
#include "chaos/ChaosInternals.h"

namespace chaos
{
	bool DoSaveIntoJSON(nlohmann::json* json, GamepadInputFilterSettings const& src)
	{
		JSONTools::SetAttribute(json, "length_interpolation_type", src.length_interpolation_type);
		JSONTools::SetAttribute(json, "dead_zone", src.dead_zone);
		JSONTools::SetAttribute(json, "max_zone", src.max_zone);
		JSONTools::SetAttribute(json, "sector_snap_angle_ratio", src.sector_snap_angle_ratio);
		JSONTools::SetAttribute(json, "sector_snap_count", src.sector_snap_count);
		return true;
	}

	bool DoLoadFromJSON(JSONReadConfiguration config, GamepadInputFilterSettings& dst)
	{
		JSONTools::GetAttribute(config, "length_interpolation_type", dst.length_interpolation_type);
		JSONTools::GetAttribute(config, "dead_zone", dst.dead_zone);
		JSONTools::GetAttribute(config, "max_zone", dst.max_zone);
		JSONTools::GetAttribute(config, "sector_snap_angle_ratio", dst.sector_snap_angle_ratio);
		JSONTools::GetAttribute(config, "sector_snap_count", dst.sector_snap_count);
		return true;
	}

}; // namespace chaos
