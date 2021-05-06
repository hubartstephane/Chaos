#ifdef CHAOS_FORWARD_DECLARATION

#elif defined CHAOS_TEMPLATE_IMPLEMENTATION


#else

namespace chaos
{
	// ==========================================
	// TMObjectReferenceSolver
	// ==========================================

	/** store wanted reference to further resolving */
	class TMObjectReferenceEntryClassSolverBase
	{
	public:

		/** main method */
		virtual bool Solve(TMLevelInstance * level_instance, void* weak_ptr_result, int id) { return false; }
	};

	// ==========================================
	// TMObjectReferenceEntryClassSolver
	// ==========================================

	template<typename T>
	class TMObjectReferenceEntryClassSolver : public TMObjectReferenceEntryClassSolverBase
	{
	public:

		/** override */
		virtual bool Solve(TMLevelInstance* level_instance, void* weak_ptr_result, int id) override;
	};

	// ==========================================
	// TMObjectReferenceEntry
	// ==========================================

	class TMObjectReferenceEntry
	{
	public:

		/** where to store the data */
		void* weak_ptr_result = nullptr;
		/** the index to search */
		int id = 0;
		/** the solver for reference */
		TMObjectReferenceEntryClassSolverBase* class_solver = nullptr;
	};

	// ==========================================
	// TMObjectReferenceSolver
	// ==========================================

	class TMObjectReferenceSolver
	{
	public:

		/** add a reference to later solve */
		template<typename T>
		void DeclareReference(weak_ptr<T>& reference, int id);
		/** add a reference to later solve */
		template<typename T>
		void DeclareReference(weak_ptr<T>& reference, char const* property_name, TiledMap::PropertyOwner const* property_owner);
		/** solving all references */
		void SolveReferences(TMLevelInstance* level_instance);

	protected:

		/** get the class solver (this is a singleton) */
		template<typename T>
		static TMObjectReferenceEntryClassSolver<T> * GetClassSolver()
		{
			static TMObjectReferenceEntryClassSolver<T> result;
			return &result;
		}

	protected:

		/** the references to resolve */
		std::vector<TMObjectReferenceEntry> entries;
	};

}; // namespace chaos

#endif // CHAOS_FORWARD_DECLARATION