#include "chaos/ChaosPCH.h"
#pragma once

#include <chaos/chaos.h>

namespace chaos
{
	ClassManager::ClassManager(ClassManager* in_parent_manager):
		parent_manager(in_parent_manager)
	{
	}

	ClassManager* ClassManager::GetDefaultInstance()
	{
		static DisableReferenceCount<ClassManager> default_instance;
		return &default_instance;
	}

	Class* ClassManager::CreateClass(std::string name, Class * in_parent_class)
	{
		// search in current manager (but not parents) for a class that have same name
		auto it = std::ranges::find_if(classes, [&name](Class const* cls)
		{
			if (StringTools::Stricmp(cls->GetClassName(), name) == 0)
			{
				return true;
			}
			return false;
		});
		// cannot have a class with same name in the same manager (but not in parent manager)
		if (it != classes.end())
		{
			Log::Error("ClassManager::CreateClass(...): class already existing [%s]", name.c_str());
			return nullptr;
		}
		// create the class
		return new Class(this, std::move(name), in_parent_class);
	}

	ClassFindResult ClassManager::FindClass(char const* name, bool search_manager_hierarchy)
	{
		assert(name != nullptr);

		// early exit
		if (StringTools::IsEmpty(name))
			return {nullptr, classes.end(), ClassMatchType::MATCH_NAME};

		// search in manager chain
		ClassManager* manager = this;
		while (manager != nullptr)
		{
			ClassMatchType match_type = ClassMatchType::MATCH_NAME;
			auto it = std::ranges::find_if(manager->classes, [name, &match_type](Class const* cls)
			{
				if (StringTools::Stricmp(cls->GetClassName(), name) == 0)
				{
					match_type = ClassMatchType::MATCH_NAME;
					return true;
				}
				else if (StringTools::Stricmp(cls->GetShortName(), name) == 0)
				{
					match_type = ClassMatchType::MATCH_SHORTNAME;
					return true;
				}
				return false;
			});
			if (it != manager->classes.end())
				return { this, it, match_type };
			if (!search_manager_hierarchy)
				break;
			manager = manager->parent_manager.get();
		}
		/* no class, no possible alias */
		return { nullptr, classes.end(), ClassMatchType::MATCH_NAME };
	}

}; // namespace chaos
