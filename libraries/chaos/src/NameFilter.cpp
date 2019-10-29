#include <chaos/NameFilter.h>
#include <chaos/StringTools.h>
#include <chaos/JSONTools.h>

namespace chaos
{

	// ============================================================
	// NameFilter methods
	// ============================================================

	bool NameFilter::IsEmpty() const
	{
		return (enabled_names.size() + disabled_names.size() == 0);
	}

	bool NameFilter::IsNameEnabled(char const * name) const
	{
		// nullptr is equivalent to empty string
		if (name == nullptr)
			name = "";
		// is this pass forbidden ?
		for (std::string const & disabled : disabled_names)
			if (StringTools::Stricmp(name, disabled) == 0)
				return false;
		// is this pass in the enabled list
		if (enabled_names.size() > 0)
		{
			for (std::string const & enabled : enabled_names)
				if (StringTools::Stricmp(name, enabled) == 0)
					return true;
			return false;
		}
		return true;
	}
	
	void NameFilter::AddEnabledNames(char const * names)
	{
		AddNamesImpl(names, enabled_names);
	}

	void NameFilter::AddDisabledNames(char const * names)
	{
		AddNamesImpl(names, disabled_names);
	}

	void NameFilter::RemoveEnabledNames(char const * names)
	{
		RemoveNamesImpl(names, enabled_names);
	}

	void NameFilter::RemoveDisabledNames(char const * names)
	{
		RemoveNamesImpl(names, disabled_names);
	}

	void NameFilter::AddNamesImpl(char const * names, std::vector<std::string> & target_list)
	{
		// nullptr is equivalent to empty string
		if (names == nullptr)
			names = "";
		// split by separator
		std::vector<std::string> name_array = chaos::StringTools::Split(names, separator);
		for (std::string & name : name_array)
		{
			name.erase(std::remove_if(name.begin(), name.end(), isspace), name.end()); // remove space 
			// search if the name is already existing
			for (std::string const & element : target_list)
				if (StringTools::Stricmp(name, element) == 0)
					continue;
			// insert the element
			target_list.push_back(std::move(name));
		}
	}

	void NameFilter::RemoveNamesImpl(char const * names, std::vector<std::string> & target_list)
	{
		// nullptr is equivalent to empty string
		if (names == nullptr)
			names = "";
		// split by separator
		std::vector<std::string> name_array = chaos::StringTools::Split(names, separator);
		for (std::string const & name : name_array)
		{
			size_t count = target_list.size();
			for (size_t i = 0; i < count; ++i)
			{
				if (StringTools::Stricmp(name, target_list[i]) == 0)
				{
					target_list.erase(target_list.begin() + i);
					break;
				}
			}
		}
	}

	// ============================================================
	// JSON methods
	// ============================================================

	bool SaveIntoJSON(nlohmann::json & json_entry, NameFilter const & src)
	{
		if (!json_entry.is_object())
			json_entry = nlohmann::json::object();
		JSONTools::SetAttribute(json_entry, "enabled_names", src.enabled_names);
		JSONTools::SetAttribute(json_entry, "disabled_names", src.disabled_names);
		return true;
	}

	bool LoadFromJSON(nlohmann::json const & json_entry, NameFilter & dst)
	{
		// the simplest format is with string and ',' as separator 
		if (json_entry.is_string())
		{
			dst.AddEnabledNames(json_entry.get<std::string>().c_str());
			return true;
		}
		else if (json_entry.is_object())
		{
			// "enabled_names" can be a string with ';' as separator or an array of string
			nlohmann::json::const_iterator it_enabled = json_entry.find("enabled_names");
			if (it_enabled != json_entry.end())
			{
				if (it_enabled->is_string())
					dst.AddEnabledNames(it_enabled->get<std::string>().c_str());
				else
					LoadFromJSON(*it_enabled, dst.enabled_names);
			}

			// "disabled_names" can be a string with ',' as separator or an array of string
			nlohmann::json::const_iterator it_disabled = json_entry.find("disabled_names");
			if (it_disabled != json_entry.end())
			{
				if (it_disabled->is_string())
					dst.AddDisabledNames(it_disabled->get<std::string>().c_str());
				else
					LoadFromJSON(*it_disabled, dst.disabled_names);
			}
			return true;		
		}			
		return false;
	}

}; // namespace chaos
