#pragma once

#include <death/TM.h>

#include <chaos/GeometryFramework.h>
#include <chaos/ParticleLayer.h>
#include <chaos/Edge.h>

namespace death
{
	// =====================================
	// TileCollisionInfo
	// =====================================

	class TileCollisionInfo
	{
	public:

		/** the layer instance concerned by this collision */
		TMLayerInstance* layer_instance = nullptr;
		/** the allocation concerned by this collision */
		chaos::ParticleAllocationBase* allocation = nullptr;
		/** the particle which with the collision is detected */
		TMParticle* particle = nullptr;
		/** some information about the tile information */
		chaos::TiledMap::TileInfo tile_info;
	};

	// =====================================
	// Collision function
	// =====================================

	chaos::box2 ComputeTileCollisionAndReaction(TMLevelInstance* level_instance, chaos::box2 src_box, chaos::box2 dst_box, int collision_mask, chaos::ParticleAllocationBase* ignore_allocation, glm::vec2 const& box_extend, char const* wangset_name, std::function<void(TMParticle&, chaos::Edge)> func);

	// =====================================
	// TMCollisionIteratorBase
	// =====================================

	class TMCollisionIteratorBase
	{

	public:

		/** the default constructor */
		TMCollisionIteratorBase() = default;
		/** the copy constructor */
		TMCollisionIteratorBase(TMCollisionIteratorBase const& src) = default;
		/** the constructor with initialization */
		TMCollisionIteratorBase(TMLevelInstance* in_level_instance, chaos::box2 const& in_collision_box, uint64_t in_collision_mask, bool in_open_geometry);

		/** returns whether the iterator is still valid */
		operator bool() const;

	protected:

		/** the collision box */
		chaos::box2 collision_box;
		/** the collision mask for iterating over layers */
		uint64_t collision_mask = 0;
		/** whether we want to check open geometries */
		bool open_geometry = false;


		/** the level instance of concern */
		TMLevelInstance* level_instance = nullptr;
		/** index of the layer */
		size_t layer_instance_index = 0;
		/** whether the iterator finishs with the current layer */
		bool ignore_other_layers = false;
	};

	// =====================================
	// TMTileCollisionIterator
	// =====================================

	class TMTileCollisionIterator : public TMCollisionIteratorBase
	{
	public:

		/** the default constructor */
		TMTileCollisionIterator() = default;
		/** the copy constructor */
		TMTileCollisionIterator(TMTileCollisionIterator const& src) = default;
		/** the constructor with initialization */
		TMTileCollisionIterator(TMLevelInstance* in_level_instance, chaos::box2 const& in_collision_box, uint64_t in_collision_mask, bool in_open_geometry);

		/** next layer */
		void NextLayer();
		/** next allocation */
		void NextAllocation();
		/** next particle (i.e operator ++) */
		void Next();

		// indirection
		TileCollisionInfo const& operator *() const;
		// indirection
		TileCollisionInfo const* operator ->() const;
		// pre increment iterator
		TMTileCollisionIterator& operator ++ ();
		// post increment iterator
		TMTileCollisionIterator operator ++ (int i);

	protected:

		/** find the very first collision from given conditions */
		void FindFirstCollision();
		/** called to set the iterator at its end */
		void EndIterator();

	protected:

		/** allocation index in that layer */
		size_t allocation_index = 0;
		/** index of the particle in that layer */
		size_t particle_index = 0;

		/** the collision data */
		TileCollisionInfo cached_result;
	};

	// =====================================
	// TMObjectCollisionIteratorBase
	// =====================================

	template<typename T>
	class TMObjectCollisionIteratorBase : public TMCollisionIteratorBase
	{
	public:

		/** the default constructor */
		TMObjectCollisionIteratorBase() = default;
		/** the copy constructor */
		TMObjectCollisionIteratorBase(TMObjectCollisionIteratorBase const& src) = default;
		/** the constructor with initialization */
		TMObjectCollisionIteratorBase(TMLevelInstance* in_level_instance, chaos::box2 const& in_collision_box, uint64_t in_collision_mask, bool in_open_geometry) :
			TMCollisionIteratorBase(in_level_instance, in_collision_box, in_collision_mask, in_open_geometry)
		{
		}
		// indirection
		T& operator *() const
		{
			assert(level_instance != nullptr); // end already reached. cannot indirect
			return *cached_result;
		}
		// indirection
		T* operator ->() const
		{
			assert(level_instance != nullptr); // end already reached. cannot indirect
			return cached_result;
		}

	protected:

		/** called to set the iterator at its end */
		void EndIterator()
		{
			level_instance = nullptr;
			layer_instance_index = 0;
			object_index = 0;
		}

	protected:

		/** index of the layer */
		size_t layer_instance_index = 0;
		/** trigger index in that layer */
		size_t object_index = 0;

		/** the current result of the research */
		T* cached_result = nullptr;
	};


	// =====================================
	// TMTriggerCollisionIterator
	// =====================================

	class TMTriggerCollisionIterator : public TMObjectCollisionIteratorBase<TMTrigger>
	{
	public:

		/** the default constructor */
		TMTriggerCollisionIterator() = default;
		/** the copy constructor */
		TMTriggerCollisionIterator(TMTriggerCollisionIterator const& src) = default;
		/** the constructor with initialization */
		TMTriggerCollisionIterator(TMLevelInstance* in_level_instance, chaos::box2 const& in_collision_box, uint64_t in_collision_mask, bool in_open_geometry);

		// pre increment iterator
		TMTriggerCollisionIterator& operator ++ ();
		// post increment iterator
		TMTriggerCollisionIterator operator ++ (int i);

		/** go to next layer */
		void NextLayer();
		/** go to next element */
		void Next();

	protected:

		/** find the very first collision from given conditions */
		void FindFirstCollision();
	};

	// =====================================
	// TMObjectCollisionIterator
	// =====================================

	class TMObjectCollisionIterator : public TMObjectCollisionIteratorBase<TMObject>
	{
	public:

		/** the default constructor */
		TMObjectCollisionIterator() = default;
		/** the copy constructor */
		TMObjectCollisionIterator(TMObjectCollisionIterator const& src) = default;
		/** the constructor with initialization */
		TMObjectCollisionIterator(TMLevelInstance* in_level_instance, chaos::box2 const& in_collision_box, uint64_t in_collision_mask, bool in_open_geometry);

		// pre increment iterator
		TMObjectCollisionIterator& operator ++ ();
		// post increment iterator
		TMObjectCollisionIterator operator ++ (int i);

		/** go to next layer */
		void NextLayer();
		/** go to next element */
		void Next();

	protected:

		/** find the very first collision from given conditions */
		void FindFirstCollision();
	};


}; // namespace death
