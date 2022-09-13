#include "chaos/ChaosPCH.h"
#pragma once

#include <chaos/chaos.h>

namespace chaos
{
	// ==========================================================
	// ClassRegistration functions
	// ==========================================================

	ClassRegistration & ClassRegistration::operator()(std::string in_short_name)
	{
		class_ptr->SetShortName(std::move(in_short_name));
		return *this;
	}

	// ==========================================================
	// ClassFindResult functions
	// ==========================================================

	ClassFindResult::ClassFindResult(ClassManager* in_class_manager, iterator_type in_iterator, bool in_matching_name):
		class_manager(in_class_manager),
		iterator(in_iterator),
		matching_name(in_matching_name)
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
		std::string const& searched_name = (matching_name) ?
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
			class_manager = class_manager->parent_manager;
			if (class_manager != nullptr)
				iterator = class_manager->classes.begin();
		}
		return nullptr;
	}

	// ==========================================================
	// ClassManager functions
	// ==========================================================

	ClassManager::ClassManager(ClassManager* in_parent_manager):
		parent_manager(in_parent_manager)
	{
	}

	ClassManager* ClassManager::GetDefaultInstance()
	{
		//static DisableReferenceCount<ClassManager> default_instance;

		static ClassManager default_instance;
		return &default_instance;
	}

	ClassFindResult ClassManager::FindClass(char const* name)
	{
		assert(name != nullptr);

		// early exit
		if (StringTools::IsEmpty(name))
			return {nullptr, classes.end(), false};

		// search in manager chain
		ClassManager* manager = this;
		while (manager != nullptr)
		{
			bool matching_name = false;
			auto it = std::ranges::find_if(manager->classes, [name, &matching_name](Class const* cls)
			{
				if (StringTools::Stricmp(cls->GetClassName(), name) == 0)
				{
					matching_name = true;
					return true;
				}
				else if (StringTools::Stricmp(cls->GetShortName(), name) == 0)
				{
					matching_name = false;
					return true;
				}
				else
				{
					return false;
				}
			});
			if (it != manager->classes.end())
				return { this, it, matching_name };

			manager = manager->parent_manager;
		}
		/* no class, no possible alias */
		return { nullptr, classes.end(), false };
	}

}; // namespace chaos
