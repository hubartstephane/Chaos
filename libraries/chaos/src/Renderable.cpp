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
