namespace chaos
{
#ifdef CHAOS_FORWARD_DECLARATION

	class ClassManager;

	enum class FindClassFlags;
	CHAOS_DECLARE_ENUM_BITMASK_METHOD(FindClassFlags, CHAOS_API);

#elif !defined CHAOS_TEMPLATE_IMPLEMENTATION

	/**
	 * ClassManager: an object that registered some classes, C++ or JSON
	 */

	enum class FindClassFlags : int
	{
		Name = 1,             // consider name when searching
		Shortname = 2,        // consider shortname when searching
		ParentManager = 4,    // search into child managers
		All = Name | Shortname | ParentManager
	};

	/**
	 * ClassManager: an object that registered some classes, native C++ or with special initialization
	 */

	class CHAOS_API ClassManager : public Object
	{
		friend class ClassFindResult;
		friend class ClassLoader;

		template<typename CPP_TYPE>
		friend class Class;

	public:

		/** constructor */
		ClassManager(ClassManager* in_parent_manager = nullptr);

		/** destructor */
		~ClassManager();

		/** gets the parent manager */
		ClassManager* GetParentManager() { return parent_manager.get(); }
		/** gets the parent manager */
		ClassManager const* GetParentManager() const { return parent_manager.get(); }

		/** search whether manager is an ancestor of another manager */
		bool IsAncestorManagerFor(ClassManager const* child_manager, bool accept_same = true) const;

		/** search for a class with miscelleous criteria. return intermediate for last moment class requirement resolution */
		ClassFindResult FindClass(char const* name, FindClassFlags flags = FindClassFlags::All);

	protected:

		/** insert a new class in this manager */
		void InsertClass(ClassBase* cls);

	protected:

		/* the parent class manager */
		shared_ptr<ClassManager> parent_manager;
		/** the classes owned by this manager */
		std::vector<shared_ptr<ClassBase>> classes;
	};

#endif

}; // namespace chaos