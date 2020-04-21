#include <chaos/WrapMode.h>
#include <chaos/JSONTools.h>

namespace chaos
{
	static std::vector<std::pair<WrapMode, char const*>> const wrapmode_encoding =
	{
		{ WrapMode::none, "none" },
		{ WrapMode::clamp, "clamp" },
		{ WrapMode::wrap, "wrap" },
		{ WrapMode::check_ranges, "check_ranges" }
	};

	CHAOS_IMPLEMENT_ENUMJSON_METHOD(WrapMode, wrapmode_encoding);

}; // namespace chaos