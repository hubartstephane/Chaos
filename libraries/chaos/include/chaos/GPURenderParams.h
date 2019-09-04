#pragma once

#include <chaos/StandardHeaders.h>
#include <chaos/GPUClasses.h>

#include <chaos/GeometryFramework.h>

#include <chaos/GPUMaterialProvider.h>
#include <chaos/GPURenderableFilter.h>
#include <chaos/GPUInstancingInfo.h>

namespace chaos
{

	// ========================================================
	// GPURenderParams : some data for the rendering
	// ========================================================

	class GPURenderParams
	{

	public:

		/** get the material */
		GPURenderMaterial const * GetMaterial(GPURenderable const * renderable, GPURenderMaterial const * default_material) const;

	public:

		/** the viewport */
		chaos::box2 viewport;
		/** the screen size */
		glm::ivec2 screen_size = glm::ivec2(0, 0);
		/** material provider */
		shared_ptr<GPUMaterialProvider> material_provider;
		/** some filters */
		shared_ptr<GPURenderableFilter> object_filter;
		/** material specialization */
		std::string renderpass_name;
		/** the instancing information */
		GPUInstancingInfo instancing;		
	};

}; // namespace chaos