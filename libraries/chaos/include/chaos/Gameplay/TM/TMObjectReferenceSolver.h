#ifdef CHAOS_FORWARD_DECLARATION

#else

namespace chaos
{
	// ==========================================
	// TMObjectReferenceSolver : store wanted reference to further resolving
	// ==========================================

	class TMObjectReferenceEntryClassSolverBase
	{
	public:

		/** main method */
		virtual bool Solve(TMLevelInstance * level_instance, void* weak_ptr_result, int id) { return false; }
	};

	template<typename T>
	class TMObjectReferenceEntryClassSolver : public TMObjectReferenceEntryClassSolverBase
	{
	public:

		/** override */
		virtual bool Solve(TMLevelInstance * level_instance, void* weak_ptr_result, int id) override
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
	};

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

	class TMObjectReferenceSolver
	{
	public:

		/** add a reference to later solve */
		template<typename T>
		void DeclareReference(weak_ptr<T>& reference, int id)
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
		void DeclareReference(weak_ptr<T>& reference, char const* property_name, TiledMap::PropertyOwner const* property_owner)
		{
			int id = property_owner->GetPropertyValueObject(property_name, -1);
			DeclareReference(reference, id);
		}
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