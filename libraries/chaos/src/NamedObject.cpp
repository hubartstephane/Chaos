#include <chaos/NamedObject.h>
#include <chaos/JSONTools.h>

namespace chaos
{
	TagType MakeStaticTagType(char const * name)
	{
		static std::set<std::string> tags;
		// search if tag is already registered
		for (std::string const & it : tags)
			if (it == name)
				return (TagType)it.c_str();
		// insert a new string
		auto it = tags.insert(std::move(std::string(name)));		
		return (TagType)it.first->c_str();
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
				if (StringTools::Stricmp(forbidden_names[i].c_str(), object_name) == 0)
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
					if (StringTools::Stricmp(enable_names[i], object_name) == 0)
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

	bool SaveIntoJSON(nlohmann::json & json_entry, NamedObject const & src)
	{
		if (!json_entry.is_object())
			json_entry = nlohmann::json::object(); 

		JSONTools::SetAttribute(json_entry, "name", src.GetName());
		JSONTools::SetAttribute(json_entry, "tag", src.GetTag());
		return true;
	}

	bool LoadFromJSON(nlohmann::json const & json_entry, NamedObject & dst)
	{
		if (!json_entry.is_object())
			return false;

		std::string name;
		JSONTools::GetAttribute(json_entry, "name", name, "");
		dst.SetName(name.c_str());

		TagType tag;
		JSONTools::GetAttribute(json_entry, "tag", tag, 0);
		dst.SetTag(tag);
		return true;
	}

}; // namespace chaos
