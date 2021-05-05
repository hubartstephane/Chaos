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

}; // namespace chaos
