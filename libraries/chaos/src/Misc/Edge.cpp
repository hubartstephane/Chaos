#include "chaos/ChaosPCH.h"
#include "chaos/ChaosInternals.h"

namespace chaos
{
	static chaos::EnumTools::EnumMetaData<Edge> const Edge_metadata =
	{
		{ Edge::TOP, "top" },
		{ Edge::RIGHT, "right" },
		{ Edge::BOTTOM, "bottom" },
		{ Edge::LEFT, "left" }
	};

	CHAOS_IMPLEMENT_ENUM_METHOD(Edge, Edge_metadata, CHAOS_API);

}; // namespace chaos