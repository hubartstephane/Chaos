#pragma once

#include <chaos/StandardHeaders.h>
#include <chaos/GPUClasses.h>

#include <chaos/NamedObject.h>
#include <chaos/ReferencedObject.h>
#include <chaos/GPURenderable.h>

namespace chaos
{
	// ========================================================
	// GPURenderableLayerSystem : used as a sorted container for renderers
	// ========================================================

	class GPURenderableLayerSystem : public GPURenderable
	{
	protected:

		/** an utility class to store a a GPURenderable with a render order */
		class RenderableLayerInfo : public NamedObjectWrapper<GPURenderable>
		{	
		public:

			/** special method for sorted insertion : lower_bound + insert */
			operator int() const { return render_order; }
			/** the render order */
			int render_order = 0;
		};

	public:

		/** Find a renderable by its name */
		GPURenderable * FindChildRenderable(char const * name);
		/** Find a renderable by its name */
		GPURenderable const * FindChildRenderable(char const * name) const;
		/** Find a renderable by its tag */
		GPURenderable * FindChildRenderable(TagType tag);
		/** Find a renderable by its tag */
		GPURenderable const * FindChildRenderable(TagType tag) const;
		/** insert a renderable */
		bool AddChildRenderable(GPURenderable * renderable, int render_order);
		/** remove a renderable */
		bool RemoveChildRenderable(GPURenderable * renderable);

		/** get the number of children */
		size_t GetChildCount() const;
		/** get the child at a given position (not the rendering order) */
		GPURenderable * GetChildAt(size_t index);
		/** get the child at a given position (not the rendering order) */
		GPURenderable const * GetChildAt(size_t index) const;

	protected:

		/** the main rendering method */
		virtual int DoDisplay(GPURenderer * renderer, GPUProgramProviderBase const * uniform_provider, GPURenderParams const & render_params) const override;
		/** find a renderable */
		RenderableLayerInfo * FindChildRenderableInfo(GPURenderable * renderable);
		/** find a renderable */
		RenderableLayerInfo const * FindChildRenderableInfo(GPURenderable * renderable) const;

	protected:

		/** all child renderable */
		std::vector<RenderableLayerInfo> layers;
	};

}; // namespace chaos

