#include <chaos/ParticleManager.h>
#include <chaos/GPUDrawPrimitive.h>
#include <chaos/GPUResourceManager.h>
#include <chaos/GPUBufferCache.h>
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

	ParticleLayerBase * ParticleManager::FindLayer(char const * name)
	{
		return auto_cast(layer_system.FindChildRenderable(name));
	}

	ParticleLayerBase const * ParticleManager::FindLayer(char const * name) const
	{
		return auto_cast(layer_system.FindChildRenderable(name));
	}

	ParticleLayerBase * ParticleManager::FindLayer(TagType id)
	{
		return auto_cast(layer_system.FindChildRenderable(id));
	}

	ParticleLayerBase const * ParticleManager::FindLayer(TagType id) const
	{
		return auto_cast(layer_system.FindChildRenderable(id));
	}

	void ParticleManager::DoAddLayer(ParticleLayerBase * layer, int render_order, TagType layer_id)
	{        		
        layer->particle_manager = this;
        layer->SetTag(layer_id);
        layer->SetTextureAtlas(atlas.get());
		layer_system.AddChildRenderable(layer, render_order);
	}

	void ParticleManager::RemoveLayer(ParticleLayerBase * layer)
	{
		layer_system.RemoveChildRenderable(layer);
	}

	bool ParticleManager::DoTick(double delta_time)
	{
		size_t count = layer_system.GetChildCount();
		for (size_t i = 0; i < count; ++i)
		{
			ParticleLayerBase * layer = auto_cast(layer_system.GetChildAt(i));
			if (layer == nullptr)
				continue;
			layer->Tick(delta_time);
		}
		return true;
	}

	int ParticleManager::DoDisplay(GPURenderer * renderer, GPUProgramProviderBase const * uniform_provider, GPURenderParams const & render_params)
	{
		return layer_system.Display(renderer, uniform_provider, render_params);
	}

}; // namespace chaos

