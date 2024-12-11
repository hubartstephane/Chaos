#include "chaos/ChaosPCH.h"
#include "chaos/ChaosInternals.h"

namespace chaos
{
	// =====================================
	// TMParticlePopulator implementation
	// =====================================

	TMParticlePopulator::TMParticlePopulator(TMParticlePopulator const& src):
		layer_instance(src.layer_instance),
		level(src.level),
		texture_atlas(src.texture_atlas),
		folder_info(src.folder_info)
	{
		// XXX : do not copy nor particles, nor allocation => force a new allocation
	}

	bool TMParticlePopulator::Initialize(TMLayerInstance* in_layer_instance)
	{
		assert(in_layer_instance != nullptr);
		assert(in_layer_instance->particle_layer != nullptr);
		layer_instance = in_layer_instance;

		// get the level
		level = layer_instance->GetLevel();
		if (level == nullptr)
			return false;
		// get the texture atlas
		texture_atlas = in_layer_instance->particle_layer->GetTextureAtlas();
		if (texture_atlas == nullptr)
			return false;
		// get the folder in which the bitmap information are stored
		folder_info = texture_atlas->GetFolderInfo(level->GetFolderInfoRequest());
		if (folder_info == nullptr)
			return false;
		return true;
	}

	bool TMParticlePopulator::FlushCachedParticlesToAllocation()
	{
		ParticleAccessor<TMParticle> accessor = allocation->AddParticles(particle_count);

		if (!accessor.IsValid())
		{
			ParticleLog::Error("TMParticlePopulator::FlushCachedParticlesToAllocation => invalid accessor");
			return false;
		}

		for (size_t i = 0; i < particle_count; ++i)
			accessor[i] = particles[i];

		return true;
	}

	bool TMParticlePopulator::FlushParticles()
	{
		// nothing to flush
		if (particle_count == 0)
			return true;
		// create an allocation if necessary
		if (allocation == nullptr)
		{
			allocation = layer_instance->SpawnParticles(0);
			if (allocation == nullptr)
			{
				ParticleLog::Error("TMParticlePopulator::FlushParticles : fails to SpawnParticles");
				particle_count = 0;
				return false;
			}
		}
		// reserve memory and flush
		bool result = FlushCachedParticlesToAllocation();
		particle_count = 0;
		return result;
	}

	bool TMParticlePopulator::AddParticle(char const* bitmap_name, Hotpoint hotpoint, box2 particle_box, glm::vec4 const& color, float rotation, int particle_flags, int gid, bool keep_aspect_ratio)
	{
		assert(bitmap_name != nullptr);

		// search bitmap information for the particle
		BitmapAtlas::BitmapInfo const* bitmap_info = folder_info->GetBitmapInfo(bitmap_name);
		if (bitmap_info == nullptr)
		{
			ParticleLog::Error("TMParticlePopulator::AddParticle : unknown bitmap [%s]", (bitmap_name != nullptr)? bitmap_name : "");
			return false;
		}
		// get the real layout of the bitmap by removing animation
		BitmapAtlas::BitmapLayout layout = *bitmap_info;
		if (bitmap_info->HasAnimation() && bitmap_info->GetAnimationImageCount() > 0)
			layout = bitmap_info->GetAnimationLayout(0, WrapMode::CLAMP); // take frame 0 by default
		// compute the bounding box
		if (IsGeometryEmpty(particle_box))
		{
			particle_box.half_size = glm::vec2(layout.width, layout.height) * 0.5f;
		}
		// maintain aspect ratio
		else
		{
			if (keep_aspect_ratio)
			{
				int layout_width  = layout.width;
				int layout_height = layout.height;
				if ((particle_flags & ParticleFlags::TEXTURE_DIAGONAL_FLIP) != 0)
					std::swap(layout_width, layout_height);

				particle_box = SetBoxAspect(particle_box, MathTools::CastAndDiv<float>(layout_width, layout_height), SetBoxAspectMethod::PREFER_UPDATE_WIDTH);
			}
		}

		// add the particle
		TMParticle particle;
		particle.bounding_box = particle_box;
		particle.texcoords = layout.GetTexcoords();
		particle.color = color;
		particle.flags = particle_flags;
		particle.gid = gid;
		particle.rotation = rotation;
		particle.bitmap_info = bitmap_info;

		// for tiled map objects, rotation use BOTTOM-LEFT as pivot whereas in our particle system, the  pivot is center
		if (rotation != 0.0f)
		{
			glm::vec2 pivot = ConvertHotpoint(particle_box.position, particle_box.half_size * 2.0f, Hotpoint::CENTER, hotpoint);

			float c = std::cos(rotation);
			float s = std::sin(rotation);

			particle.bounding_box.position = pivot + GLMTools::Rotate((particle.bounding_box.position - pivot), c, s);
		}

		particles[particle_count++] = particle;

		// increment the bounding box
		bounding_box = bounding_box | particle_box;

		// flush previous particles to make room for the new one
		if (particle_count == PARTICLE_BUFFER_SIZE)
			if (!FlushParticles())
				return false;
		return true;
	}

	// XXX : copy all data except for the cached particles waiting to be flushed
	TMParticlePopulator& TMParticlePopulator::operator = (TMParticlePopulator const& src)
	{
		layer_instance = src.layer_instance;
		level = src.level;
		texture_atlas = src.texture_atlas;
		folder_info = src.folder_info;
		return *this;
	}

}; // namespace chaos
