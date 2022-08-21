#include "chaos/Chaos.h"

namespace chaos
{
	// ==============================================================
	// JSONParam
	// ==============================================================

	JSONParam::JSONParam(nlohmann::json const& in_json):
		json(in_json) {}

	JSONParam::operator nlohmann::json const& () const
	{
		return json;
	}

}; // namespace chaos
