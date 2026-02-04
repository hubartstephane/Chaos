#include "chaos/ChaosPCH.h"
#include "chaos/ChaosInternals.h"

namespace chaos
{
	static EnumMetaData<TextAlignment> const TextAlignment_metadata =
	{
		{ TextAlignment::Left, "left" },
		{ TextAlignment::Right, "right" },
		{ TextAlignment::Center, "center" },
		{ TextAlignment::Justify, "justify" }
	};

	CHAOS_IMPLEMENT_ENUM_METHOD(TextAlignment, &TextAlignment_metadata, CHAOS_API);

}; // namespace chaos