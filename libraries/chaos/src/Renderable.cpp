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

	int Renderable::Display(GPURenderer * renderer, GPUProgramProviderBase const * uniform_provider, RenderParams const & render_params) const
	{
		assert(renderer != nullptr);
		if (!IsVisible())
			return 0;
		// internal filters
		if (!IsRenderPassEnabled(render_params.renderpass_name.c_str()))
			return 0;
		// filter object
		if (render_params.object_filter != nullptr)
			if (!render_params.object_filter->CanRender(this))
				return 0;
		// update it (only if necessary
		uint64_t renderer_timestamp = renderer->GetTimestamp();
		if (renderer_timestamp == 0 || update_timestamp != renderer_timestamp) // test for 0 to ensure resource is updated even if caller does not care about updating a timestamp
		{
			if (!DoUpdateGPUResources(renderer))
				return 0;
			update_timestamp = renderer_timestamp;
		}
		return DoDisplay(renderer, uniform_provider, render_params);
	}

	int Renderable::DoDisplay(GPURenderer * renderer, GPUProgramProviderBase const * uniform_provider, RenderParams const & render_params) const
	{
		return 0;
	}	

	bool Renderable::DoUpdateGPUResources(GPURenderer * renderer) const
	{
		return true;
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

	bool Renderable::CanTick()
	{
		if (!Tickable::CanTick())
			return false;
		if (!visible && !tick_hidden)
			return false;
		return true;
	}

	bool Renderable::CanTickIfHidden() const
	{
		return tick_hidden;
	}

	void Renderable::SetCanTickIfHidden(bool in_tick_hidden)
	{
		tick_hidden = in_tick_hidden;
	}

	bool Renderable::IsRenderPassEnabled(char const * renderpass_name) const
	{
		return renderpass_filter.IsNameEnabled(renderpass_name);
	}
	
	void Renderable::AddEnabledRenderPasses(char const * renderpass_names)
	{
		renderpass_filter.AddEnabledNames(renderpass_names);
	}

	void Renderable::AddDisabledRenderPasses(char const * renderpass_names)
	{
		renderpass_filter.AddDisabledNames(renderpass_names);
	}

	void Renderable::RemoveEnabledRenderPasses(char const * renderpass_names)
	{
		renderpass_filter.RemoveEnabledNames(renderpass_names);
	}

	void Renderable::RemoveDisabledRenderPasses(char const * renderpass_names)
	{
		renderpass_filter.RemoveDisabledNames(renderpass_names);
	}

}; // namespace chaos
