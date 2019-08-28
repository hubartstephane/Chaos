#pragma once

#include <chaos/StandardHeaders.h>
#include <chaos/GPUClasses.h>

#include <chaos/ReferencedObject.h>

namespace chaos
{
	// ========================================================
	// GPUMaterialProvider : used to override the material used
	// ========================================================	

	class GPUMaterialProvider : public ReferencedObject
	{
	public:

		/** the material provider main method */
		virtual GPURenderMaterial const * GetMaterial(GPURenderable const * renderable, GPURenderMaterial const * default_material, GPURenderParams const & render_params) const;
	};	

}; // namespace chaos
