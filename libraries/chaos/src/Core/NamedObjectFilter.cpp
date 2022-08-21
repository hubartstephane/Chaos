#include "chaos/Chaos.h"

namespace chaos
{
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

}; // namespace chaos
