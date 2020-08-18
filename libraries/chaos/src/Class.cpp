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
		Class* result = DoDeclareSpecialClassStep1(class_name, json);
		if (result != nullptr)
		{
			if (!result->DoDeclareSpecialClassStep2() || !result->DoDeclareSpecialClassStep3())
			{
				DoInvalidateSpecialClass(result);
				return nullptr;
			}
		}
		return result;
	}

	Class * Class::DoDeclareSpecialClassStep1(char const* class_name, nlohmann::json const & json)
	{
		// check parameter and not already registered
		assert(class_name != nullptr && strlen(class_name) > 0);
		assert(FindClass(class_name) == nullptr);

		Class* result = new Class;
		if (result != nullptr)
		{
			result->class_name = class_name;
			result->json_data = json;

			GetClassesList().push_back(result);
			return result;
		}
		return nullptr;
	}

	bool Class::DoDeclareSpecialClassStep2()
	{
		// parent class is MANDATORY for Special objects
		std::string parent_class_name;
		if (!JSONTools::GetAttribute(json_data, "parent_class", parent_class_name))
		{
			LogTools::Error("Class::DoDeclareSpecialClassStep2 : special class [%s] require a parent class", class_name.c_str());
			return false;
		}
		// parent class is MANDATORY for Special objects
		parent = FindClass(parent_class_name.c_str());
		if (parent == nullptr)
		{
			LogTools::Error("Class::DoDeclareSpecialClassStep2 : special class [%s] has unknown parent class [%s]", class_name.c_str(), parent_class_name.c_str());
			return false;
		}
		// initialize missing data (size, creation_delegate)
		class_size = parent->class_size;

		return true;
	}

	bool Class::DoDeclareSpecialClassStep3()
	{
		if (parent->create_instance_func != nullptr) // check whether is instanciable !
		{
			create_instance_func = [this]()
			{
				Object* result = parent->CreateInstance();
				if (result != nullptr)
				{
					JSONSerializable* serializable = auto_cast(result);
					if (serializable != nullptr)
						serializable->SerializeFromJSON(json_data);
				}
				return result;
			};
		}
		return true;
	}
		
	void Class::DoInvalidateSpecialClass(Class const* cls)
	{
		assert(cls != nullptr);

		std::vector<Class*>& classes = GetClassesList();
		classes.erase(std::remove(classes.begin(), classes.end(), cls));
	}

	Object * Class::CreateInstance() const
	{
		if (!CanCreateInstance())
		{
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

	std::vector<Class *>& Class::GetClassesList()
	{
		static std::vector<Class *> result;
		return result;
	}

}; // namespace chaos
