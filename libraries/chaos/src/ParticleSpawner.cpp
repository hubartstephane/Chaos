#include <chaos/ParticleSpawner.h>

namespace chaos
{
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
					texcoords = bitmap_info->GetAnimationLayout(0, chaos::WrapMode::clamp).GetTexcoords();
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
		if (!new_allocation)
		{
			// create a first allocation, while there are none
			if (particle_layer->GetAllocationCount() == 0)
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
		}
		else
		{
			// independant allocation
			result = particle_layer->SpawnParticles(count);
		}
		return result;
	}

}; // namespace chaos

