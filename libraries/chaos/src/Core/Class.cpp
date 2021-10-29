#pragma once

#include <chaos/chaos.h>

namespace chaos
{

	Class * ClassRegistration::operator()(char const * in_alias)
	{
		assert(!StringTools::IsEmpty(in_alias));
		class_ptr->SetAlias(in_alias);
		return class_ptr;
	}

	// XXX: we don't use a class static data but a class static function because we don't know when static data will be created
	//      (and it can happens after static class declaration)
	std::map<char const*, Class*, StringTools::RawStringLess>& Class::GetClasses()
	{
		static std::map<char const *, Class*, StringTools::RawStringLess> classes;
		return classes;
	}

	std::multimap<char const*, Class*, StringTools::RawStringLess>& Class::GetAliases()
	{
		static std::multimap<char const *, Class*, StringTools::RawStringLess> aliases;
		return aliases;
	}

	Object * Class::CreateInstance() const
	{
		if (CanCreateInstance())
			return create_instance_func();
		Log::Error("Class::CreateInstance : the class [%s] cannot be instanciated", name.c_str());
		return nullptr;
	}

	bool Class::CreateInstanceOnStack(std::function<void(Object*)> func) const
	{
		if (!CanCreateInstanceOnStack())
		{
			Log::Error("Class::CreateInstanceOnStack : the class [%s] cannot be instanciated", name.c_str());
			return false;
		}
		create_instance_on_stack_func(func);
		return true;			
	}

	bool Class::IsDeclared() const
	{
		return declared;
	}

	Class const * Class::FindClass(char const* name, Class const * check_class)
	{
		assert(name != nullptr);

		auto& classes = GetClasses();
		auto& aliases = GetAliases();

		/** find the class by name first */
		auto it = classes.find(name);
		if (it != classes.end())
			if (check_class == nullptr || it->second->InheritsFrom(check_class, true) == InheritanceType::YES)
				return it->second;
		/** find class by alias */
		auto it1 = aliases.lower_bound(name);
		auto it2 = aliases.upper_bound(name);
	
		auto found = aliases.end();
		for (auto it = it1; it != it2; ++it)
		{
			if (check_class == nullptr || it->second->InheritsFrom(check_class, true) == InheritanceType::YES)
			{
	#if _DEBUG
				// continue matching search to ensure this there is no ambiguity
				if (found != aliases.end())
				{
					Log::Error("request for ambiguous alias %s", name);
					return nullptr;
				}
				found = it;
	#else
				return it->second;
	#endif
			}
		}
		return (found == aliases.end()) ? nullptr : found->second;
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

	void Class::SetAlias(std::string in_alias)
	{
		assert(StringTools::IsEmpty(alias));
		assert(!StringTools::IsEmpty(in_alias));
		
		auto& aliases = GetAliases();

	#if _DEBUG
		// search whether this exact alias does not exists
		auto it1 = aliases.lower_bound(in_alias.c_str());
		auto it2 = aliases.upper_bound(in_alias.c_str());
		for (auto it = it1; it != it2; ++it)
			assert(it->second != this);
	#endif
		// create the alias
		alias = std::move(in_alias);
		aliases.emplace(alias.c_str(), this); // the key is the pointer on the std::string
	}

}; // namespace chaos
