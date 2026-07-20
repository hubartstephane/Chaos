#include "chaos/ChaosPCH.h"
#include "chaos/ChaosInternals.h"

namespace chaos
{
	CHAOS_IMPLEMENT_ENUM_BITMASK_METHOD(FindClassFlags, nullptr, CHAOS_API);

	ClassManager::ClassManager(ClassManager* in_parent_manager) :
		parent_manager(in_parent_manager)
	{
	}

	ClassManager::~ClassManager()
	{
		for (auto& cls : classes) // make all classes "orphan"
			cls->manager = nullptr;
	}

	ClassFindResult ClassManager::FindClass(char const* name, FindClassFlags flags)
	{
		assert(name != nullptr);
		assert(HasAnyFlags(flags, FindClassFlags::Name | FindClassFlags::Shortname)); // at least one search criteria

		// early exit
		if (StringTools::IsEmpty(name))
			return { nullptr, classes.end(), ClassMatchType::Name, flags }; // empty ClassFindResult result

		// search in manager chain
		ClassManager* manager = this;
		while (manager != nullptr)
		{
			ClassMatchType match_type = ClassMatchType::Name;
			auto it = std::ranges::find_if(manager->classes, [name, &match_type, flags](shared_ptr<ClassBase> const& cls)
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
				return { manager, it, match_type, flags };
			if (!HasAnyFlags(flags, FindClassFlags::ParentManager))
				break;
			manager = manager->parent_manager.get();
		}
		// no class, no possible alias
		return { nullptr, classes.end(), ClassMatchType::Name, flags }; // empty ClassFindResult result
	}

	bool ClassManager::IsAncestorManagerFor(ClassManager const* child_manager, bool accept_same) const
	{
		assert(child_manager != nullptr);

		if (child_manager == this)
			return accept_same;

		ClassManager const* target_manager = child_manager->parent_manager.get();
		while (target_manager != nullptr)
		{
			if (target_manager == this)
				return true;
			target_manager = target_manager->parent_manager.get();
		}
		return false;
	}

	void ClassManager::InsertClass(ClassBase* cls)
	{
		assert(cls != nullptr);
		assert(cls->manager == nullptr);

		classes.push_back(cls);
		cls->manager = this;
	}

}; // namespace chaos
