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

	class Renderable : public ReferencedObject, public NamedObject
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

	// ========================================================
	// RenderableLayerInfo : an entry in the RenderableLayer
	// ========================================================

	class RenderableLayerInfo : public NamedObjectWrapper<Renderable>
	{
		friend class RenderableLayer;

	public:

		/** special method for sorted insertion : lower_bound + insert */
		operator int() const { return render_order; }

	public:

		/** the render order */
		int render_order = 0;
	};

	// ========================================================
	// RenderableLayer : used as a sorted container for renderers
	// ========================================================

	class RenderableLayer : public Renderable
	{
	public:

		/** Find a renderable by its name */
		Renderable * FindChildRenderable(char const * name);
		/** Find a renderable by its name */
		Renderable const * FindChildRenderable(char const * name) const;
		/** Find a renderable by its tag */
		Renderable * FindChildRenderable(TagType tag);
		/** Find a renderable by its tag */
		Renderable const * FindChildRenderable(TagType tag) const;
		/** insert a renderable */
		bool AddChildRenderable(Renderable * renderable, int render_order);
		/** remove a renderable */
		bool RemoveChildRenderable(Renderable * renderable);

	protected:

		/** the main rendering method */
		virtual int DoDisplay(GPUProgramProviderBase const * uniform_provider, RenderParams const & render_params) const override;
		/** find a renderable */
		RenderableLayerInfo * FindChildRenderable(Renderable * renderable);
		/** find a renderable */
		RenderableLayerInfo const * FindChildRenderable(Renderable * renderable) const;

	protected:

		/** all child renderable */
		std::vector<RenderableLayerInfo> layers;
	};


#if 0
	// ========================================================
	// ParticleLayer : used as container for particles
	// ========================================================

	class ParticleLayer : public Renderable
	{
	public:

		/** constructor */
		ParticleLayer()


	};
#endif	

	// ========================================================
	// ParticleManager : a container for ParticleLayers
	// ========================================================	

	class Tickable : public ReferencedObject
	{
	public:

		/** change the pause state */
		void SetPause(bool in_pause = true);
		/** get the pause state */
		bool IsPaused() const;
		/** the tick entry point */
		bool Tick(double delta_time);

	protected:

		/** the tick user function */
		virtual bool DoTick(double delta_time);
		/** called whenever object pause state has been changed */
		virtual void OnPauseStateChanged(bool in_pause);

	protected:

		/** the pause state */
		bool paused = false;


	};








}; // namespace chaos
