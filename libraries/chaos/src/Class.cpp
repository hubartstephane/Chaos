#pragma once

#include <chaos/Class.h>
#include <chaos/StringTools.h>
#include <chaos/JSONTools.h>
#include <chaos/LogTools.h>
#include <chaos/JSONSerializable.h>

namespace chaos
{
	Class const* Class::DeclareSpecialClass(char const* class_name, nlohmann::json const & json)
	{
		return DoDeclareSpecialClass(class_name, json, false);
	}

	Class const* Class::DoDeclareSpecialClass(char const* class_name, nlohmann::json const & json, bool accept_unknown_parent)
	{
		// check parameter and not already registered
		assert(class_name != nullptr && strlen(class_name) > 0);
		assert(FindClass(class_name) == nullptr);

		// parent class is MANDATORY for Special objects
		std::string parent_class_name;
		if (!JSONTools::GetAttribute(json, "parent_class", parent_class_name) && !accept_unknown_parent)
		{
			assert(0);
			LogTools::Error("Class::DoDeclareSpecialClass : special class [%s] require a parent class", class_name);
			return nullptr;
		}
		// parent class is MANDATORY for Special objects
		Class const* parent = FindClass(parent_class_name.c_str());
		if (parent == nullptr && !accept_unknown_parent)
		{
			assert(0);
			LogTools::Error("Class::DoDeclareSpecialClass : special class [%s] has unknown parent class [%s]", class_name, parent_class_name.c_str());
			return nullptr;
		}

		Class* result_cls = new Class;
		if (result_cls != nullptr)
		{
			result_cls->class_name = class_name;
			result_cls->json_data = json;

			if (parent != nullptr)
			{
				result_cls->class_size = parent->class_size;
				result_cls->parent = parent;
				if (parent->create_instance_func != nullptr) // check whether is instanciable !
				{
					result_cls->create_instance_func = [result_cls]()
					{
						Object* result = result_cls->parent->CreateInstance();
						if (result != nullptr)
						{
							JSONSerializable* serializable = auto_cast(result);
							if (serializable != nullptr)
								serializable->SerializeFromJSON(result_cls->json_data);
						}
						return result;
					};
				}				
			}
			GetClassesList().push_back(result_cls);
			return result_cls;
		}
		return nullptr;
	}









	Object * Class::CreateInstance() const
	{
		if (!CanCreateInstance())
		{
			assert(0);
			LogTools::Error("Class::CreateInstance : the class [%s] cannot be instanciated", class_name.c_str());
			return nullptr;
		}
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
		if (class_name != nullptr && strlen(class_name) > 0)
		{
			for (Class const* cls : GetClassesList())
			{
				if (StringTools::Strcmp(class_name, cls->class_name) == 0)
				{
					if (!cls->IsDeclared()) // useless, but keep it for sanity
						return nullptr;
					return cls;
				}
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
