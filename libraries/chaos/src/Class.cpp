#pragma once

#include <chaos/Class.h>
#include <chaos/StringTools.h>

namespace chaos
{
	void* Class::CreateInstance() const
	{
		if (create_instance_func)
			return create_instance_func();
		return nullptr;
	}

	bool Class::IsDeclared() const
	{
		return (class_name.length() > 0);
	}

	Class const * Class::FindClass(char const* class_name)
	{
		assert(class_name != nullptr && strlen(class_name) > 0);
		for (Class const* cls : GetClassesList())
		{
			if (StringTools::Strcmp(class_name, cls->class_name) == 0)
			{
				if (!cls->IsDeclared()) // useless, but keep it for sanity
					return nullptr;
				return cls;
			}
		}
		return nullptr;
	}

	// static
	InheritanceType Class::InheritsFrom(Class const* child_class, Class const* parent_class, bool accept_equal)
	{
		if (child_class == nullptr)
			return InheritanceType::UNKNOWN;
		return child_class->InheritsFrom(parent_class, accept_equal);
	}

	InheritanceType Class::InheritsFrom(Class const* parent_class, bool accept_equal) const
	{
		// class not registered, cannot known result
		if (!IsDeclared())
			return InheritanceType::UNKNOWN;
		// parent not registered, cannot known result
		if (parent_class == nullptr || !parent_class->IsDeclared())
			return InheritanceType::UNKNOWN;

		// returns no if classes are same and we don't accept that as a valid result
		if (this == parent_class)
		{
			if (!accept_equal)
				return InheritanceType::NO;
			else
				return InheritanceType::YES;
		}
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

	std::vector<Class const *>& Class::GetClassesList()
	{
		static std::vector<Class const*> result;
		return result;
	}

}; // namespace chaos
