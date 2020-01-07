#include <chaos/GPURenderParams.h>

namespace chaos
{
	// ========================================================
	// GPURenderParams implementation
	// ========================================================

	GPURenderMaterial const * GPURenderParams::GetMaterial(GPURenderable const * renderable, GPURenderMaterial const * default_material) const
	{
        if (material_provider != nullptr)
        {
            GPURenderMaterial const* result = material_provider->GetMaterial(renderable, default_material, *this);
            if (result != nullptr)
                return result;
        }
        return default_material;
	}

}; // namespace chaos
