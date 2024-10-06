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
		assert(HasAnyFlags(flags, FindClassFlags::NAME | FindClassFlags::SHORTNAME)); // at least one search criteria

		// early exit
		if (StringTools::IsEmpty(name))
			return {nullptr, classes.end(), ClassMatchType::MATCH_NAME}; // empty ClassFindResult result

		// search in manager chain
		ClassManager* manager = this;
		while (manager != nullptr)
		{
			ClassMatchType match_type = ClassMatchType::MATCH_NAME;
			auto it = std::ranges::find_if(manager->classes, [name, &match_type, flags](Class const* cls)
			{
				if (HasAnyFlags(flags, FindClassFlags::NAME))
				{
					if (StringTools::Stricmp(cls->GetClassName(), name) == 0)
					{
						match_type = ClassMatchType::MATCH_NAME;
						return true;
					}
				}				
				if (HasAnyFlags(flags, FindClassFlags::SHORTNAME))
				{
					if (StringTools::Stricmp(cls->GetShortName(), name) == 0)
					{
						match_type = ClassMatchType::MATCH_SHORTNAME;
						return true;
					}
				}
				return false;
			});
			if (it != manager->classes.end())
				return { this, it, match_type };
			if (!HasAnyFlags(flags, FindClassFlags::PARENT_MANAGER))
				break;
			manager = manager->parent_manager.get();
		}
		// no class, no possible alias
		return { nullptr, classes.end(), ClassMatchType::MATCH_NAME }; // empty ClassFindResult result
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
