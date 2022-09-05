#include "chaos/ChaosPCH.h"
#pragma once

#include <chaos/chaos.h>

namespace chaos
{
	// ==========================================================
	// ClassRegistration functions
	// ==========================================================

	ClassRegistration & ClassRegistration::operator()(char const * in_alias)
	{
		assert(!StringTools::IsEmpty(in_alias));
		class_ptr->SetAlias(in_alias);
		return *this;
	}

	// ==========================================================
	// ClassFindResult functions
	// ==========================================================

	ClassFindResult::ClassFindResult(Class* in_result, alias_iterator_type in_alias_iterator) :
		result(in_result),
		alias_iterator(in_alias_iterator)
	{
	}

	ClassFindResult::operator Class* () const
	{
		return Resolve(nullptr);
	}

	Class* ClassFindResult::Resolve(Class const * check_class) const
	{
		// something found in classes
		if (result != nullptr)
			return result;

		// something found in aliases
		auto& aliases = Class::GetAliases();
		if (alias_iterator != aliases.end())
		{
			auto alias_it2 = aliases.upper_bound(alias_iterator->first); // search then upper_bound (alias_it is the lower bound)


#if _DEBUG  // in this search we ensure there is no ambiguity
			auto found = aliases.end();
			for (auto it = alias_iterator; it != alias_it2; ++it)
			{
				if (check_class == nullptr || it->second->InheritsFrom(check_class, true) == InheritanceType::YES)
				{
					// continue matching search to ensure this there is no ambiguity
					assert(found == aliases.end());
					found = it;
				}
			}
			if (found != aliases.end())
				return found->second;
#else		// in this search we return the very first matching element
			for (auto it = alias_iterator; it != alias_it2; ++it)
			{
				if (check_class == nullptr || it->second->InheritsFrom(check_class, true) == InheritanceType::YES)
				{
					return it->second;
				}
			}
#endif
		}
		// nothing found
		return nullptr;
	}

	// ==========================================================
	// Class functions
	// ==========================================================

	// XXX: we don't use a class static data but a class static function because we don't know when static data will be created
	//      (and it can happens after static class declaration)

	std::vector<Class*>& Class::GetClasses()
	{
		static std::vector<Class*> classes;
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

	bool Class::IsDeclared() const
	{
		return declared;
	}

	ClassFindResult Class::FindClass(char const* name)
	{
		assert(name != nullptr);

		auto& aliases = GetAliases();

		/* find the class by name first */
		auto it = std::find_if(GetClasses().begin(), GetClasses().end(), [name](Class const * cls)
		{
			return (StringTools::Stricmp(cls->GetClassName(), name) == 0);
		});

		if (it != GetClasses().end())
			return { *it ,GetAliases().end()};

		/* find class by alias */

		return { nullptr, aliases.lower_bound(name) };
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
