#include <chaos/ParticleManager.h>
#include <chaos/DrawPrimitive.h>
#include <chaos/GPUResourceManager.h>
#include <chaos/ParticleTools.h>

namespace chaos
{

	// ==============================================================
	// PARTICLE MANAGER
	// ==============================================================

	void ParticleManager::SetTextureAtlas(BitmapAtlas::TextureArrayAtlas * in_atlas)
	{
		atlas = in_atlas;
	}

	ParticleLayer * ParticleManager::FindLayer(char const * name)
	{
		return dynamic_cast<ParticleLayer*>(layer_system.FindChildRenderable(name));
	}

	ParticleLayer const * ParticleManager::FindLayer(char const * name) const
	{
		return dynamic_cast<ParticleLayer const*>(layer_system.FindChildRenderable(name));
	}

	ParticleLayer * ParticleManager::FindLayer(TagType id)
	{
		return dynamic_cast<ParticleLayer*>(layer_system.FindChildRenderable(id));
	}

	ParticleLayer const * ParticleManager::FindLayer(TagType id) const
	{
		return dynamic_cast<ParticleLayer const*>(layer_system.FindChildRenderable(id));
	}

	ParticleLayer * ParticleManager::AddLayer(ParticleLayerDesc * layer_desc, int render_order, TagType layer_id)
	{
		ParticleLayer * result = new ParticleLayer(layer_desc);
		if (result == nullptr)
			return result;
		DoAddLayer(result, render_order, layer_id);
		return result;
	}

	ParticleLayer * ParticleManager::AddLayer(ParticleLayerDesc * layer_desc, int render_order, TagType layer_id, char const * material_name)
	{
		ParticleLayer * result = ParticleLayer::CreateParticleLayer(layer_desc, material_name);
		if (result == nullptr)
			return result;
		DoAddLayer(result, render_order, layer_id);
		return result;
	}


	ParticleLayer * ParticleManager::AddLayer(ParticleLayerDesc * layer_desc, int render_order, TagType layer_id, GPURenderMaterial * render_material)
	{
		ParticleLayer * result = ParticleLayer::CreateParticleLayer(layer_desc, render_material);
		if (result == nullptr)
			return result;
		DoAddLayer(result, render_order, layer_id);
		return result;
	}

	void ParticleManager::DoAddLayer(ParticleLayer * layer, int render_order, TagType layer_id)
	{
		layer->SetTag(layer_id);
		layer->SetTextureAtlas(atlas.get());
		layer_system.AddChildRenderable(layer, render_order);
	}

	void ParticleManager::RemoveLayer(ParticleLayer * layer)
	{
		layer_system.RemoveChildRenderable(layer);
	}

	bool ParticleManager::DoTick(double delta_time)
	{
		size_t count = layer_system.GetChildCount();
		for (size_t i = 0; i < count; ++i)
		{
			ParticleLayer * layer = dynamic_cast<ParticleLayer *>(layer_system.GetChildAt(i));
			if (layer == nullptr)
				continue;
			layer->Tick(delta_time);
		}
		return true;
	}

	int ParticleManager::DoDisplay(GPUProgramProviderBase const * uniform_provider, RenderParams const & render_params) const
	{
		return layer_system.Display(uniform_provider, render_params);
	}

}; // namespace chaos

