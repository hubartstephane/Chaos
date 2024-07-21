#include "chaos/ChaosPCH.h"
#include "chaos/ChaosInternals.h"

namespace chaos
{
	TagType DeclareTag(char const * name)
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

}; // namespace chaos
