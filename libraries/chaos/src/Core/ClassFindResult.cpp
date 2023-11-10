#include "chaos/ChaosPCH.h"
#include "chaos/ChaosInternals.h"

namespace chaos
{
	// ==========================================================
	// ClassFindResult functions
	// ==========================================================

	ClassFindResult::ClassFindResult(ClassManager* in_class_manager, iterator_type in_iterator, ClassMatchType in_match_type):
		class_manager(in_class_manager),
		iterator(in_iterator),
		match_type(in_match_type)
	{
	}

	ClassFindResult::operator Class* () const
	{
		return Resolve(nullptr);
	}

	Class* ClassFindResult::Resolve(Class const * check_class) const
	{
		// check for cached result or stop if class_manager
		if (result != nullptr || class_manager == nullptr)
			return result;

		// we know that the iterator points on a valid entry. get the string that made this entry a good one
		std::string const& searched_name = (match_type == ClassMatchType::MATCH_NAME) ?
			(*iterator)->GetClassName() :
			(*iterator)->GetShortName();

		// check for the very first entry (string comparaison not necessary)
		if (check_class == nullptr || (*iterator)->InheritsFrom(check_class, true) == InheritanceType::YES)
		{
			result = *iterator;
			return result;
		}

		// process manager chain
		while (class_manager != nullptr)
		{
			// search all classes of the manager
			for (; iterator != class_manager->classes.end(); ++iterator)
			{
				if ((StringTools::Stricmp((*iterator)->GetClassName(), searched_name) == 0) ||
					(StringTools::Stricmp((*iterator)->GetShortName(), searched_name) == 0))
				{
					result = *iterator;
					return result;
				}
			}
			// next manager in chain
			class_manager = class_manager->parent_manager.get();
			if (class_manager != nullptr)
				iterator = class_manager->classes.begin();
		}
		return nullptr;
	}

}; // namespace chaos
