#include "chaos/ChaosPCH.h"
#include "chaos/ChaosInternals.h"

namespace chaos
{
	static EnumMetaData<Edge> const Edge_metadata =
	{
		{ Edge::Top, "top" },
		{ Edge::Right, "right" },
		{ Edge::Bottom, "bottom" },
		{ Edge::Left, "left" }
	};

	CHAOS_IMPLEMENT_ENUM_METHOD(Edge, &Edge_metadata, CHAOS_API);

}; // namespace chaos