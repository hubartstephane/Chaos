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
		for (Class const * cls : GetClassesList())
			if (StringTools::Strcmp(class_name, cls->class_name) == 0)
				return cls;
		return nullptr;
	}

	InheritanceType Class::InheritsFrom(Class const* other, bool accept_equal) const
	{
		assert(other != nullptr);

		// fast test on the size
		if (class_size < other->class_size)
			return InheritanceType::NO;

		// class not registered, cannot known result
		if (!IsDeclared() || !other->IsDeclared())
			return InheritanceType::UNKNOWN;

		// returns no if classes are same and we don't accept that as a valid result
		if (this == other)
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
			if (p == other)
				return InheritanceType::YES;
			// fast test on the size
			if (p->class_size < other->class_size)
				return InheritanceType::NO;
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
