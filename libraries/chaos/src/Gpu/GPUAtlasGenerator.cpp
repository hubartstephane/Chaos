#include "chaos/ChaosPCH.h"
#include "chaos/ChaosInternals.h"

namespace chaos
{
	// ========================================================================
	// GPUAtlasGenerator implementation
	// ========================================================================

	GPUAtlas* GPUAtlasGenerator::ComputeResult(AtlasInput const& in_input, AtlasGeneratorParams const& in_params)
	{
		// generate a standard atlas to be converted
		Atlas          atlas;
		AtlasGenerator generator;
		if (!generator.ComputeResult(in_input, atlas, in_params))
			return nullptr;

		// generate texture Atlas
		GPUAtlas* result = new GPUAtlas;
		if (result == nullptr)
			return nullptr;
		if (!result->LoadFromBitmapAtlas(std::move(atlas)))
		{
			delete(result);
			return nullptr;
		}
		return result;
	}

}; // namespace chaos

