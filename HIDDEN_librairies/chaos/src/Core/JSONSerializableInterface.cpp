#include "chaos/ChaosPCH.h"
#include "chaos/ChaosInternals.h"

namespace chaos
{
	bool JSONSerializableInterface::SerializeIntoJSON(nlohmann::json * json) const
	{
		if (json == nullptr)
			return false;
		assert(json->is_object());
		return true;
	}

	bool JSONSerializableInterface::SerializeFromJSON(JSONReadConfiguration config)
	{
		return true;
	}

}; // namespace chaos
