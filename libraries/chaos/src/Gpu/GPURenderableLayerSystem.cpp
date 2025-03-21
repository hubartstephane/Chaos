#include "chaos/ChaosPCH.h"
#include "chaos/ChaosInternals.h"

namespace chaos
{
	// ========================================================
	// GPURenderableLayerSystem implementation
	// ========================================================

	int GPURenderableLayerSystem::DoDisplay(GPURenderContext * render_context, GPUProgramProviderInterface const * uniform_provider, GPURenderParams const & render_params)
	{
		int result = 0;
		for (RenderableLayerInfo const & layer_info : layers)
			result += layer_info.object->Display(render_context, uniform_provider, render_params);
		return result;
	}

	GPURenderableLayerSystem::RenderableLayerInfo * GPURenderableLayerSystem::FindChildRenderableInfo(GPURenderable * renderable)
	{
		for (RenderableLayerInfo & info : layers)
			if (info.object == renderable)
				return &info;
		return nullptr;
	}

	GPURenderableLayerSystem::RenderableLayerInfo const * GPURenderableLayerSystem::FindChildRenderableInfo(GPURenderable * renderable) const
	{
		for (RenderableLayerInfo const & info : layers)
			if (info.object == renderable)
				return &info;
		return nullptr;
	}

	bool GPURenderableLayerSystem::AddChildRenderable(GPURenderable * renderable, int render_order)
	{
		if (FindChildRenderableInfo(renderable) != nullptr)
			return false;

		auto it = std::lower_bound(layers.begin(), layers.end(), render_order); // sorted insertion

		RenderableLayerInfo info;
		info.object = renderable;
		info.render_order = render_order;
		layers.insert(it, std::move(info));

		return true;
	}

	bool GPURenderableLayerSystem::RemoveChildRenderable(GPURenderable * renderable)
	{
		RenderableLayerInfo * layer_info = FindChildRenderableInfo(renderable);
		if (layer_info == nullptr)
			return false;
		layers.erase((layer_info - &layers[0]) + layers.begin());
		return true;
	}

	GPURenderable * GPURenderableLayerSystem::FindChildRenderable(ObjectRequest request)
	{
		RenderableLayerInfo * info = request.FindObject(layers);
		if (info == nullptr)
			return nullptr;
		return info->object.get();
	}

	GPURenderable const * GPURenderableLayerSystem::FindChildRenderable(ObjectRequest request) const
	{
		RenderableLayerInfo const * info = request.FindObject(layers);
		if (info == nullptr)
			return nullptr;
		return info->object.get();
	}

	size_t GPURenderableLayerSystem::GetChildCount() const
	{
		return layers.size();
	}
	GPURenderable * GPURenderableLayerSystem::GetChildAt(size_t index)
	{
		return layers[index].object.get();
	}

	GPURenderable const * GPURenderableLayerSystem::GetChildAt(size_t index) const
	{
		return layers[index].object.get();
	}

}; // namespace chaos
