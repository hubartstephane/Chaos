#pragma once

#include <chaos/StandardHeaders.h>
#include <chaos/NamedObject.h>
#include <chaos/ReferencedObject.h>
#include <chaos/DrawPrimitive.h>
#include <chaos/GPUProgramProvider.h>
#include <chaos/GPURenderMaterial.h>

namespace chaos
{
	class MaterialProvider;
	class RenderParams;
	class Renderable;


	// ========================================================
	// RenderParams : some data for the rendering
	// ========================================================

	class RenderParams
	{

	public:

		/** get the material */
		GPURenderMaterial const * GetMaterial(Renderable const * renderable, GPURenderMaterial const * default_material) const;

	public:

		/** material provider */
		boost::intrusive_ptr<MaterialProvider> material_provider;
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
		virtual GPURenderMaterial const * GetMaterial(Renderable const * renderable, GPURenderMaterial const * default_material, RenderParams const & render_params = RenderParams()) const;
	};	


	// ========================================================
	// Renderable : base class for all object that can be rendered
	// ========================================================

	class Renderable : public virtual ReferencedObject, public virtual NamedObject
	{
	public:

		/** public method to render the object */
		int Display(GPUProgramProviderBase const * uniform_provider, RenderParams const & render_params = RenderParams()) const;

		/** show or hide the object */
		void Show(bool in_visible = true);
		/** returns whether the object is visible or not */
		bool IsVisible() const;

	protected:

		/** the user defined method to display the object */
		virtual int DoDisplay(GPUProgramProviderBase const * uniform_provider, RenderParams const & render_params) const;
		/** called whenever object visibility has been changed */
		virtual void OnVisibilityChanged(bool in_visible);

	public:

		/** whether the object is hidden or visible */
		bool visible = true;
	};

}; // namespace chaos
