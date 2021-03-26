#include <chaos/Chaos.h>

namespace chaos
{
	/** constructor */
	ParticleSpawner::ParticleSpawner(BitmapAtlas::TextureArrayAtlas const* in_atlas, ParticleLayerBase* in_particle_layer, ObjectRequest bitmap_request, ObjectRequest folder_request) :
		atlas(in_atlas),
		particle_layer(in_particle_layer)
	{
		// in case of error, make the Spawner invalid
		if (!SetBitmapInfo(bitmap_request, folder_request))
			particle_layer = nullptr;
	}

	bool ParticleSpawner::SetBitmapInfo(ObjectRequest bitmap_request, ObjectRequest folder_request)
	{
		// early exit
		if (particle_layer == nullptr || atlas == nullptr)
			return false;
		// if the requested bitmap is nullptr, considere the call as successfull
		if (bitmap_request.IsNoneRequest())
		{
			bitmap_info = nullptr;
			return true;
		}
		// find the folder
		BitmapAtlas::FolderInfo const* bitmap_set = atlas->GetFolderInfo(folder_request, true);
		if (bitmap_set == nullptr)
			return false;
		// get the bitmap
		bitmap_info = bitmap_set->GetBitmapInfo(bitmap_request, true);
		if (bitmap_info == nullptr)
			return false;
		return true;
	}

	ParticleAllocationBase* ParticleSpawner::SpawnParticles(size_t count, bool new_allocation)
	{
		// early exit
		if (particle_layer == nullptr)
			return nullptr;

		return particle_layer->SpawnParticles(count, new_allocation, [this](ParticleAccessor<ParticleDefault> accessor)
		{
			if (bitmap_info != nullptr && accessor.IsValid())
			{
				// find the corresponding Bitmap
				ParticleTexcoords texcoords;

				if (bitmap_info->HasAnimation() && bitmap_info->GetAnimationImageCount() > 0)
					texcoords = bitmap_info->GetAnimationLayout(0, WrapMode::CLAMP).GetTexcoords();
				else
					texcoords = bitmap_info->GetTexcoords();

				// apply the texcoords to all particles
				for (ParticleDefault& particle : accessor)
 					particle.texcoords = texcoords;
			}
		});		
	}

}; // namespace chaos

