#ifdef CHAOS_FORWARD_DECLARATION

#else

namespace chaos
{

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

}; // namespace chaos

#endif // CHAOS_FORWARD_DECLARATION