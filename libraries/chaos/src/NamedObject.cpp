#include <chaos/NamedObject.h>
#include <chaos/JSONTools.h>

namespace chaos
{
	void NamedObject::SetName(char const * in_name)
	{
		if (in_name == nullptr)
			name.clear();
		else
			name = in_name;
	}

	void SaveIntoJSON(NamedObject const & info, nlohmann::json & json_entry)
	{
		json_entry["name"] = info.name;
		json_entry["tag"] = info.tag;
	}

	void LoadFromJSON(NamedObject & info, nlohmann::json const & json_entry)
	{
		JSONTools::GetAttribute(json_entry, "name", info.name, "");
		JSONTools::GetAttribute(json_entry, "tag", info.tag, 0);
	}

}; // namespace chaos
