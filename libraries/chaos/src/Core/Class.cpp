#include "chaos/ChaosPCH.h"
#pragma once

#include <chaos/chaos.h>

namespace chaos
{
	// ==========================================================
	// ClassRegistration functions
	// ==========================================================

	ClassRegistration & ClassRegistration::operator()(std::string in_short_name)
	{
		assert(!StringTools::IsEmpty(in_short_name));
		class_ptr->SetShortName(std::move(in_short_name));
		return *this;
	}

	// ==========================================================
	// ClassFindResult functions
	// ==========================================================

	ClassFindResult::ClassFindResult(Class* in_result, short_name_iterator_type in_short_name_iterator) :
		result(in_result),
		short_name_iterator(in_short_name_iterator)
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

		// something found in aliases (but this may be the wrong type)
		// the first iterator is helpfull to keep trace of the searched name
		auto& classes = Class::GetClasses();

		if (short_name_iterator != classes.end())
		{
			std::string const& searched_name = (*short_name_iterator)->GetShortName();

			auto found_it = classes.end();

			for (auto it = short_name_iterator; it != classes.end() ; ++it)
			{
				if (StringTools::Stricmp(searched_name, (*it)->GetShortName()) == 0)
				{
					if (check_class == nullptr || (*it)->InheritsFrom(check_class, true) == InheritanceType::YES)
					{
#if _DEBUG  // ensure no ambiguity in debug and continue the search
						assert(found_it == classes.end());
						found_it = it;
#else
						return *it;
#endif
					}
				}
			}

#if _DEBUG
			if (found_it != classes.end())
				return *found_it;
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

		auto& classes = GetClasses();

		/** early exit */
		if (StringTools::IsEmpty(name))
			return {nullptr, classes.end()};

		/* find the class by name first */
		{
			auto it = std::ranges::find_if(classes, [name](Class const * cls)
			{
				return (StringTools::Stricmp(cls->GetClassName(), name) == 0);
			});
			if (it != classes.end())
				return { *it ,classes.end()};
		}
		/* find class by short name */
		{
			auto it = std::ranges::find_if(classes, [name](Class const * cls)
			{
				return (StringTools::Stricmp(cls->GetShortName(), name) == 0);
			});
			// note: the iterator points on a class that have the same short name as the one been searched
			// (this may nevertheless not be the one searched)
			// the ClassFindResult will continue the search if the class does not match and will use the short name accessed throught this iterator
			// (to avoid an extra allocating for the searched name)
			if (it != classes.end())
				return { nullptr , it};
		}
		/* no class, no possible alias */
		return { nullptr, classes.end()};
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

	void Class::SetShortName(std::string in_short_name)
	{
		assert(StringTools::IsEmpty(short_name));
		assert(!StringTools::IsEmpty(in_short_name));
		short_name = std::move(in_short_name);
	}

}; // namespace chaos
