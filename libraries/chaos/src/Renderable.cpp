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
		// internal filters
		if (!IsRenderPassEnabled(render_params.submaterial_name.c_str()))
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

	int Renderable::DoDisplay(Renderer * renderer, GPUProgramProviderBase const * uniform_provider, RenderParams const & render_params) const
	{
		return 0;
	}	

	bool Renderable::DoUpdateGPUResources(Renderer * renderer) const
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
		// nullptr is equivalent to empty string
		if (renderpass_name == nullptr)
			renderpass_name = "";
		// is this pass forbidden ?
		for (std::string const & disabled : disabled_renderpasses)
			if (StringTools::Stricmp(renderpass_name, disabled))
				return false;
		// is this pass in the enabled list
		if (enabled_renderpasses.size() > 0)
		{
			for (std::string const & enabled : enabled_renderpasses)
				if (StringTools::Stricmp(renderpass_name, enabled))
					return true;
			return false;
		}
		return true;
	}
	
	void Renderable::AddEnabledRenderPass(char const * renderpass_name)
	{
		AddRenderPassImpl(renderpass_name, enabled_renderpasses);
	}

	void Renderable::AddDisabledRenderPass(char const * renderpass_name)
	{
		AddRenderPassImpl(renderpass_name, disabled_renderpasses);
	}

	void Renderable::RemoveEnabledRenderPass(char const * renderpass_name)
	{
		RemoveRenderPassImpl(renderpass_name, enabled_renderpasses);
	}

	void Renderable::RemoveDisabledRenderPass(char const * renderpass_name)
	{
		RemoveRenderPassImpl(renderpass_name, disabled_renderpasses);
	}

	void Renderable::AddRenderPassImpl(char const * renderpass_name, std::vector<std::string> & target_list)
	{
		// nullptr is equivalent to empty string
		if (renderpass_name == nullptr)
			renderpass_name = "";
		// search if the name is already existing
		for (std::string const & element : target_list)
			if (StringTools::Stricmp(renderpass_name, element))
				return;
		// insert the element
		target_list.push_back(renderpass_name);
	}

	void Renderable::RemoveRenderPassImpl(char const * renderpass_name, std::vector<std::string> & target_list)
	{
		// nullptr is equivalent to empty string
		if (renderpass_name == nullptr)
			renderpass_name = "";
		// remove the element
		size_t count = target_list.size();
		for (size_t i = 0; i < count; ++i)
		{
			if (StringTools::Stricmp(renderpass_name, target_list[i]))
			{
				target_list.erase(target_list.begin() + i);
				return;
			}
		}
	}


}; // namespace chaos
