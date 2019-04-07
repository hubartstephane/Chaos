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

	ParticleLayerBase * ParticleManager::FindLayer(char const * name)
	{
		return dynamic_cast<ParticleLayerBase*>(layer_system.FindChildRenderable(name));
	}

	ParticleLayerBase const * ParticleManager::FindLayer(char const * name) const
	{
		return dynamic_cast<ParticleLayerBase const*>(layer_system.FindChildRenderable(name));
	}

	ParticleLayerBase * ParticleManager::FindLayer(TagType id)
	{
		return dynamic_cast<ParticleLayerBase*>(layer_system.FindChildRenderable(id));
	}

	ParticleLayerBase const * ParticleManager::FindLayer(TagType id) const
	{
		return dynamic_cast<ParticleLayerBase const*>(layer_system.FindChildRenderable(id));
	}

	void ParticleManager::DoAddLayer(ParticleLayerBase * layer, int render_order, TagType layer_id)
	{
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
			ParticleLayerBase * layer = dynamic_cast<ParticleLayerBase *>(layer_system.GetChildAt(i));
			if (layer == nullptr)
				continue;
			layer->Tick(delta_time);
		}
		return true;
	}

	int ParticleManager::DoDisplay(Renderer * renderer, GPUProgramProviderBase const * uniform_provider, RenderParams const & render_params) const
	{
		return layer_system.Display(renderer, uniform_provider, render_params);
	}

}; // namespace chaos

