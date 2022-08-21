#include "chaos/Chaos.h"

namespace chaos
{

	// ========================================================
	// GPURenderable implementation
	// ========================================================

	int GPURenderable::Display(GPURenderer * renderer, GPUProgramProviderInterface const * uniform_provider, GPURenderParams const & render_params)
	{
		assert(renderer != nullptr);
		if (!PrepareDisplay(renderer, uniform_provider, render_params))
			return 0;
		return DoDisplay(renderer, uniform_provider, render_params);
	}

	bool GPURenderable::PrepareDisplay(GPURenderer* renderer, GPUProgramProviderInterface const* uniform_provider, GPURenderParams const& render_params)
	{
		if (!IsVisible())
			return false;
		// internal filters
		if (!IsRenderPassEnabled(render_params.renderpass_name.c_str()))
			return false;
		// filter object
		if (render_params.object_filter != nullptr)
			if (!render_params.object_filter->CanRender(this))
				return false;
		// update it (only if necessary
		uint64_t renderer_timestamp = renderer->GetTimestamp();
		if (renderer_timestamp == 0 || update_timestamp != renderer_timestamp) // test for 0 to ensure resource is updated even if caller does not care about updating a timestamp
		{
			if (!DoUpdateGPUResources(renderer))
				return false;
			update_timestamp = renderer_timestamp;
		}
		return true;
	}

	int GPURenderable::DoDisplay(GPURenderer * renderer, GPUProgramProviderInterface const * uniform_provider, GPURenderParams const & render_params)
	{
		return 0;
	}

	bool GPURenderable::DoUpdateGPUResources(GPURenderer * renderer)
	{
		return true;
	}

	void GPURenderable::Show(bool in_visible)
	{
		if (visible == in_visible)
			return;
		visible = in_visible;
		OnVisibilityChanged(in_visible);
	}

	bool GPURenderable::IsVisible() const
	{
		return visible;
	}

	void GPURenderable::OnVisibilityChanged(bool in_visible)
	{

	}

	bool GPURenderable::CanTick()
	{
		if (!Tickable::CanTick())
			return false;
		if (!visible && !tick_hidden)
			return false;
		return true;
	}

	bool GPURenderable::CanTickIfHidden() const
	{
		return tick_hidden;
	}

	void GPURenderable::SetCanTickIfHidden(bool in_tick_hidden)
	{
		tick_hidden = in_tick_hidden;
	}

	bool GPURenderable::IsRenderPassEnabled(char const * renderpass_name) const
	{
		return renderpass_filter.IsNameEnabled(renderpass_name);
	}

	void GPURenderable::AddEnabledRenderPasses(char const * renderpass_names)
	{
		renderpass_filter.AddEnabledNames(renderpass_names);
	}

	void GPURenderable::AddDisabledRenderPasses(char const * renderpass_names)
	{
		renderpass_filter.AddDisabledNames(renderpass_names);
	}

	void GPURenderable::RemoveEnabledRenderPasses(char const * renderpass_names)
	{
		renderpass_filter.RemoveEnabledNames(renderpass_names);
	}

	void GPURenderable::RemoveDisabledRenderPasses(char const * renderpass_names)
	{
		renderpass_filter.RemoveDisabledNames(renderpass_names);
	}

}; // namespace chaos
