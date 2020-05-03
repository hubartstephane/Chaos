#include <death/LayerInstanceParticlePopulator.h>
#include <death/TiledMapLevel.h>

#include <chaos/TiledMapTools.h>
#include <chaos/ParticleDefault.h>
#include <chaos/WrapMode.h>

namespace death
{
	// =====================================
	// LayerInstanceParticlePopulator implementation
	// =====================================

	bool TiledMapLayerInstanceParticlePopulator::Initialize(TiledMapLayerInstance* in_layer_instance)
	{
		assert(in_layer_instance != nullptr);
		layer_instance = in_layer_instance;

		death::TiledMapLevel* level = layer_instance->GetLevel();
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

	void TiledMapLayerInstanceParticlePopulator::FlushCachedParticlesToAllocation()
	{
		chaos::ParticleAccessor<TiledMapParticle> accessor = allocation->AddParticles(particle_count);
		for (size_t i = 0; i < particle_count; ++i)
			accessor[i] = particles[i];
	}

	void TiledMapLayerInstanceParticlePopulator::FlushParticles()
	{
		// nothing to flush
		if (particle_count == 0)
			return;
		// create an allocation if necessary
		if (allocation == nullptr)
		{
			allocation = layer_instance->SpawnParticles(0);
			if (allocation == nullptr)
				return;
		}
		// reserve memory and flush
		FlushCachedParticlesToAllocation();
		// empty the cache
		particle_count = 0;
	}

	bool TiledMapLayerInstanceParticlePopulator::AddParticle(char const* bitmap_name, chaos::box2 particle_box, glm::vec4 const& color, int gid, bool horizontal_flip, bool vertical_flip, bool diagonal_flip, bool keep_aspect_ratio)
	{
		assert(bitmap_name != nullptr);

		// search bitmap information for the particle
		chaos::BitmapAtlas::BitmapInfo const* bitmap_info = folder_info->GetBitmapInfo(bitmap_name);
		if (bitmap_info == nullptr)
			return false;
		// get the real layout of the bitmap by removing animation
		chaos::BitmapAtlas::BitmapLayout layout = *bitmap_info;
		if (bitmap_info->HasAnimation())
			layout = bitmap_info->GetAnimationLayout(0, chaos::WrapMode::clamp); // take frame 0 by default
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

		// rotate the box if diagonal flipping is on
		if (diagonal_flip)
		{
			glm::vec2 hotpoint = particle_box.position - particle_box.half_size; // the bottom left corner position has to be unmodified (as in Tiled Map Editor)
			std::swap(particle_box.half_size.x, particle_box.half_size.y);
			particle_box.position = hotpoint + particle_box.half_size;
		}

		// add the particle
		TiledMapParticle particle;
		particle.bounding_box = particle_box;
		particle.texcoords = layout.GetTexcoords();
		particle.color = color;
		particle.flags = 0;

		if (horizontal_flip)
			particle.flags |= chaos::ParticleDefaultFlags::TEXTURE_HORIZONTAL_FLIP;
		if (vertical_flip)
			particle.flags |= chaos::ParticleDefaultFlags::TEXTURE_VERTICAL_FLIP;
		if (diagonal_flip)
			particle.flags |= chaos::ParticleDefaultFlags::TEXTURE_DIAGONAL_FLIP;
		
		particle.gid = gid;


		// shuxxx

		particle.bitmap_info = bitmap_info;

		particles[particle_count++] = particle;

		// increment the bounding box
		bounding_box = bounding_box | particle_box;

		// flush previous particles to make room for the new one
		if (particle_count == PARTICLE_BUFFER_SIZE)
			FlushParticles();
		return true;
	}

}; // namespace death
