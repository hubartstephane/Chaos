#include <chaos/Renderable.h>

namespace chaos
{
	// ========================================================
	// MaterialProvider implementation
	// ========================================================

	GPURenderMaterial const * MaterialProvider::GetMaterial(Renderable const * renderable, GPURenderMaterial const * default_material, RenderParams const & render_params) const
	{
		return default_material;
	}

	// ========================================================
	// RenderParams implementation
	// ========================================================	

	GPURenderMaterial const * RenderParams::GetMaterial(Renderable const * renderable, GPURenderMaterial const * default_material) const
	{
		if (material_provider == nullptr)
			return default_material;
		return material_provider->GetMaterial(renderable, default_material, *this);
	}

	// ========================================================
	// Renderable implementation
	// ========================================================

	int Renderable::Display(GPUProgramProviderBase const * uniform_provider, RenderParams const & render_params) const
	{
		if (!IsVisible())
			return 0;
		return DoDisplay(uniform_provider, render_params);
	}

	int Renderable::DoDisplay(GPUProgramProviderBase const * uniform_provider, RenderParams const & render_params) const
	{
		return 0;
	}	

	void Renderable::Show(bool in_visible)
	{
		if (visible == in_visible)
			return;
		visible = in_visible;
		OnVisibilityChanged(in_visible);
	}

	bool Renderable::IsVisible() const
	{
		return visible;
	}

	void Renderable::OnVisibilityChanged(bool in_visible)
	{

	}

	// ========================================================
	// RenderableLayer implementation
	// ========================================================	

	int RenderableLayer::DoDisplay(GPUProgramProviderBase const * uniform_provider, RenderParams const & render_params) const
	{
		int result = 0;
		for (RenderableLayerInfo const & layer_info : layers)
			result += layer_info.object->Display(uniform_provider, render_params);
		return result;
	}

	RenderableLayerInfo * RenderableLayer::FindChildRenderable(Renderable * renderable)
	{
		for (RenderableLayerInfo & info : layers)
			if (info.object == renderable)
				return &info;
		return nullptr;
	}

	RenderableLayerInfo const * RenderableLayer::FindChildRenderable(Renderable * renderable) const
	{
		for (RenderableLayerInfo const & info : layers)
			if (info.object == renderable)
				return &info;
		return nullptr;
	}

	bool RenderableLayer::AddChildRenderable(Renderable * renderable, int render_order)
	{
		if (FindChildRenderable(renderable) != nullptr)
			return false;

		auto it = std::lower_bound(layers.begin(), layers.end(), render_order); // sorted insertion

		RenderableLayerInfo info;
		info.object = renderable;
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

	Renderable * RenderableLayer::FindChildRenderable(char const * name)
	{
		RenderableLayerInfo * info = NamedObject::FindNamedObject(layers, name);
		if (info == nullptr)
			return nullptr;
		return info->object.get();
	}

	Renderable const * RenderableLayer::FindChildRenderable(char const * name) const
	{
		RenderableLayerInfo const * info = NamedObject::FindNamedObject(layers, name);
		if (info == nullptr)
			return nullptr;
		return info->object.get();
	}

	Renderable * RenderableLayer::FindChildRenderable(TagType tag)
	{
		RenderableLayerInfo * info = NamedObject::FindNamedObject(layers, tag);
		if (info == nullptr)
			return nullptr;
		return info->object.get();
	}

	Renderable const * RenderableLayer::FindChildRenderable(TagType tag) const
	{
		RenderableLayerInfo const * info = NamedObject::FindNamedObject(layers, tag);
		if (info == nullptr)
			return nullptr;
		return info->object.get();
	}	

	// ========================================================
	// Tickable implementation
	// ========================================================	
	
	void Tickable::SetPause(bool in_pause)
	{
		if (paused == in_pause)
			return;
		paused = in_pause;
		OnPauseStateChanged(in_pause);
	}
		
	bool Tickable::IsPaused() const
	{
		return paused;
	}

	bool Tickable::Tick(double delta_time)
	{
		if (IsPaused())
			return false;
		return DoTick(delta_time);
	}

	bool Tickable::DoTick(double delta_time)
	{
		return true;
	}

	void Tickable::OnPauseStateChanged(bool in_pause)
	{

	}



}; // namespace chaos
