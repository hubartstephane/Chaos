#include "chaos/ChaosPCH.h"
#include "chaos/Chaos.h"

namespace chaos
{
	static std::vector<std::pair<Edge, char const *>> const edge_encoding =
	{
		{ Edge::TOP, "top" },
		{ Edge::RIGHT, "right" },
		{ Edge::BOTTOM, "bottom" },
		{ Edge::LEFT, "left" }
	};

	CHAOS_IMPLEMENT_ENUM_METHOD(Edge, edge_encoding);

}; // namespace chaos