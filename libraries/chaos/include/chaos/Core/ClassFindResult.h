namespace chaos
{
#ifdef CHAOS_FORWARD_DECLARATION

	class ClassFindResult;
	enum class ClassMatchType;

#elif !defined CHAOS_TEMPLATE_IMPLEMENTATION

	/**
	 * ClassMatchType: whether the iterator of ClassFindResult has been found with a name or shortname matching
	 */

	enum class ClassMatchType : int
	{
		Name,
		Shortname
	};

	/**
	 * ClassFindResult: utility class for search result from ClassManager. The purpose is to delay as far as possible type checking
	 * 
	 *                  A class has a name and may have a short name
	 *                  Inside a ClassManager, there's only a single class for a given name, but there may be several classes with the same shortname
	 *                  ClassManagers are chained with a parent-child relationship (and parent managers may have classes with the same name)
	 *                  Given that, a search inside by (short)name by the ClassManager may produce several results
	 * 
	 *                  To filter-out some of theses outputs some additionnal information is to be brought: the accepted classes
	 *                  At search, because the searched type is still unknown yet, we could search for all possible classes matching for (short)name
	 *                  This would consume more resource than wanted
	 * 
	 *                  The solution is to delay the effective search until the real type is known
	 *                  The initial search only produce the very first class matching naming requirement
	 *                  At resolution, we continue iterating for next classes if necessary
	 *
	 *                  Under other circumbstances such a ClassFindResult should store the name somehow (probably with a std::string) by there's a hack here
	 *                  to avoid memory allocation:
	 *                  ClassFindResult stores an iterator on the very first object matching by (short)name the request (see ClassManager::Find(...) )
	 *                  When looking for further results the iterator is used to get the name that was used
	 */

	class CHAOS_API ClassFindResult
	{
		friend class ClassManager;

		using iterator_type = std::vector<shared_ptr<ClassBase>>::iterator;

	public:

		/** casting result into the effective Class<..> */
		template<typename CPP_TYPE>
		operator Class<CPP_TYPE>* () const;

		/** casting result into a ClassBase */
		operator ClassBase* () const;

		/** check whether the result is empty */
		bool operator == (nullptr_t) const;
		/** check whether the result is empty */
		bool operator != (nullptr_t) const;

	protected:

		/** constructor */
		ClassFindResult(ClassManager* in_class_manager, iterator_type in_iterator, ClassMatchType in_match_type, FindClassFlags in_find_flags);

	protected:

		/** cache the resolved result */
		mutable ClassBase* result = nullptr;
		/** the class manager where to search */
		mutable ClassManager* class_manager = nullptr;
		/** the very first name matching the request. we can use it for further research instead to store the name somehow (that would be costly) */
		mutable iterator_type iterator;
		/** whether the iterator correspond to a matching name or a matching short name */
		ClassMatchType match_type = ClassMatchType::Name;
		/** the flags used during the search */
		FindClassFlags find_flags = FindClassFlags::All;
	};

#else

	template<typename CPP_TYPE>
	ClassFindResult::operator Class<CPP_TYPE>* () const
	{
		// check for cached result (class_manager == nullptr -> can use result as a cached result (even if null))
		if (result != nullptr || class_manager == nullptr)
			return (Class<CPP_TYPE>*)result;

		// check for the very first entry (string comparaison already done)
		if ((*iterator)->InheritsFrom<CPP_TYPE>(true))
		{
			result = iterator->get();
			class_manager = nullptr;
			return (Class<CPP_TYPE>*)result;
		}

		// we know that the iterator points on a valid entry. get the string that made this entry a good one
		std::string const& searched_name = (match_type == ClassMatchType::Name) ?
			iterator->get()->GetClassName() :
			iterator->get()->GetShortName();

		++iterator; // no need to test again for this iterator

		if (iterator == class_manager->classes.end())
		{
			if (class_manager->GetParentManager() == nullptr || !HasAnyFlags(find_flags, FindClassFlags::ParentManager))
			{
				result = nullptr;
				class_manager = nullptr;
				return nullptr;
			}
			class_manager = class_manager->GetParentManager();
			iterator = class_manager->classes.begin();
		}

		// search in manager chain
		while (class_manager != nullptr)
		{
			while (iterator != class_manager->classes.end())
			{
				if (HasAnyFlags(find_flags, FindClassFlags::Name))
				{
					if (StringTools::Stricmp((*iterator)->GetClassName(), searched_name) == 0)
					{
						if ((*iterator)->InheritsFrom<CPP_TYPE>(true))
						{
							result = iterator->get(); // cache the result for next call
							return (Class<CPP_TYPE>*)result;
						}
						++iterator;
						continue; // the class does not match no matter what. no need to check for the short name
					}
				}
				if (HasAnyFlags(find_flags, FindClassFlags::Shortname))
				{
					if (StringTools::Stricmp((*iterator)->GetShortName(), searched_name) == 0)
					{
						if ((*iterator)->InheritsFrom<CPP_TYPE>(true))
						{
							result = iterator->get(); // cache the result for next call
							return (Class<CPP_TYPE>*)result;
						}
					}
				}
				++iterator;
			}
			if (!HasAnyFlags(find_flags, FindClassFlags::ParentManager))
				break;

			class_manager = class_manager->GetParentManager();
			if (class_manager == nullptr)
				break;

			iterator = class_manager->classes.begin();
		}
		class_manager = nullptr; // so we have early result next time this function is called
		return nullptr;
	};

#endif

}; // namespace chaos