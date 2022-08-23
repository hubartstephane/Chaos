#include "chaos/ChaosPCH.h"
#include "chaos/Chaos.h"

namespace chaos
{
	static std::vector<std::pair<WrapMode, char const*>> const wrapmode_encoding =
	{
		{ WrapMode::NONE, "NONE" },
		{ WrapMode::CLAMP, "CLAMP" },
		{ WrapMode::WRAP, "WRAP" },
		{ WrapMode::CHECK_RANGES, "CHECK_RANGES" }
	};

	CHAOS_IMPLEMENT_ENUM_METHOD(WrapMode, wrapmode_encoding);

}; // namespace chaos