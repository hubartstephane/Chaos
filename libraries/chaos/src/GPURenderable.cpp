#include <chaos/GPURenderable.h>

#include <chaos/GPURenderer.h>
#include <chaos/GPURenderParams.h>

namespace chaos
{

	// ========================================================
	// GPURenderable implementation
	// ========================================================

	int GPURenderable::Display(GPURenderer * renderer, GPUProgramProviderBase const * uniform_provider, GPURenderParams const & render_params) const
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

	int GPURenderable::DoDisplay(GPURenderer * renderer, GPUProgramProviderBase const * uniform_provider, GPURenderParams const & render_params) const
	{
		return 0;
	}	

	bool GPURenderable::DoUpdateGPUResources(GPURenderer * renderer) const
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
