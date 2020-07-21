#pragma once

#include <chaos/StandardHeaders.h>

#include <chaos/Object.h>
#include <chaos/BitmapAtlas.h>
#include <chaos/ParticleLayer.h>
#include <chaos/ParticleAccessor.h>

namespace chaos
{

    class ParticleSpawner : public Object
    {
    public:

        /** simplest constructor */
        ParticleSpawner(ParticleLayerBase* in_particle_layer) :
            particle_layer(in_particle_layer)
        {
        }

        /** constructor with additionnal bitmap arguments */
        ParticleSpawner(ParticleLayerBase* in_particle_layer, ObjectRequest bitmap_request) :
            particle_layer(in_particle_layer)
        {
            // in case of error, make the Spawner invalid
            if (!SetBitmapInfo(bitmap_request))
                particle_layer = nullptr;
        }

        /** change the bitmap info */
		bool SetBitmapInfo(ObjectRequest bitmap_request, ObjectRequest folder_request = "sprites"); // folder "sprites" by default
   
        /** simple spawn method */
        ParticleAllocationBase* SpawnParticles(size_t count, bool new_allocation);

        /** spawn + user initialization methods */
        template<typename INIT_PARTICLE_FUNC>
        ParticleAllocationBase* SpawnParticles(size_t count, bool new_allocation, INIT_PARTICLE_FUNC init_func)
        {
            ParticleAllocationBase* result = SpawnParticles(count, new_allocation);
            // call user initialization function
            if (result != nullptr)
            {
                size_t allocation_count = result->GetParticleCount();
                init_func(result->GetParticleAccessor(allocation_count - count, count));  // partial accessor, take the last particles in the array
            }
            return result;
        }

        /** gets the bitmap info used for spawned particles */
        BitmapAtlas::BitmapInfo const * GetBitmapInfo() const { return bitmap_info; }

        /** returns whether the spawner is valid */
        bool IsValid() const { return (particle_layer != nullptr); }
        /** returns whether the spawner has a bitmap info */
        bool HasBitmap() const { return (bitmap_info != nullptr); }

    protected:

        /** the spawn internal method (that just creates particles) */
        ParticleAllocationBase* DoSpawnParticles(size_t count, bool new_allocation);

    protected:

        /** the layer instance where to spawn the particles */
        ParticleLayerBase* particle_layer = nullptr;
        /** the bitmap info to use for the spawned particles */
        BitmapAtlas::BitmapInfo const* bitmap_info = nullptr;
    };


}; // namespace chaos

