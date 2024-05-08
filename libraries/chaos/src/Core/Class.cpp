#include "chaos/ChaosPCH.h"
#include "chaos/ChaosInternals.h"

namespace chaos
{
	// ==========================================================
	// Class functions
	// ==========================================================

	Class::Class(ClassManager* in_manager, std::string in_name, Class * in_parent):
		parent(in_parent),
		name(std::move(in_name)),
		manager(in_manager)
	{
		assert(in_manager != nullptr);

#if _DEBUG
		// ensure the parent class does not come from a child manager (cyclic references)
		if (in_parent != nullptr)
		{
			ClassManager* parent_manager = in_parent->GetClassManager();
			while (parent_manager != nullptr)
			{
				assert(parent_manager != in_manager);
				parent_manager = parent_manager->GetParentManager();
			}
		}
#endif
		manager->classes.push_back(this);
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
				InitializeObjectInstance(this, result);
				return result;
			}
			else
				Log::Error("Class::CreateInstance : failed to instanciate class [%s]", name.c_str());
		}
		else
			Log::Error("Class::CreateInstance : the class [%s] cannot be instanciated", name.c_str());

		return nullptr;
	}

	bool Class::IsDeclared() const
	{
		return declared;
	}

	void Class::InitializeObjectInstance(Class const * cls, Object* object)
	{
		if (cls != nullptr)
		{
			InitializeObjectInstance(cls->parent, object); // recursive: parents first
			for (initialization_function_type const& func : cls->initialization_functions)
				func(object);
		}
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

	void Class::AddObjectInitializationFunction(initialization_function_type func)
	{
		initialization_functions.push_back(std::move(func));
	}

}; // namespace chaos
