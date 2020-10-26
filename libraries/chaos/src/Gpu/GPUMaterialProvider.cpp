#include <chaos/Chaos.h>

namespace chaos
{
	// ========================================================
	// GPUMaterialProvider implementation
	// ========================================================

	GPURenderMaterial const * GPUMaterialProvider::GetMaterial(GPURenderable const * renderable, GPURenderMaterial const * default_material, GPURenderParams const & render_params) const
	{
		return default_material;
	}

}; // namespace chaos
