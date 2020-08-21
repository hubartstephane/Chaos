#include <chaos/ParticleSpawner.h>

namespace chaos
{

	bool ParticleSpawner::SetBitmapInfo(ObjectRequest bitmap_request, ObjectRequest folder_request)
	{
		if (particle_layer == nullptr)
			return false;
		// get the atlas
		BitmapAtlas::TextureArrayAtlas const* atlas = particle_layer->GetTextureAtlas();
		if (atlas == nullptr)
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
		ParticleAllocationBase* result = DoSpawnParticles(count, new_allocation);
		if (result != nullptr)
		{
			if (bitmap_info != nullptr)
			{
				size_t allocation_count = result->GetParticleCount();

				// find the corresponding Bitmap
				ParticleTexcoords texcoords;

				if (bitmap_info->HasAnimation() && bitmap_info->GetAnimationImageCount() > 0)
					texcoords = bitmap_info->GetAnimationLayout(0, chaos::WrapMode::CLAMP).GetTexcoords();
				else
					texcoords = bitmap_info->GetTexcoords();

				// apply the texcoords to all particles
				ParticleAccessor<ParticleDefault> accessor = result->GetParticleAccessor(allocation_count - count, count); // partial accessor, take the last particles in the array
				for (ParticleDefault& particle : accessor)
					particle.texcoords = texcoords;
			}
		}
		return result;
	}

	ParticleAllocationBase* ParticleSpawner::DoSpawnParticles(size_t count, bool new_allocation)
	{
		// early exit
		if (particle_layer == nullptr)
			return nullptr;

		ParticleAllocationBase* result = nullptr;

		// creation of a new allocation
		if (new_allocation || particle_layer->GetAllocationCount() == 0)
		{
			result = particle_layer->SpawnParticles(count);
		}
		// take the very first existing allocation and add particles at the end
		else
		{
	 		result = particle_layer->GetAllocation(0);
			if (result == nullptr || !result->AddParticles(count))
				return nullptr;
		}
		return result;
	}

}; // namespace chaos

