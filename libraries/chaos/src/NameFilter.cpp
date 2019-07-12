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
	
	void NameFilter::AddEnabledName(char const * name)
	{
		AddNameImpl(name, enabled_names);
	}

	void NameFilter::AddDisabledName(char const * name)
	{
		AddNameImpl(name, disabled_names);
	}

	void NameFilter::RemoveEnabledName(char const * name)
	{
		RemoveNameImpl(name, enabled_names);
	}

	void NameFilter::RemoveDisabledName(char const * name)
	{
		RemoveNameImpl(name, disabled_names);
	}

	void NameFilter::AddNameImpl(char const * name, std::vector<std::string> & target_list)
	{
		// nullptr is equivalent to empty string
		if (name == nullptr)
			name = "";
		// search if the name is already existing
		for (std::string const & element : target_list)
			if (StringTools::Stricmp(name, element) == 0)
				return;
		// insert the element
		target_list.push_back(name);
	}

	void NameFilter::RemoveNameImpl(char const * name, std::vector<std::string> & target_list)
	{
		// nullptr is equivalent to empty string
		if (name == nullptr)
			name = "";
		// remove the element
		size_t count = target_list.size();
		for (size_t i = 0; i < count; ++i)
		{
			if (StringTools::Stricmp(name, target_list[i]) == 0)
			{
				target_list.erase(target_list.begin() + i);
				return;
			}
		}
	}

}; // namespace chaos
