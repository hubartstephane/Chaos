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

	bool NamedObjectFilterList::CheckName(NamedObject const & object) const
	{
		// test for forbidden list
		char const * object_name = object.GetName();
		if (object_name != nullptr && strlen(object_name) > 0)
		{
			size_t c1 = forbidden_names.size();
			for (size_t i = 0; i < c1; ++i)
				if (_strcmpi(forbidden_names[i].c_str(), object_name) == 0)
					return false;
		}

		TagType tag = object.GetTag();

		size_t c2 = forbidden_tags.size();
		for (size_t i = 0; i < c2; ++i)
			if (forbidden_tags[i] == tag)
				return false;

		// check for requirement (OR condition)
		size_t c3 = enable_names.size();
		size_t c4 = enable_tags.size();
		if (c3 > 0 || c4 > 0)
		{
			if (object_name != nullptr && strlen(object_name) > 0)
			{
				for (size_t i = 0; i < c3; ++i)
					if (_strcmpi(enable_names[i].c_str(), object_name) == 0)
						return true;
			}
			for (size_t i = 0; i < c4; ++i)
				if (enable_tags[i] == tag)
					return true;
			// if requirement list, we must met at least one condition
			return false; 
		}
		return true; // no requirement => OK
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
