#include <chaos/Chaos.h>

namespace chaos
{

	bool GameParticleCreator::Initialize(chaos::ParticleManager * in_particle_manager, chaos::ParticleTextGenerator::Generator * in_particle_text_generator, chaos::BitmapAtlas::TextureArrayAtlas * in_texture_atlas)
	{
		// particle text generator can be null if we dont want to create text
		assert(in_particle_manager != nullptr);
		assert(in_texture_atlas != nullptr);

		particle_manager = in_particle_manager;
		particle_text_generator = in_particle_text_generator;
		texture_atlas = in_texture_atlas;
		return true;
	}

	chaos::BitmapAtlas::BitmapInfo const * GameParticleCreator::FindBitmapInfo(chaos::ObjectRequest bitmap_request) const
	{
		// find bitmap set
		chaos::BitmapAtlas::FolderInfo const * bitmap_set = texture_atlas->GetFolderInfo("sprites");
		if (bitmap_set == nullptr)
			return nullptr;
		// find bitmap info
		return bitmap_set->GetBitmapInfo(bitmap_request);
	}

	chaos::ParticleAllocationBase * GameParticleCreator::SpawnParticles(chaos::ObjectRequest layer_id, chaos::ObjectRequest bitmap_request, size_t count, bool new_allocation) const
	{
        chaos::ParticleSpawner spawner = particle_manager->GetParticleSpawner(layer_id, bitmap_request);
        if (!spawner.IsValid())
            return nullptr;
        if (!bitmap_request.IsNoneRequest() && !spawner.HasBitmap())
            return nullptr;
        return spawner.SpawnParticles(count, new_allocation);
	}

	chaos::ParticleAllocationBase * GameParticleCreator::SpawnTextParticles(chaos::ObjectRequest layer_id, char const * text, chaos::ParticleTextGenerator::GeneratorParams const & params) const
	{
		// find layer of concern
		chaos::ParticleLayerBase * layer = particle_manager->FindLayer(layer_id);
		if (layer == nullptr)
			return nullptr;

		// generate the tokens
		chaos::ParticleTextGenerator::GeneratorResult result;
		particle_text_generator->Generate(text, result, params);

		// generate the particles
		return chaos::ParticleTextGenerator::CreateTextAllocation(layer, result);
	}
	
}; // namespace chaos
