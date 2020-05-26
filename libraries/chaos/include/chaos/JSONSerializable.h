#pragma once

#include <chaos/StandardHeaders.h>

namespace chaos
{

	class JSONSerializable
	{
	public:

		/** the processor may save its configuration into a JSON file */
		virtual bool SaveIntoJSON(nlohmann::json& json_entry) const;
		/** the processor may save its configuration from a JSON file */
		virtual bool LoadFromJSON(nlohmann::json const& json_entry);
	};

}; // namespace chaos
