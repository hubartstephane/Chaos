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



	class TileCollisionComputer
	{
	public:

		/** constructor */
		TileCollisionComputer(TileCollisionComputer const& src) = default;
		/** constructor */
		TileCollisionComputer(TMLevelInstance* in_level_instance, chaos::box2 in_src_box, chaos::box2 in_dst_box, int in_collision_mask, chaos::ParticleAllocationBase* in_ignore_allocation, glm::vec2 const& in_box_extend, char const* in_wangset_name) :
			level_instance(in_level_instance),
			src_box(in_src_box),
			dst_box(in_dst_box),
			collision_mask(in_collision_mask),
			ignore_allocation(in_ignore_allocation),
			box_extend(in_box_extend),
			wangset_name(in_wangset_name),
			delta_position(dst_box.position - src_box.position) {}

	public:

		/** the entry point for the whole computation */
		chaos::box2 Run(std::function<void(TileCollisionInfo const& collision_info)> func);
	
		/** compute reaction for a a particle */
		void ComputeReaction(TileCollisionInfo const& collision_info, std::function<bool(TileCollisionInfo const&, chaos::Edge)> func);

	public:

		/** the level instance used */
		TMLevelInstance* level_instance = nullptr;
		/** the initial position of the moving object */
		chaos::box2 src_box;
		/** the final position of the moving object (if there were no collision) */
		chaos::box2 dst_box;
		/** the mask for all searched objects */
		int collision_mask = 0;
		/** allocation to be ignored */
		chaos::ParticleAllocationBase* ignore_allocation = nullptr;
		/** some extension for the collision surface*/
		glm::vec2 const& box_extend;
		/** the name of the collision wangset (use NEIGHBOUR FLAGS if unspecified) */
		char const* wangset_name = nullptr;

		/** the difference of position between src_box and dst_box before any reaction is computed */
		glm::vec2 delta_position = glm::vec2(0.0f, 0.0f);

	protected:

		/** cache for wangset */
		chaos::TiledMap::Wangset const* wangset = nullptr;
		/** cache for wangset */
		chaos::TiledMap::TileSet const* tileset = nullptr;
	};

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
