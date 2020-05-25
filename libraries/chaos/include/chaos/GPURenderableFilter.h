#pragma once

#include <chaos/StandardHeaders.h>
#include <chaos/GPUClasses.h>

#include <chaos/Object.h>
#include <chaos/NamedObject.h>

namespace chaos
{
	// ========================================================
	// GPURenderableFilter : filter-out some objects frome rendering
	// ========================================================

	class GPURenderableFilter : public Object
	{
	public:

		/** check whether given object can be rendered */
		virtual bool CanRender(GPURenderable const * renderable) const;

	public:

		/** the filter to apply */
		NamedObjectFilterList name_filter;
	};

	// ========================================================
	// GPUTypedRenderableFilter : filter-out some objects frome rendering
	// ========================================================

	template<typename T>
	class GPUTypedRenderableFilter : public GPURenderableFilter
	{
	public:

		/** override */
		virtual bool CanRender(class GPURenderable const * renderable) const override
		{
			assert(renderable != nullptr);
			// check whether the typed object can be filtered out from its class
			T const * typed_object = auto_cast(renderable);
			if (typed_object == nullptr)
				return true;
			// check from name
			return GPURenderableFilter::CanRender(renderable);
		}
	};

	// ========================================================
	// GPUParticleLayerFilterList : filter-out some objects frome rendering
	// ========================================================
		
	using GPUParticleLayerFilterList = GPUTypedRenderableFilter<class ParticleLayerBase>;

}; // namespace chaos
