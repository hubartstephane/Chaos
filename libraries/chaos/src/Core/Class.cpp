#include "chaos/ChaosPCH.h"
#pragma once

#include <chaos/chaos.h>

namespace chaos
{
	// ==========================================================
	// Class functions
	// ==========================================================

	Object * Class::CreateInstance() const
	{
		if (CanCreateInstance())
			return create_instance_func();
		Log::Error("Class::CreateInstance : the class [%s] cannot be instanciated", name.c_str());
		return nullptr;
	}

	bool Class::IsDeclared() const
	{
		return declared;
	}

	InheritanceType Class::InheritsFrom(Class const* child_class, Class const* parent_class, bool accept_equal)
	{
		if (child_class == nullptr)
			return InheritanceType::UNKNOWN;
		return child_class->InheritsFrom(parent_class, accept_equal);
	}

	InheritanceType Class::InheritsFrom(Class const* parent_class, bool accept_equal) const
	{
		// returns no if classes are same and we don't accept that as a valid result
		if (this == parent_class)
		{
			if (!accept_equal)
				return InheritanceType::NO;
			else
				return InheritanceType::YES;
		}
		// class not registered, cannot known result
		if (!IsDeclared())
			return InheritanceType::UNKNOWN;
		// parent not registered, cannot known result
		if (parent_class == nullptr || !parent_class->IsDeclared())
			return InheritanceType::UNKNOWN;
		// from top to root in the hierarchy
		for (Class const* p = parent; p != nullptr; p = p->parent)
		{
			// found the searched parent
			if (p == parent_class)
				return InheritanceType::YES;
			// unintialized class
			if (!p->IsDeclared())
				return InheritanceType::UNKNOWN;
		}
		return InheritanceType::NO;
	}

	void Class::SetShortName(std::string in_short_name)
	{
		assert(StringTools::IsEmpty(short_name));
		assert(!StringTools::IsEmpty(in_short_name));
		short_name = std::move(in_short_name);
	}

}; // namespace chaos
