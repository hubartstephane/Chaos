#pragma once

#include <chaos/StandardHeaders.h>
#include <chaos/ReferencedObject.h>
#include <chaos/RenderMaterial.h>

// -----------------------------------------------------------------

// all classes in this file
#define CHAOS_PARTICLE_CLASSES (ParticleRange) (ParticleRangeAllocation) (ParticleLayerBase) (ParticleManager)

// forward declaration
#define CHAOS_PARTICLE_FORWARD_DECL(r, data, elem) class elem;
BOOST_PP_SEQ_FOR_EACH(CHAOS_PARTICLE_FORWARD_DECL, _, CHAOS_PARTICLE_CLASSES)

// friendship macro
#define CHAOS_PARTICLE_FRIEND_DECL(r, data, elem) friend class elem;
#define CHAOS_PARTICLE_ALL_FRIENDS BOOST_PP_SEQ_FOR_EACH(CHAOS_PARTICLE_FRIEND_DECL, _, CHAOS_PARTICLE_CLASSES)

class ParticleRange
{
	CHAOS_PARTICLE_ALL_FRIENDS

public:

	/** the beginning of the range */
	size_t start = 0;
	/** the number of element in the range */
	size_t count = 0;
};

class ParticleRangeAllocation : public chaos::ReferencedObject
{
	CHAOS_PARTICLE_ALL_FRIENDS

public:

	/** destructor */
	virtual ~ParticleRangeAllocation();
	/** remove the allocation from its layer */
	void RemoveFromLayer();

protected:

	/** the particle layer that contains the range */
	ParticleLayerBase * layer = nullptr;
	/** the index of the range in its array */
	size_t range_index = std::numeric_limits<size_t>::max();
};

class ParticleLayerBase
{
	CHAOS_PARTICLE_ALL_FRIENDS

public:

	static size_t const DESTROY_PARTICLE_MARK = std::numeric_limits<size_t>::max();

	/** ticking the particle system */
	virtual void TickParticles(float delta_time);
	/** spawn a given number of particles */
	ParticleRange SpawnParticles(size_t count, boost::intrusive_ptr<ParticleRangeAllocation> * allocation = nullptr);
	/** mark any particle as to be destroyed next tick */
	void MarkParticlesToDestroy(size_t start, size_t count);

protected:

	/** internal method to remove a range from the layer */
	void RemoveParticleAllocation(ParticleRangeAllocation * allocation);
	/** internal particle allocation method */
	virtual ParticleRange DoSpawnParticles(size_t count);

protected:

	/** the order of the layer in the manager */
	int order = 0;
	/** the material used to render the layer */
	boost::intrusive_ptr<chaos::RenderMaterial> render_material;
	/** particles ranges */
	std::vector<ParticleRange> particles_ranges;
	/** ranges reservations */
	std::vector<boost::intrusive_ptr<ParticleRangeAllocation>> range_allocations;
	/** a utility vector that is used to mark particles to destroy, then as an internal utility vector */
	std::vector<size_t> suppression_vector;
};

// -----------------------------------------------------------------

template<typename PARTICLE_DESC_TYPE>
class ParticleLayer : public ParticleLayerBase
{
	CHAOS_PARTICLE_ALL_FRIENDS

public:

	using particle_desc_type = PARTICLE_DESC_TYPE;
	using particle_type = typename PARTICLE_DESC_TYPE::particle_type;
	using vertex_type = typename PARTICLE_DESC_TYPE::vertex_type;

	/** constructor */
	ParticleLayer(particle_desc_type in_desc = particle_desc_type()) :
		particle_desc(in_desc) 
	{
	}

	/** the ticking method */
	virtual void TickParticles(float delta_time) override
	{
		// update the particles themselves
		DoUpdateParticles(delta_time);
		// destroy the particles that are to be destroyed
		DoDestroyParticles(delta_time);
	}

protected:

	virtual ParticleRange DoSpawnParticles(size_t count) override
	{
		size_t current_particle_count = particles.size();

		// initialize the result
		ParticleRange result;
		result.start = current_particle_count;
		result.count = count;
		// create the particles and the suppression corresponding data
		size_t new_count = current_particle_count + count;
		particles.resize(new_count, particle_type());
		suppression_vector.resize(new_count, 0);
	
		return result;
	}

	void DoUpdateParticles(float delta_time)
	{
		// early exit
		size_t count = particles.size();
		if (count == 0)
			return;
		// update all particles
		for (size_t i = 0; i < count; ++i)
			particle_desc.UpdateParticle(delta_time, &particles[i]);
	}

	void DoDestroyParticles(float delta_time)
	{
		// early exit
		size_t count = particles.size();
		if (count == 0)
			return;
		// remove the particles and update the suppression vector
		size_t i = 0;
		size_t j = 0;
		while (i < count)
		{
			if (suppression_vector[i] != DESTROY_PARTICLE_MARK && !particle_desc.MustDestroyParticle(&particles[i])) // particle is OK
			{
				if (i != j)
					particles[j] = particles[i]; // keep the particle by copying it 
				suppression_vector[i] = (i - j);
				++j;
			}
			else
				suppression_vector[i] = (i - j);
			++i;
		}
		particles.resize(j);
		suppression_vector.resize(j);
		// update the ranges (code is useless from one TICK to the next. the only important value is NUMERIC LIMIT)
		size_t range_count = particles_ranges.size();
		for (size_t i = 0; i < range_count; ++i)
		{
			// read the range
			size_t start = particles_ranges[i].start;
			size_t end   = start + particles_ranges[i].count;
			// apply the suppression count
			start -= suppression_vector[start]; 
			end   -= suppression_vector[end];
			// update the structure
			particles_ranges[i].start = start; 
			particles_ranges[i].count = end - start;
		}
		// reset the suppression vector
#if 1
		for (size_t i = 0; i < j; ++i)
			suppression_vector[i] = 0;
#endif 
	}

protected:

	/** the particle behavior description */
	particle_desc_type particle_desc;
	/** the array containing the particles */
	std::vector<particle_type> particles;
};

// -----------------------------------------------------------------

template<typename PARTICLE_TYPE, typename VERTEX_TYPE>
class ParticleLayerDescBase
{
	CHAOS_PARTICLE_ALL_FRIENDS

public:

	using particle_type = typename PARTICLE_TYPE;
	using vertex_type = typename VERTEX_TYPE;

	inline bool MustDestroyParticle(particle_type const * p)
	{
		return false;
	}

	inline void UpdateParticle(float delta_time, particle_type * p)
	{

	}
};


// -----------------------------------------------------------------

class ParticleManager : public chaos::ReferencedObject
{
	CHAOS_PARTICLE_ALL_FRIENDS


};






// -----------------------------------------------------------------

class ParticleExample
{

};

class VertexExample
{

};

class ParticleLayerDescExample : public ParticleLayerDescBase<ParticleExample, VertexExample>
{


};


// undefine macros
#undef CHAOS_PARTICLE_CLASSES
#undef CHAOS_PARTICLE_FORWARD_DECL
#undef CHAOS_PARTICLE_FRIEND_DECL
#undef CHAOS_PARTICLE_ALL_FRIENDS