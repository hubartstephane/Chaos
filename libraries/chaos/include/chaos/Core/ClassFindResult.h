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
		MATCH_NAME,
		MATCH_SHORTNAME
	};

	/**
	 * ClassFindResult : Intermediate object for class searching.
	 *                   A class has a name and may have a short name
	 *                   Inside a ClassManager, there's only a single class for a given name, but there may be several classes with the same shortname
	 *                   ClassManagers are chained with a parent-child relationship
	 *                   Given that, a search inside by (short)name by the ClassManager may produce several results
	 *                   To filter-out some of theses outputs some additionnal information is to be brought: the accepted classes
	 *                   SubClassOf<...> is able to give this additional information to ClassManager for effectively searched class
	 *                   That's why ClassManager's search returns the intermediate object ClassFindResult
	 * 
	 *                   ClassManager::Find(name) -----> ClassFindResult
	 * 
	 *                   ClassFindResult -----> SubClassOf<...>      the full search is delayed until SubClassOf<...> bring some information about searched class
	 * 
	 *                   Under other circumbstances such a ClassFindResult should store the name somehow (probably with a std::string) by there's a hack here
	 *                   to avoid memory allocation:
	 *                   At construction by ClassManager, ClassFindResult stores an iterator on the very first object matching by (short)name the request.
	 *                   When looking for further results the iterator is used to get the name that was used
	 */

	class CHAOS_API ClassFindResult
	{
		friend class ClassManager;

		using iterator_type = std::vector<Class*>::iterator;

	public:

		/** gets the result of the class */
		operator Class* () const;
		/** finalize the search */
		Class* Resolve(Class const* check_class) const;

	protected:

		/** constructor */
		ClassFindResult(ClassManager* in_class_manager, iterator_type in_iterator, ClassMatchType in_match_type);

		/** cache the resolved result */
		mutable Class* result = nullptr;
		/** the class manager where to search */
		mutable ClassManager* class_manager = nullptr;
		/** the very first name matching the request. we can use it for further research instead to store the name somehow (that would be costly) */
		mutable iterator_type iterator;
		/** whether the iterator correspond to a matching name or a matching short name */
		ClassMatchType match_type = ClassMatchType::MATCH_NAME;
	};

#endif

}; // namespace chaos