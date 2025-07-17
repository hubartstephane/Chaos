#include "chaos/ChaosPCH.h"
#include "chaos/ChaosInternals.h"

namespace chaos
{
	static EnumMetaData<TextAlignment> const TextAlignment_metadata =
	{
		{ TextAlignment::LEFT, "left" },
		{ TextAlignment::RIGHT, "right" },
		{ TextAlignment::CENTER, "center" },
		{ TextAlignment::JUSTIFY, "justify" }
	};

	CHAOS_IMPLEMENT_ENUM_METHOD(TextAlignment, &TextAlignment_metadata, CHAOS_API);

}; // namespace chaos