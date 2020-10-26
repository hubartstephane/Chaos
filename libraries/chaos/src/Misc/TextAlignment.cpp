#include <chaos/Chaos.h>

namespace chaos
{
	static std::vector<std::pair<TextAlignment, char const*>> const alignment_encoding =
	{
		{ TextAlignment::LEFT, "left" },
		{ TextAlignment::RIGHT, "right" },
		{ TextAlignment::CENTER, "center" },
		{ TextAlignment::JUSTIFY, "justify" }
	};

	CHAOS_IMPLEMENT_ENUMJSON_METHOD(TextAlignment, alignment_encoding);

}; // namespace chaos