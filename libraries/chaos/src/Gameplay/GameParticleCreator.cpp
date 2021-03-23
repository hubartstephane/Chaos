#include <chaos/Chaos.h>

namespace chaos
{

	bool GameParticleCreator::Initialize(ParticleManager * in_particle_manager, ParticleTextGenerator::Generator * in_particle_text_generator, BitmapAtlas::TextureArrayAtlas * in_texture_atlas)
	{
		// particle text generator can be null if we dont want to create text
		assert(in_particle_manager != nullptr);
		assert(in_texture_atlas != nullptr);

		particle_manager = in_particle_manager;
		particle_text_generator = in_particle_text_generator;
		texture_atlas = in_texture_atlas;
		return true;
	}

	BitmapAtlas::BitmapInfo const * GameParticleCreator::FindBitmapInfo(ObjectRequest bitmap_request, ObjectRequest folder_request) const
	{
		// find bitmap set
		BitmapAtlas::FolderInfo const * bitmap_set = texture_atlas->GetFolderInfo(folder_request);
		if (bitmap_set == nullptr)
			return nullptr;
		// find bitmap info
		return bitmap_set->GetBitmapInfo(bitmap_request);
	}

	ParticleAllocationBase * GameParticleCreator::SpawnParticles(ObjectRequest layer_id, ObjectRequest bitmap_request, size_t count, bool new_allocation) const
	{
        ParticleSpawner spawner = particle_manager->GetParticleSpawner(layer_id, bitmap_request);
        if (!spawner.IsValid())
            return nullptr;
        if (!bitmap_request.IsNoneRequest() && !spawner.HasBitmap())
            return nullptr;
        return spawner.SpawnParticles(count, new_allocation);
	}

	ParticleAllocationBase * GameParticleCreator::SpawnTextParticles(ObjectRequest layer_id, char const * text, ParticleTextGenerator::GeneratorParams const & params) const
	{
		// find layer of concern
		ParticleLayerBase * layer = particle_manager->FindLayer(layer_id);
		if (layer == nullptr)
			return nullptr;

		// generate the tokens
		ParticleTextGenerator::GeneratorResult result;
		particle_text_generator->Generate(text, result, params);

		// generate the particles
		return ParticleTextGenerator::CreateTextAllocation(layer, result);
	}
	
}; // namespace chaos
