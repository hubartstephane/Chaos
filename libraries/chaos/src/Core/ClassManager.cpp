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
