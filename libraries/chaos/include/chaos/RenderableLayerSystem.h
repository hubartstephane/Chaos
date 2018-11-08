#pragma once

#include <chaos/StandardHeaders.h>
#include <chaos/NamedObject.h>
#include <chaos/ReferencedObject.h>
#include <chaos/Renderable.h>

namespace chaos
{
	// ========================================================
	// RenderableLayerSystem : used as a sorted container for renderers
	// ========================================================

	class RenderableLayerSystem : public Renderable
	{
	protected:

		/** an utility class to store a a Renderable with a render order */
		class RenderableLayerInfo : public NamedObjectWrapper<Renderable>
		{	
		public:

			/** special method for sorted insertion : lower_bound + insert */
			operator int() const { return render_order; }
			/** the render order */
			int render_order = 0;
		};

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

		/** get the number of children */
		size_t GetChildCount() const;
		/** get the child at a given position (not the rendering order) */
		Renderable * GetChildAt(size_t index);
		/** get the child at a given position (not the rendering order) */
		Renderable const * GetChildAt(size_t index) const;

	protected:

		/** the main rendering method */
		virtual int DoDisplay(GPUProgramProviderBase const * uniform_provider, RenderParams const & render_params) const override;
		/** find a renderable */
		RenderableLayerInfo * FindChildRenderableInfo(Renderable * renderable);
		/** find a renderable */
		RenderableLayerInfo const * FindChildRenderableInfo(Renderable * renderable) const;

	protected:

		/** all child renderable */
		std::vector<RenderableLayerInfo> layers;
	};

}; // namespace chaos
