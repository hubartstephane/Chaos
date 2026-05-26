#include "chaos/ChaosPCH.h"
#include "chaos/ChaosInternals.h"

namespace chaos
{
	// ==========================================================
	// ClassBase functions
	// ==========================================================

	ClassBase::ClassBase(std::string in_name) :
		name(std::move(in_name))
	{
	}

	InheritanceType ClassBase::InheritsFrom(ClassBase const* child_class, ClassBase const* parent_class, bool accept_equal)
	{
		if (child_class == nullptr)
			return InheritanceType::Unknown;
		return child_class->InheritsFrom(parent_class, accept_equal);
	}

	InheritanceType ClassBase::InheritsFrom(ClassBase const* parent_class, bool accept_equal) const
	{
		// returns no if classes are same and we don't accept that as a valid result
		if (this == parent_class)
		{
			if (!accept_equal)
				return InheritanceType::No;
			else
				return InheritanceType::Yes;
		}
		// class not registered, cannot known result
		if (!IsDeclared())
			return InheritanceType::Unknown;
		// parent not registered, cannot known result
		if (parent_class == nullptr || !parent_class->IsDeclared())
			return InheritanceType::Unknown;
		// from top to root in the hierarchy
		for (ClassBase const* p = parent; p != nullptr; p = p->parent)
		{
			// found the searched parent
			if (p == parent_class)
				return InheritanceType::Yes;
			// unintialized class
			if (!p->IsDeclared())
				return InheritanceType::Unknown;
		}
		return InheritanceType::No;
	}

	void ClassBase::SetShortName(std::string in_short_name)
	{
		assert(StringTools::IsEmpty(short_name));
		assert(!StringTools::IsEmpty(in_short_name));
		short_name = std::move(in_short_name);
	}

	void ClassBase::SetParentClass(ClassBase const* in_parent)
	{
		assert(in_parent != nullptr);
		assert(parent == nullptr);

		parent = in_parent;

#if _DEBUG
		assert(!HasCyclicParent());
#endif // #if _DEBUG
	}

#if _DEBUG
	bool ClassBase::HasCyclicParent() const
	{
		if (parent != nullptr && manager != nullptr)
		{
			bool found_different_manager = false;

			ClassManager* parent_manager = parent->GetClassManager();
			while (parent_manager != nullptr)
			{
				if (parent_manager == manager) // cycle found: parent class in child manager
				{
					if (found_different_manager)
						return true;
				}
				else
				{
					found_different_manager = true;
				}
				parent_manager = parent_manager->GetParentManager();
			}
		}
		return false;
	}
#endif // _DEBUG

}; // namespace chaos
