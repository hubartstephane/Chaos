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
	 *                   While there may be several classes with the same short_name, we have to select the good one when searching
	 *                   We so want that the search is affected by the Subclass affectation that requires it.
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