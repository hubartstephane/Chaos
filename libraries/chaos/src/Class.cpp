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

	Class const * ClassTools::GetClass(char const* class_name)
	{
		assert(class_name != nullptr && strlen(class_name) > 0);
		for (Class const * cls : GetClassesList())
			if (StringTools::Strcmp(class_name, cls->class_name) == 0)
				return cls;
		return nullptr;
	}

	InheritanceType ClassTools::InheritsFrom(Class const* child, Class const* parent, bool accept_equal)
	{
		assert(child != nullptr);
		assert(parent != nullptr);

		// fast test on the size
		if (child->class_size < parent->class_size)
			return InheritanceType::NO;

		// class not registered, cannot known result
		if (!child->IsDeclared() || !parent->IsDeclared())
			return InheritanceType::UNKNOWN;

		// returns no if classes are same and we don't accept that as a valid result
		if (child == parent)
		{
			if (!accept_equal)
				return InheritanceType::NO;
			else
				return InheritanceType::YES;
		}
		// from top to root in the hierarchy
		for (child = child->parent; child != nullptr; child = child->parent)
		{
			// found the searched parent
			if (child == parent)
				return InheritanceType::YES;
			// fast test on the size
			if (child->class_size < parent->class_size)
				return InheritanceType::NO;
			// unintialized class
			if (!child->IsDeclared())
				return InheritanceType::UNKNOWN;
		}
		return InheritanceType::NO;
	}

	std::vector<Class const *>& ClassTools::GetClassesList()
	{
		static std::vector<Class const*> result;
		return result;
	}

}; // namespace chaos
