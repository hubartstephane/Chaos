#include <chaos/NamedObject.h>
#include <chaos/JSONTools.h>

namespace chaos
{
	TagType MakeStaticTagType(char const * name)
	{
		static std::map<std::string, TagType> tags;

		// search if tag is already registered
		for (auto it : tags)
			if (it.first == name)
				return it.second;
		// create a tag
		TagType * result = new TagType;
		if (result == nullptr)
			return 0;
		// the pointer points on itsef
		*result = (TagType)result; 
		tags[name] = (TagType)result;
		return (TagType)result;
	}

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
