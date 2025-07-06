#include "chaos/ChaosPCH.h"
#include "chaos/ChaosInternals.h"

namespace chaos
{

	// ========================================================
	// GPURenderable implementation
	// ========================================================

	int GPURenderable::Display(GPURenderContext * render_context, GPUProgramProviderInterface const * uniform_provider, GPURenderParams const & render_params)
	{
		assert(render_context != nullptr);
		if (!ShouldDisplayObject(render_context, uniform_provider, render_params))
			return 0;
		if (!UpdateGPUResources(render_context, uniform_provider, render_params))
			return 0;
		return DoDisplay(render_context, uniform_provider, render_params);
	}

	bool GPURenderable::ShouldDisplayObject(GPURenderContext * render_context, GPUProgramProviderInterface const * uniform_provider, GPURenderParams const & render_params) const
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
		return true;
	}

	bool GPURenderable::UpdateGPUResources(GPURenderContext* render_context, GPUProgramProviderInterface const* uniform_provider, GPURenderParams const& render_params)
	{
		// update once per frame at most
		uint64_t renderer_timestamp = render_context->GetGPUDevice()->GetTimestamp();
		if (renderer_timestamp == 0 || update_timestamp != renderer_timestamp) // test for 0 to ensure resource is updated even if caller does not care about updating a timestamp
		{
			if (!DoUpdateGPUResources(render_context))
				return false;
			update_timestamp = renderer_timestamp;
		}
		return true;
	}

	int GPURenderable::DoDisplay(GPURenderContext * render_context, GPUProgramProviderInterface const * uniform_provider, GPURenderParams const & render_params)
	{
		return 0;
	}

	bool GPURenderable::DoUpdateGPUResources(GPURenderContext * render_context)
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
