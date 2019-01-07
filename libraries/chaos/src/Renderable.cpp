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

	int Renderable::Display(Renderer * renderer, GPUProgramProviderBase const * uniform_provider, RenderParams const & render_params) const
	{
		assert(renderer != nullptr);
		if (!IsVisible())
			return 0;
		// filter object
		if (render_params.object_filter != nullptr)
			if (!render_params.object_filter->CanRender(this))
				return 0;
		// update it (only if necessary
		if (render_params.timestamp == 0 || update_timestamp != render_params.timestamp) // test for 0 to ensure resource is updated even if caller does not care about updating a timestamp
		{
			if (!DoUpdateGPUResources())
				return 0;
			update_timestamp = render_params.timestamp;
		}
		return DoDisplay(renderer, uniform_provider, render_params);
	}

	int Renderable::DoDisplay(Renderer * renderer, GPUProgramProviderBase const * uniform_provider, RenderParams const & render_params) const
	{
		return 0;
	}	

	bool Renderable::DoUpdateGPUResources() const
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

}; // namespace chaos
