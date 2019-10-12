#include <death/GameParticleCreator.h>

#include <chaos/ParticleDefault.h>

namespace death
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

	chaos::ParticleAllocationBase * GameParticleCreator::SpawnParticles(chaos::TagType layer_id, size_t count) const
	{
		// spawn the particles
		chaos::ParticleLayerBase * layer = particle_manager->FindLayer(layer_id);
		if (layer == nullptr)
			return nullptr;
		return layer->SpawnParticles(count);
	}

	chaos::BitmapAtlas::BitmapInfo const * GameParticleCreator::FindBitmapInfo(char const * bitmap_name) const
	{
		// find bitmap set
		chaos::BitmapAtlas::FolderInfo const * bitmap_set = texture_atlas->GetFolderInfo("sprites");
		if (bitmap_set == nullptr)
			return nullptr;
		// find bitmap info
		return bitmap_set->GetBitmapInfo(bitmap_name);
	}

	chaos::ParticleAllocationBase * GameParticleCreator::CreateParticles(char const * bitmap_name, size_t count, chaos::TagType layer_id) const
	{
		// allocate the objects
		chaos::ParticleAllocationBase * allocation = SpawnParticles(layer_id, count);
		if (allocation == nullptr)
			return nullptr;

		if (!InitializeParticles(allocation, bitmap_name, count))
		{
			allocation->RemoveFromLayer();
			//delete(allocation); // shuxxx !!!
			return nullptr;
		}
		return allocation;
	}

	bool GameParticleCreator::InitializeParticles(chaos::ParticleAllocationBase * allocation, char const * bitmap_name, size_t last_count) const
	{
		// find bitmap info
		chaos::BitmapAtlas::BitmapInfo const * info = FindBitmapInfo(bitmap_name);
		if (info == nullptr)
			return false;

		chaos::ParticleAccessor<chaos::ParticleDefault::Particle> particles = allocation->GetParticleAccessor<chaos::ParticleDefault::Particle>();

		size_t particles_count = particles.GetCount();
		size_t start = 0;
		size_t count = last_count;

		if (particles_count < count)
			count = particles_count; 
		else
			start = particles_count - count;

		for (size_t i = 0 ; i < count ; ++i)
		{
			chaos::ParticleDefault::Particle & particle = particles[start + i];
			particle.texcoords = chaos::ParticleTools::GetParticleTexcoords(*info);
		}

		return true;
	}

	chaos::ParticleAllocationBase * GameParticleCreator::CreateTextParticles(char const * text, chaos::ParticleTextGenerator::GeneratorParams const & params, chaos::TagType layer_id) const
	{
		// find layer of concern
		chaos::ParticleLayerBase * layer = particle_manager->FindLayer(layer_id);
		if (layer == nullptr)
			return nullptr;

		// generate the tokens
		chaos::ParticleTextGenerator::GeneratorResult result;

		particle_text_generator->Generate(text, result, params);

		chaos::ParticleAllocationBase * allocation = chaos::ParticleTextGenerator::CreateTextAllocation(layer, result);

		return allocation;
	}
	
}; // namespace death
