#include <chaos/Renderable.h>

namespace chaos
{

	// ========================================================
	// Renderable implementation
	// ========================================================

	int Renderable::Display(GPUProgramProviderBase const * uniform_provider, InstancingInfo const & instancing) const
	{
		if (!IsVisible())
			return 0;
		return DoDisplay(uniform_provider, instancing);
	}

	void Renderable::Show(bool new_visible)
	{
		if (visible == new_visible)
			return;
		visible = new_visible;
		OnVisibilityChanged(visible);
	}

	bool Renderable::IsVisible() const
	{
		return visible;
	}

	void Renderable::OnVisibilityChanged(bool new_visible)
	{

	}

	// ========================================================
	// RenderableLayer implementation
	// ========================================================	

	int RenderableLayer::DoDisplay(GPUProgramProviderBase const * uniform_provider, InstancingInfo const & instancing) const
	{
		int result = 0;
		for (RenderableLayerInfo const & layer_info : layers)
			result += layer_info.renderable->Display(uniform_provider, instancing);
		return result;
	}

	RenderableLayerInfo * RenderableLayer::FindChildRenderable(Renderable * renderable)
	{
		for (RenderableLayerInfo & info : layers)
			if (info.renderable == renderable)
				return &info;
		return nullptr;
	}

	RenderableLayerInfo const * RenderableLayer::FindChildRenderable(Renderable * renderable) const
	{
		for (RenderableLayerInfo const & info : layers)
			if (info.renderable == renderable)
				return &info;
		return nullptr;
	}

	bool RenderableLayer::AddChildRenderable(Renderable * renderable, int render_order)
	{
		if (FindChildRenderable(renderable) != nullptr)
			return false;

		auto it = std::lower_bound(layers.begin(), layers.end(), render_order); // sorted insertion

		RenderableLayerInfo info;
		info.renderable = renderable;
		info.render_order = render_order;
		layers.insert(it, std::move(info));

		return true;
	}

	bool RenderableLayer::RemoveChildRenderable(Renderable * renderable)
	{
		RenderableLayerInfo * layer_info = FindChildRenderable(renderable);
		if (layer_info == nullptr)
			return false;
		layers.erase((layer_info - &layers[0]) + layers.begin());
		return true;
	}

}; // namespace chaos
