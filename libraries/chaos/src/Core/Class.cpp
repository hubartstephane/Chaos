#include "chaos/ChaosPCH.h"
#include "chaos/ChaosInternals.h"

namespace chaos
{
	// ==========================================================
	// Class functions
	// ==========================================================

	Class::Class(std::string in_name) :
		name(std::move(in_name))
	{
	}

	bool Class::CanCreateInstance() const
	{
		return (GetCreateInstanceFunc() != nullptr);
	}

	bool Class::CanCreateInstanceOnStack() const
	{
		return (GetCreateInstanceOnStackFunc() != nullptr);
	}

	Class::create_instance_function_type const* Class::GetCreateInstanceFunc() const
	{
		for (Class const * cls = this ; cls != nullptr ; cls = cls->parent)
			if (cls->create_instance)
				return &cls->create_instance;
		return nullptr;
	}

	Class::create_instance_on_stack_function_type const* Class::GetCreateInstanceOnStackFunc() const
	{
		for (Class const* cls = this; cls != nullptr; cls = cls->parent)
			if (cls->create_instance_on_stack)
				return &cls->create_instance_on_stack;
		return nullptr;
	}

	Object* Class::operator ()() const
	{
		return CreateInstance();
	}

	Object * Class::CreateInstance() const
	{
		if (create_instance_function_type const * create_func = GetCreateInstanceFunc())
		{
			if (Object* result = (*create_func)())
			{
				InitializeObjectInstance(result);
				return result;
			}
			else
				ClassLog::Error("Class::CreateInstance : failed to instanciate class [%s]", name.c_str());
		}
		else
			ClassLog::Error("Class::CreateInstance : the class [%s] cannot be instanciated", name.c_str());

		return nullptr;
	}

	bool Class::IsDeclared() const
	{
		return declared;
	}

	void Class::InitializeObjectInstance(Object* object) const
	{
		// recursive: parents first
		if (parent != nullptr)
			parent->InitializeObjectInstance(object);
		// apply initialization functors
		OnObjectInstanceInitialized(object);
	}

	void Class::OnObjectInstanceInitialized(Object* object) const
	{
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

	void Class::SetParentClass(Class const* in_parent)
	{
		assert(in_parent != nullptr);
		assert(parent == nullptr);

		parent = in_parent;

#if _DEBUG
		assert(!HasCyclicParent());
#endif // #if _DEBUG
	}

#if _DEBUG
	bool Class::HasCyclicParent() const
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
					found_different_manager = true;

				parent_manager = parent_manager->GetParentManager();
			}
		}
		return false;
	}
#endif // _DEBUG

}; // namespace chaos
