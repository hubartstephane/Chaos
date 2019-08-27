#include <chaos/NameFilter.h>
#include <chaos/StringTools.h>

namespace chaos
{

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
		std::vector<std::string> name_array = chaos::StringTools::Split(names, ';');
		for (std::string & name : name_array)
		{
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
		std::vector<std::string> name_array = chaos::StringTools::Split(names, ';');
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

}; // namespace chaos
