#include <chaos/Chaos.h>

namespace chaos
{

	// ==============================================================
	// PARTICLE MANAGER
	// ==============================================================

    ParticleManager::ParticleManager()
    {
        vertex_array_cache = new GPUVertexArrayCache();
    }

	void ParticleManager::SetTextureAtlas(BitmapAtlas::TextureArrayAtlas * in_atlas)
	{
		atlas = in_atlas;
	}

	ParticleLayerBase * ParticleManager::FindLayer(ObjectRequest request)
	{
		return auto_cast(layer_system.FindChildRenderable(request));
	}

	ParticleLayerBase const * ParticleManager::FindLayer(ObjectRequest request) const
	{
		return auto_cast(layer_system.FindChildRenderable(request));
	}

	void ParticleManager::DoAddLayer(ParticleLayerBase * layer, int render_order, ObjectRequest layer_id)
	{        		
        layer->particle_manager = this;
		layer->SetObjectNaming(layer_id);
        layer->SetTextureAtlas(atlas.get());
		layer_system.AddChildRenderable(layer, render_order);
	}

	void ParticleManager::RemoveLayer(ParticleLayerBase * layer)
	{
		layer_system.RemoveChildRenderable(layer);
	}

	bool ParticleManager::DoTick(float delta_time)
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

