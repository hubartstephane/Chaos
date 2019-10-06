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

			death::TiledMap::Level * level = layer_instance->GetTiledLevel();
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
				allocation = layer_instance->CreateParticleAllocation(0);
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

		bool LayerInstanceParticlePopulator::AddParticle(char const * bitmap_name, chaos::box2 particle_box, glm::vec4 const & color, int gid, bool horizontal_flip, bool vertical_flip, bool keep_aspect_ratio)
		{
			// search bitmap information for the particle
			chaos::BitmapAtlas::BitmapInfo const * bitmap_info = folder_info->GetBitmapInfo(bitmap_name);
			if (bitmap_info == nullptr)
				return false;

			// shuludum

			// get the real layout of the bitmap by removing animation
			chaos::BitmapAtlas::BitmapLayout layout = *bitmap_info;
			if (bitmap_info->HasAnimation())
				layout = bitmap_info->GetAnimationLayout(0, chaos::BitmapAtlas::GetBitmapLayoutFlag::clamp); // take frame 0 by default

			// compute the bounding box
			if (IsGeometryEmpty(particle_box))
			{
				particle_box.half_size = glm::vec2(layout.width, layout.height) * 0.5f;
			}
			// maintain aspect ratio
			else
			{
				if (keep_aspect_ratio)
					particle_box = chaos::AlterBoxToAspect(particle_box, chaos::MathTools::CastAndDiv<float>(layout.width, layout.height), true);
			}

			// add the particle
			TileParticle particle;
			particle.bounding_box = particle_box;
			particle.texcoords = chaos::ParticleTools::GetParticleTexcoords(layout, texture_atlas->GetAtlasDimension());
			particle.texcoords = chaos::ParticleTools::ApplySymetriesToTexcoords(particle.texcoords, horizontal_flip, vertical_flip);
			particle.color = color;
			particle.gid = gid;


			particle.bitmap_info = bitmap_info;

			

			// shuludum





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
