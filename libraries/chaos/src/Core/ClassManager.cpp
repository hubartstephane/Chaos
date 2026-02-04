#include "chaos/ChaosPCH.h"
#include "chaos/ChaosInternals.h"

namespace chaos
{
	CHAOS_IMPLEMENT_ENUM_BITMASK_METHOD(FindClassFlags, nullptr, CHAOS_API);

	ClassManager::ClassManager(ClassManager* in_parent_manager):
		parent_manager(in_parent_manager)
	{
	}

	ClassManager* ClassManager::GetDefaultInstance()
	{
		static DisableReferenceCount<ClassManager> default_instance;
		return &default_instance;
	}

	ClassFindResult ClassManager::FindClass(char const* name, FindClassFlags flags)
	{
		assert(name != nullptr);
		assert(HasAnyFlags(flags, FindClassFlags::Name | FindClassFlags::Shortname)); // at least one search criteria

		// early exit
		if (StringTools::IsEmpty(name))
			return {nullptr, classes.end(), ClassMatchType::Name}; // empty ClassFindResult result

		// search in manager chain
		ClassManager* manager = this;
		while (manager != nullptr)
		{
			ClassMatchType match_type = ClassMatchType::Name;
			auto it = std::ranges::find_if(manager->classes, [name, &match_type, flags](Class const* cls)
			{
				if (HasAnyFlags(flags, FindClassFlags::Name))
				{
					if (StringTools::Stricmp(cls->GetClassName(), name) == 0)
					{
						match_type = ClassMatchType::Name;
						return true;
					}
				}				
				if (HasAnyFlags(flags, FindClassFlags::Shortname))
				{
					if (StringTools::Stricmp(cls->GetShortName(), name) == 0)
					{
						match_type = ClassMatchType::Shortname;
						return true;
					}
				}
				return false;
			});
			if (it != manager->classes.end())
				return { this, it, match_type };
			if (!HasAnyFlags(flags, FindClassFlags::ParentManager))
				break;
			manager = manager->parent_manager.get();
		}
		// no class, no possible alias
		return { nullptr, classes.end(), ClassMatchType::Name }; // empty ClassFindResult result
	}

	void ClassManager::InsertClass(Class* cls)
	{
		assert(cls != nullptr);
		assert(cls->manager == nullptr);

		cls->manager = this;
		classes.push_back(cls);
#if _DEBUG
		assert(!cls->HasCyclicParent());
#endif
	}

}; // namespace chaos
