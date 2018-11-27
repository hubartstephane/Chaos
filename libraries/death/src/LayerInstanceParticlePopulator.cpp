#include <death/LayerInstanceParticlePopulator.h>
#include <death/TiledMapLevel.h>

#include <chaos/TiledMapTools.h>
#include <chaos/ParticleDefault.h>

namespace death
{
	namespace TiledMap
	{
		// =====================================
		// LayerInstanceParticlePopulator implementation
		// =====================================

		bool LayerInstanceParticlePopulator::Initialize(LayerInstance * in_layer_instance)
		{
			assert(in_layer_instance != nullptr);
			layer_instance = in_layer_instance;

			death::TiledMap::Level * level = layer_instance->GetTypedLevel();
			assert(level != nullptr);

			// get the texture atlas
			texture_atlas = level->GetTextureAtlas(layer_instance);
			if (texture_atlas == nullptr)
				return false;

			// get the folder in which the bitmap information are stored
			folder_info = level->GetFolderInfo(layer_instance);
			if (folder_info == nullptr)
				return false;

			return true;
		}

		void LayerInstanceParticlePopulator::FlushParticles()
		{
			// nothing to flush
			if (particle_count == 0)
				return;
			// create an allocation if necessary
			if (allocation == nullptr)
			{
				allocation = layer_instance->CreateParticleAllocation();
				if (allocation == nullptr)
					return;
			}
			size_t old_count = allocation->GetParticleCount();

			// reserve memory
			allocation->AddParticles(particle_count);
			// an accessor to flush
			chaos::ParticleAccessor<TileParticle> p = allocation->GetParticleAccessor<TileParticle>();
			for (size_t i = 0; i < particle_count; ++i)
				p[old_count + i] = particles[i];

			// empty the cache
			particle_count = 0;
		}

		bool LayerInstanceParticlePopulator::AddParticle(char const * bitmap_name, chaos::box2 particle_box, glm::vec4 const & color, int gid, bool horizontal_flip, bool vertical_flip)
		{
			// search bitmap information for the particle
			chaos::BitmapAtlas::BitmapInfo const * bitmap_info = folder_info->GetBitmapInfo(bitmap_name);
			if (bitmap_info == nullptr)
				return false;

			// compute the bounding box
			if (particle_box.IsEmpty())
			{
				particle_box.half_size = glm::vec2(bitmap_info->width, bitmap_info->height) * 0.5f;
			}
			// maintain aspect ratio
			else
			{
				particle_box = chaos::AlterBoxToAspect(particle_box, chaos::MathTools::CastAndDiv<float>(bitmap_info->width, bitmap_info->height), true);
			}

			// add the particle
			TileParticle particle;
			particle.bounding_box = particle_box;
			particle.texcoords = chaos::ParticleTools::GetParticleTexcoords(*bitmap_info, texture_atlas->GetAtlasDimension());
			particle.texcoords = chaos::ParticleTools::ApplySymetriesToTexcoords(particle.texcoords, horizontal_flip, vertical_flip);
			particle.color = color;
			particle.gid = gid;

			particles[particle_count++] = particle;

			// increment the bounding box
			bounding_box = bounding_box | particle_box;

			// flush previous particles to make room for the new one
			if (particle_count == PARTICLE_BUFFER_SIZE)
				FlushParticles();
			return true;
		}

	}; // namespace TiledMap

}; // namespace death
