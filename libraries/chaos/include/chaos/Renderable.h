#pragma once

#include <chaos/StandardHeaders.h>
#include <chaos/NamedObject.h>
#include <chaos/ReferencedObject.h>
#include <chaos/Tickable.h>
#include <chaos/DrawPrimitive.h>
#include <chaos/GPUProgramProvider.h>
#include <chaos/GPURenderMaterial.h>
#include <chaos/Renderer.h>

namespace chaos
{
	class MaterialProvider;
	class RenderParams;
	class Renderable;


	// ========================================================
	// RenderableObjectFilter : filter-out some objects frome rendering
	// ========================================================

	class RenderableObjectFilter : public ReferencedObject
	{
	public:

		/** check whether given object can be rendered */
		virtual bool CanRender(class Renderable const * renderable) const { return true;}	
	};

	// ========================================================
	// RenderableObjectFilter : filter-out some objects frome rendering
	// ========================================================

	template<typename T>
	class TypedRenderableObjectFilter : public RenderableObjectFilter
	{
	public:

		/** override */
		virtual bool CanRender(class Renderable const * renderable) const override
		{
			assert(renderable != nullptr);
			// check whether the typed object can be filtered out from its class
			T const * typed_object = dynamic_cast<T const *>(renderable);
			if (typed_object == nullptr)
				return true;
			// check from name
			return name_filter.CheckName(*renderable);
		}	

		/** the filter to apply */
		NamedObjectFilterList name_filter;	
	};
		
	using ParticleLayerFilterList = TypedRenderableObjectFilter<class ParticleLayer>;

	// ========================================================
	// RenderParams : some data for the rendering
	// ========================================================

	class RenderParams
	{

	public:

		/** get the material */
		GPURenderMaterial const * GetMaterial(Renderable const * renderable, GPURenderMaterial const * default_material) const;

	public:

		/** the viewport */
		chaos::box2 viewport;
		/** the screen size */
		glm::ivec2 screen_size = glm::ivec2(0, 0);
		/** a time stamp */
		uint64_t timestamp = 0;
		/** material provider */
		shared_ptr<MaterialProvider> material_provider;
		/** some filters */
		shared_ptr<RenderableObjectFilter> object_filter;
		/** the instancing information */
		InstancingInfo instancing;		
	};

	// ========================================================
	// MaterialProvider : used to override the material used
	// ========================================================	

	class MaterialProvider : public ReferencedObject
	{
	public:

		/** the material provider main method */
		virtual GPURenderMaterial const * GetMaterial(Renderable const * renderable, GPURenderMaterial const * default_material, RenderParams const & render_params) const;
	};	


	// ========================================================
	// Renderable : base class for all object that can be rendered
	// ========================================================

	class Renderable : public Tickable
	{
	public:

		/** public method to render the object */
		int Display(Renderer * renderer, GPUProgramProviderBase const * uniform_provider, RenderParams const & render_params) const;

		/** show or hide the object */
		void Show(bool in_visible = true);
		/** returns whether the object is visible or not */
		bool IsVisible() const;

		/** whether the object can be ticked if hidden */
		bool CanTickIfHidden() const;
		/** change whether the object can be ticked if hidden flag */
		void SetCanTickIfHidden(bool in_tick_hidden);

	protected:

		/** override */
		virtual bool CanTick() override;
		/** the user defined method to display the object */
		virtual int DoDisplay(Renderer * renderer, GPUProgramProviderBase const * uniform_provider, RenderParams const & render_params) const;
		/** called to update the graphic resource */
		virtual bool DoUpdateGPUResources() const;
		/** called whenever object visibility has been changed */
		virtual void OnVisibilityChanged(bool in_visible);

	protected:

		/** whether the object is hidden or visible */
		bool visible = true;
		/** whether the object must be ticked if hidden */
		bool tick_hidden = false;
		/** the last time the resource has been updated */
		mutable uint64_t update_timestamp = 0;
	};

}; // namespace chaos
