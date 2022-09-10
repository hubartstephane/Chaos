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
		assert(!StringTools::IsEmpty(in_short_name));
		class_ptr->SetShortName(std::move(in_short_name));
		return *this;
	}

	// ==========================================================
	// ClassFindResult functions
	// ==========================================================

	ClassFindResult::ClassFindResult(Class* in_result, ClassManager* in_class_manager, short_name_iterator_type in_short_name_iterator) :
		result(in_result),
		class_manager(in_class_manager),
		short_name_iterator(in_short_name_iterator)
	{
	}

	ClassFindResult::operator Class* () const
	{
		return Resolve(nullptr);
	}

	Class* ClassFindResult::Resolve(Class const * check_class) const
	{
		// something found in classes
		if (result != nullptr)
			return result;

		// something found in aliases (but this may be the wrong type)
		// the first iterator is helpfull to keep trace of the searched name
		auto& classes = class_manager->classes;

		if (short_name_iterator != classes.end())
		{
			std::string const& searched_name = (*short_name_iterator)->GetShortName();

			auto found_it = classes.end();

			for (auto it = short_name_iterator; it != classes.end() ; ++it)
			{
				if (StringTools::Stricmp(searched_name, (*it)->GetShortName()) == 0)
				{
					if (check_class == nullptr || (*it)->InheritsFrom(check_class, true) == InheritanceType::YES)
					{
#if _DEBUG  // ensure no ambiguity in debug and continue the search
						assert(found_it == classes.end());
						found_it = it;
#else
						return *it;
#endif
					}
				}
			}

#if _DEBUG
			if (found_it != classes.end())
				return *found_it;
#endif
		}
		// nothing found
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

		/** early exit */
		if (StringTools::IsEmpty(name))
			return {nullptr, this, classes.end()};

		/* find the class by name first */
		{
			auto it = std::ranges::find_if(classes, [name](Class const * cls)
			{
				return (StringTools::Stricmp(cls->GetClassName(), name) == 0);
			});
			if (it != classes.end())
				return { *it, this, classes.end()};
		}
		/* find class by short name */
		{
			auto it = std::ranges::find_if(classes, [name](Class const * cls)
			{
				return (StringTools::Stricmp(cls->GetShortName(), name) == 0);
			});
			// note: the iterator points on a class that have the same short name as the one been searched
			// (this may nevertheless not be the one searched)
			// the ClassFindResult will continue the search if the class does not match and will use the short name accessed throught this iterator
			// (to avoid an extra allocating for the searched name)
			if (it != classes.end())
				return { nullptr, this, it};
		}
		/* no class, no possible alias */
		return { nullptr, this, classes.end()};
	}

}; // namespace chaos
