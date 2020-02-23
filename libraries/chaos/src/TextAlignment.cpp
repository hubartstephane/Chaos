#include <chaos/TextAlignment.h>
#include <chaos/JSONTools.h>

namespace chaos
{
	static std::vector<std::pair<TextAlignment, char const*>> const alignment_encoding =
	{
		{ TextAlignment::ALIGN_LEFT, "left" },
		{ TextAlignment::ALIGN_RIGHT, "right" },
		{ TextAlignment::ALIGN_CENTER, "center" },
		{ TextAlignment::ALIGN_JUSTIFY, "justify" }
	};

	bool LoadFromJSON(nlohmann::json const& json_entry, TextAlignment& dst)
	{
		return LoadEnumFromJSON(json_entry, alignment_encoding, dst);
	}

	bool SaveIntoJSON(nlohmann::json& json_entry, TextAlignment const& src)
	{
		return SaveEnumIntoJSON(json_entry, alignment_encoding, src);
	}

}; // namespace chaos