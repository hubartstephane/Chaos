namespace chaos
{

#if !defined CHAOS_FORWARD_DECLARATION && !defined CHAOS_TEMPLATE_IMPLEMENTATION

	// ==========================================
	// TMObjectReferenceSolver
	// ==========================================

	/** store wanted reference to further resolving */
	class CHAOS_API TMObjectReferenceEntryClassSolverBase
	{
	public:

		/** main method */
		virtual bool Solve(TMLevelInstance* level_instance, void* weak_ptr_result, int id) { return false; }
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

	class CHAOS_API TMObjectReferenceEntry
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

	class CHAOS_API TMObjectReferenceSolver
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
		static TMObjectReferenceEntryClassSolver<T>* GetClassSolver()
		{
			static TMObjectReferenceEntryClassSolver<T> result;
			return &result;
		}

	protected:

		/** the references to resolve */
		std::vector<TMObjectReferenceEntry> entries;
	};

#elif defined CHAOS_TEMPLATE_IMPLEMENTATION

	// ==========================================
	// TMObjectReferenceEntryClassSolver
	// ==========================================

	template<typename T>
	bool TMObjectReferenceEntryClassSolver<T>::Solve(TMLevelInstance* level_instance, void* weak_ptr_result, int id)
	{
		assert(level_instance != nullptr);
		assert(weak_ptr_result != nullptr);
		// search the object and cast to check if class is correct
		T* object = auto_cast(level_instance->FindObjectByID(id, true));
		if (object == nullptr)
			return false;
		// apply the object to the weak_ptr (after having casting the weak ptr)
		*(weak_ptr<T>*)(weak_ptr_result) = object;
		return true;
	}

	// ==========================================
	// TMObjectReferenceSolver
	// ==========================================

	template<typename T>
	void TMObjectReferenceSolver::DeclareReference(weak_ptr<T>& reference, int id)
	{
		if (id > 0)
		{
			TMObjectReferenceEntry entry;
			entry.weak_ptr_result = &reference;
			entry.id = id;
			entry.class_solver = GetClassSolver<T>();
			entries.push_back(entry);
		}
	}

	/** add a reference to later solve */
	template<typename T>
	void TMObjectReferenceSolver::DeclareReference(weak_ptr<T>& reference, char const* property_name, TiledMap::PropertyOwner const* property_owner)
	{
		int id = property_owner->GetPropertyValueObject(property_name, -1);
		DeclareReference(reference, id);
	}

#endif

}; // namespace chaos