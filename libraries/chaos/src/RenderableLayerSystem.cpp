#include <chaos/RenderableLayerSystem.h>

namespace chaos
{
	// ========================================================
	// RenderableLayerSystem implementation
	// ========================================================	

	int RenderableLayerSystem::DoDisplay(GPURenderer * renderer, GPUProgramProviderBase const * uniform_provider, RenderParams const & render_params) const
	{
		int result = 0;
		for (RenderableLayerInfo const & layer_info : layers)
			result += layer_info.object->Display(renderer, uniform_provider, render_params);
		return result;
	}

	RenderableLayerSystem::RenderableLayerInfo * RenderableLayerSystem::FindChildRenderableInfo(Renderable * renderable)
	{
		for (RenderableLayerInfo & info : layers)
			if (info.object == renderable)
				return &info;
		return nullptr;
	}

	RenderableLayerSystem::RenderableLayerInfo const * RenderableLayerSystem::FindChildRenderableInfo(Renderable * renderable) const
	{
		for (RenderableLayerInfo const & info : layers)
			if (info.object == renderable)
				return &info;
		return nullptr;
	}

	bool RenderableLayerSystem::AddChildRenderable(Renderable * renderable, int render_order)
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

	bool RenderableLayerSystem::RemoveChildRenderable(Renderable * renderable)
	{
		RenderableLayerInfo * layer_info = FindChildRenderableInfo(renderable);
		if (layer_info == nullptr)
			return false;
		layers.erase((layer_info - &layers[0]) + layers.begin());
		return true;
	}

	Renderable * RenderableLayerSystem::FindChildRenderable(char const * name)
	{
		RenderableLayerInfo * info = NamedObject::FindNamedObject(layers, name);
		if (info == nullptr)
			return nullptr;
		return info->object.get();
	}

	Renderable const * RenderableLayerSystem::FindChildRenderable(char const * name) const
	{
		RenderableLayerInfo const * info = NamedObject::FindNamedObject(layers, name);
		if (info == nullptr)
			return nullptr;
		return info->object.get();
	}

	Renderable * RenderableLayerSystem::FindChildRenderable(TagType tag)
	{
		RenderableLayerInfo * info = NamedObject::FindNamedObject(layers, tag);
		if (info == nullptr)
			return nullptr;
		return info->object.get();
	}

	Renderable const * RenderableLayerSystem::FindChildRenderable(TagType tag) const
	{
		RenderableLayerInfo const * info = NamedObject::FindNamedObject(layers, tag);
		if (info == nullptr)
			return nullptr;
		return info->object.get();
	}	

	size_t RenderableLayerSystem::GetChildCount() const
	{
		return layers.size();
	}
	Renderable * RenderableLayerSystem::GetChildAt(size_t index)
	{
		return layers[index].object.get();
	}

	Renderable const * RenderableLayerSystem::GetChildAt(size_t index) const
	{
		return layers[index].object.get();
	}

}; // namespace chaos
