#include "chaos/ChaosPCH.h"
#include "chaos/ChaosInternals.h"

namespace chaos
{
	// ========================================================================
	// GPUAtlas implementation
	// ========================================================================

	void GPUAtlas::Clear()
	{
		AtlasBase::Clear();
		texture = nullptr;
	}

}; // namespace chaos

