#pragma once

#include <chaos/ClassTools.h>
#include <chaos/StringTools.h>

namespace chaos
{
	void* ClassRegistration::CreateInstance() const
	{
		if (create_instance_func)
			return create_instance_func();
		return nullptr;
	}

	bool ClassRegistration::IsRegistered() const
	{
		return (class_name.length() > 0);
	}


	ClassRegistration* ClassTools::GetClassRegistration(char const* class_name)
	{
		assert(class_name != nullptr && strlen(class_name) > 0);
		for (ClassRegistration* registration : GetClassRegistrationList())
			if (StringTools::Strcmp(class_name, registration->class_name) == 0)
				return registration;
		return nullptr;
	}


	InheritanceType ClassTools::InheritsFrom(ClassRegistration const* child_registration, ClassRegistration const* parent_registration, bool accept_equal)
	{
		assert(child_registration != nullptr);
		assert(parent_registration != nullptr);

		// fast test on the size
		if (child_registration->size < parent_registration->size)
			return InheritanceType::NO;

		// class not registered, cannot known result
		if (!child_registration->IsRegistered() || !parent_registration->IsRegistered())
			return InheritanceType::UNKNOWN;

		// returns no if classes are same and we don't accept that as a valid result
		if (child_registration == parent_registration)
		{
			if (!accept_equal)
				return InheritanceType::NO;
			else
				return InheritanceType::YES;
		}
		// from top to root in the hierarchy
		for (child_registration = child_registration->parent; child_registration != nullptr; child_registration = child_registration->parent)
		{
			// found the searched parent
			if (child_registration == parent_registration)
				return InheritanceType::YES;
			// fast test on the size
			if (child_registration->size < parent_registration->size)
				return InheritanceType::NO;
			// unintialized class
			if (!child_registration->IsRegistered())
				return InheritanceType::UNKNOWN;
		}
		return InheritanceType::NO;
	}

	std::vector<ClassRegistration*>& ClassTools::GetClassRegistrationList()
	{
		static std::vector<ClassRegistration*> result;
		return result;
	}

}; // namespace chaos
