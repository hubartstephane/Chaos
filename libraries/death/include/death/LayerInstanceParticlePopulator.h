#pragma once

#include <chaos/StandardHeaders.h>
#include <chaos/GeometryFramework.h>
#include <chaos/Object.h>
#include <chaos/TiledMap.h>
#include <chaos/GPURenderable.h>
#include <chaos/Tickable.h>
#include <chaos/ParticleManager.h>
#include <chaos/ParticleDefault.h>
#include <chaos/BitmapAtlas.h>
#include <chaos/Hotpoint.h>

#include <death/TiledMapLevel.h>
#include <death/TiledMapParticle.h>

namespace death
{

	// =====================================
	// TiledMapLayerInstanceParticlePopulator : utility class to generate particles for a layer with a cache
	// =====================================

	class TiledMapLayerInstanceParticlePopulator : public chaos::Object
	{
		static size_t const PARTICLE_BUFFER_SIZE = 100;

	public:

		/** initialize the object */
		bool Initialize(TiledMapLayerInstance* in_layer_instance);
		/** insert a new particle */
		bool AddParticle(char const* bitmap_name, chaos::Hotpoint hotpoint, chaos::box2 particle_box, glm::vec4 const& color, float rotation, int particle_flags, int gid, bool keep_aspect_ratio);
		/** flush remaining particles */
		void FlushParticles();

		/** get the final bounding box */
		chaos::box2 const& GetBoundingBox() const { return bounding_box; }
		/** get the particle allocation */
		chaos::ParticleAllocationBase* GetParticleAllocation() { return allocation; }

	protected:

		/** 'copy' the cached particle into the allocation (with type conversion) */
		virtual void FlushCachedParticlesToAllocation();

	protected:

		/** the concerned layer instance */
		TiledMapLayerInstance* layer_instance = nullptr;
		/** the texture atlas required */
		chaos::BitmapAtlas::TextureArrayAtlas const* texture_atlas = nullptr;
		/** the folder containing the bitmaps */
		chaos::BitmapAtlas::FolderInfo const* folder_info = nullptr;

		/** the allocation for all those particles */
		chaos::ParticleAllocationBase* allocation = nullptr;

		/** a cache of particles */
		TiledMapParticle particles[PARTICLE_BUFFER_SIZE];
		/** the cached number of particles */
		size_t particle_count = 0;
		/** a bounding box */
		chaos::box2 bounding_box;
	};

}; // namespace death
