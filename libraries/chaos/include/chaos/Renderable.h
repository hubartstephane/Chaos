#pragma once

#include <chaos/StandardHeaders.h>
#include <chaos/NamedObject.h>
#include <chaos/ReferencedObject.h>
#include <chaos/DrawPrimitive.h>
#include <chaos/GPUProgramProvider.h>

namespace chaos
{
	// ========================================================
	// Renderable : base class for all object that can be rendered
	// ========================================================

	class Renderable : public ReferencedObject, public NamedObject
	{
	public:

		/** public method to render the object */
		int Display(GPUProgramProviderBase const * uniform_provider, InstancingInfo const & instancing = InstancingInfo()) const;

		/** show or hide the object */
		void Show(bool new_visible = true);
		/** returns whether the object is visible or not */
		bool IsVisible() const;

	protected:

		/** the user defined method to display the object */
		virtual int DoDisplay(GPUProgramProviderBase const * uniform_provider, InstancingInfo const & instancing) const;
		/** called whenever object visibility has been changed */
		virtual void OnVisibilityChanged(bool new_visible);

	public:

		/** whether the object is hidden or visible */
		bool visible = true;
	};

	// ========================================================
	// RenderableLayerInfo : an entry in the RenderableLayer
	// ========================================================

	class RenderableLayerInfo
	{
		friend class RenderableLayer;

	public:

		/** special method for sorted insertion : lower_bound + insert */
		operator int() const { return render_order; }
		/** special method to have access to NamedObject static utility functions */
		char const * GetName() const { return renderable->GetName(); }
		/** special method to have access to NamedObject static utility functions */
		TagType GetTag() const { return renderable->GetTag(); }

	public:

		/** the renderable to render */
		boost::intrusive_ptr<Renderable> renderable;
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
		virtual int DoDisplay(GPUProgramProviderBase const * uniform_provider, InstancingInfo const & instancing) const override;
		/** find a renderable */
		RenderableLayerInfo * FindChildRenderable(Renderable * renderable);
		/** find a renderable */
		RenderableLayerInfo const * FindChildRenderable(Renderable * renderable) const;

	protected:

		/** all child renderable */
		std::vector<RenderableLayerInfo> layers;
	};
}; // namespace chaos
