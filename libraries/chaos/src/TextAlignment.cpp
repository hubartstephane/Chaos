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

	CHAOS_IMPLEMENT_ENUMJSON_METHOD(TextAlignment, alignment_encoding);

}; // namespace chaos