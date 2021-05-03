#include <chaos/Chaos.h>

namespace chaos
{
	// =====================================
	// TMObjectReferenceSolver implementation
	// =====================================

	void TMObjectReferenceSolver::SolveReferences(TMLevelInstance* level_instance)
	{
		assert(level_instance != nullptr);
		for (auto& entry : entries)
			entry.class_solver->Solve(level_instance, entry.weak_ptr_result, entry.id);
	}

	// =====================================
	// TMObjectReferenceEntryClassSolver implementation
	// =====================================

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

}; // namespace chaos
