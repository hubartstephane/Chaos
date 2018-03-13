#pragma once

#include <chaos/StandardHeaders.h>
#include <chaos/ReferencedObject.h>
#include <chaos/RenderMaterial.h>
#include <chaos/BitmapAtlas.h>
#include <chaos/GPUProgramVariableProvider.h>


// -----------------------------------------------------------------

// all classes in this file
#define CHAOS_PARTICLE_CLASSES (ParticleRange) (ParticleRangeAllocation) (ParticleLayer) (ParticleManager) (ParticleLayerDesc)

// forward declaration
#define CHAOS_PARTICLE_FORWARD_DECL(r, data, elem) class elem;
BOOST_PP_SEQ_FOR_EACH(CHAOS_PARTICLE_FORWARD_DECL, _, CHAOS_PARTICLE_CLASSES)

// friendship macro
#define CHAOS_PARTICLE_FRIEND_DECL(r, data, elem) friend class elem;
#define CHAOS_PARTICLE_ALL_FRIENDS BOOST_PP_SEQ_FOR_EACH(CHAOS_PARTICLE_FRIEND_DECL, _, CHAOS_PARTICLE_CLASSES)

// ==============================================================
// PARTICLE RANGE
// ==============================================================

class ParticleRange
{
	CHAOS_PARTICLE_ALL_FRIENDS

public:

	/** the beginning of the range */
	size_t start = 0;
	/** the number of element in the range */
	size_t count = 0;
};

// ==============================================================
// PARTICLE RANGE ALLOCATION
// ==============================================================

class ParticleRangeAllocation : public chaos::ReferencedObject
{
	CHAOS_PARTICLE_ALL_FRIENDS

public:

	/** destructor */
	virtual ~ParticleRangeAllocation();
	/** remove the allocation from its layer */
	void RemoveFromLayer();

	/** get the range of concern */
	ParticleRange GetParticleRange() const;
	/** get the number of particles */
	size_t GetParticleCount() const;
	/** get the particles */
	void * GetParticleBuffer();
	/** get the particles */
	void const * GetParticleBuffer() const;

protected:

	/** the particle layer that contains the range */
	ParticleLayer * layer = nullptr;
	/** the index of the range in its array */
	size_t range_index = std::numeric_limits<size_t>::max();
	/** whether the particles existence in linked to the allocation lifetime */
	bool particles_owner = true;
};

// ==============================================================
// PARTICLE LAYER DESC
// ==============================================================

class ParticleLayerDesc : public chaos::ReferencedObject
{
	CHAOS_PARTICLE_ALL_FRIENDS

public:

	/** returns the size in memory of a particle */
	virtual size_t GetParticleSize() const;
	/** returns the size in memory of a vertex */
	virtual size_t GetVertexSize() const;
	/** returns true whether particles may destroyed themselves */
	virtual bool HasParticleLimitedLifeTime() const;
	/** returns true whether particles need to be updated */
	virtual bool AreParticlesDynamic() const;

protected:

	/** update all particles */
	virtual void UpdateParticles(float delta_time, void * particles, size_t particle_count);
	/** Test particle life. Destroy particles (move particles on deleted previous ones). returns the number of remaining particles */
	virtual size_t DestroyObsoletParticles(void * particles, size_t particle_count, size_t * deletion_vector);
};

// ==============================================================
// PARTICLE LAYER 
// ==============================================================

class ParticleLayer : public chaos::ReferencedObject
{
	CHAOS_PARTICLE_ALL_FRIENDS

public:

	static size_t const DESTROY_PARTICLE_MARK = std::numeric_limits<size_t>::max();

	/** constructor */
	ParticleLayer(ParticleLayerDesc * in_layer_desc);
	/** destructor */
	virtual ~ParticleLayer();

	/** get the total number of particles */
	size_t GetParticleCount() const;
	/** get the number of particles */
	size_t GetParticleCount(ParticleRange range) const;
	/** get the particles */
	void * GetParticleBuffer(ParticleRange range);
	/** get the particles */
	void const * GetParticleBuffer(ParticleRange range) const;

	/** returns the size in memory of a particle */
	size_t GetParticleSize() const { return particle_size;}
	/** returns the size in memory of a vertex */
	size_t GetVertexSize() const { return vertex_size;}
	/** returns true whether particles may destroyed themselves */
	bool HasParticleLimitedLifeTime() const;
	/** returns true whether particles need to be updated */
	bool AreParticlesDynamic() const;

	/** pause/resume the layer */
	void Pause(bool in_paused = true);
	/** returns whether the layer is paused */
	bool IsPaused() const;

	/** show/hide the layer */
	void Show(bool in_visible = true);
	/** returns whether the layer is visible */
	bool IsVisible() const;

	/** get the render order */
	int GetRenderOrder() const { return render_order; }
	/** set the render order */
	void SetRenderOrder(int in_render_order) { render_order = in_render_order; }

	/** change the material */
	void SetRenderMaterial(chaos::RenderMaterial * in_render_material) { render_material = in_render_material;}

	/** ticking the particle system */
	virtual void TickParticles(float delta_time);
	/** spawn a given number of particles */
	ParticleRange SpawnParticles(size_t count);
	/** spawn a given number of particles and keep a range */
	ParticleRangeAllocation *SpawnParticlesAndKeepRange(size_t count, bool particles_owner = true);

	/** mark any particle as to be destroyed next tick */
	void MarkParticlesToDestroy(size_t start, size_t count);

protected:

	/** unlink all particles allocations */
	void DetachAllParticleAllocations();
	/** internal method to remove a range from the layer */
	void RemoveParticleAllocation(ParticleRangeAllocation * allocation);
	/** internal method fix the ranges after particles destruction */
	void UpdateParticleRanges(size_t new_particle_count);
	/** internal method to update particles */
	void UpdateParticles(float delta_time);
	/** internal method to test whether particles should be destroyed (returns the number of particles still in the layer) */
	size_t DestroyObsoletParticles();

protected:

	/** the size of one particle */
	size_t particle_size = 0;
	/** the size of one vertex */
	size_t vertex_size = 0;
	/** the order of the layer in the manager */
	int render_order = 0;
	/** whether the layer is paused */
	bool paused = false;
	/** whether the layer is visible */
	bool visible = true;
	/** number of particles waiting for a destruction */
	size_t pending_kill_particles = 0;
	/** whether there was changes in particles, and a vertex array need to be recomputed */
	bool require_GPU_update = false;
	/** the material used to render the layer */
	boost::intrusive_ptr<chaos::RenderMaterial> render_material;
	/** the array containing the particles */
	std::vector<char> particles;
	/** a utility vector that is used to mark particles to destroy, then as an internal utility vector */
	std::vector<size_t> deletion_vector;
	/** particles ranges */
	std::vector<ParticleRange> particles_ranges;
	/** ranges reservations */
	std::vector<ParticleRangeAllocation*> range_allocations;
	/** the behavior description */
	boost::intrusive_ptr<ParticleLayerDesc> layer_desc;
};


// ==============================================================
// TParticleLayerDesc
// ==============================================================

template<typename PARTICLE_TYPE, typename VERTEX_TYPE>
class ParticleLayerTrait
{
public:

	using particle_type = PARTICLE_TYPE;

	using vertex_type = VERTEX_TYPE;

	bool lifetime_particles = false;

	bool dynamic_particles = false;
};

// ==============================================================
// TParticleLayerDesc
// ==============================================================

template<typename LAYER_TRAIT>
class TParticleLayerDesc : public ParticleLayerDesc
{
	CHAOS_PARTICLE_ALL_FRIENDS

public:

	using trait_type    = LAYER_TRAIT;
	using particle_type = typename trait_type::particle_type;
	using vertex_type   = typename trait_type::vertex_type;

public:

	/** constructor */
	TParticleLayerDesc(trait_type const & in_trait = trait_type()):
		trait(in_trait){}

	/** returns the size in memory of a particle */
	virtual size_t GetParticleSize() const override
	{
		return sizeof(particle_type);
	}
	/** returns the size in memory of a vertex */
	virtual size_t GetVertexSize() const override
	{
		return sizeof(vertex_type);
	}
	/** returns true whether particles have a limited lifetime */
	virtual bool HasParticleLimitedLifeTime() const
	{
		return trait.lifetime_particles;
	}
	/** returns true whether particles need to be updated */
	virtual bool AreParticlesDynamic() const
	{
		return trait.dynamic_particles;
	}

	/** loop for updating the particles */
	virtual void UpdateParticles(float delta_time, void * particles, size_t particle_count) override
	{
		particle_type * p = (particle_type*)particles;
		for (size_t i = 0; i < particle_count; ++i)
			trait.UpdateParticle(delta_time, &p[i]);
	}
	/** loop for destroying the particles */
	virtual size_t DestroyObsoletParticles(void * particles, size_t particle_count, size_t * deletion_vector) override
	{
		if (particle_count > 0)
		{
			particle_type * p = (particle_type*)particles;

			size_t i = 0;
			size_t j = 0;
			while (i < particle_count)
			{
				if (deletion_vector[i] != ParticleLayer::DESTROY_PARTICLE_MARK && !trait.IsParticleObsolet(&p[i])) // particle is still alive ?
				{
					if (i != j)
						p[j] = p[i]; // keep the particle by copying it 
					deletion_vector[i] = (i - j);
					++j;
				}
				else
					deletion_vector[i] = (i - j);
				++i;
			}
			return j;
		}
		return particle_count; // no destruction
	}

protected:

	/** internal description */
	trait_type trait;
};

// ==============================================================
// CALLBACKS
// ==============================================================

class ParticleManager : public chaos::ReferencedObject
{
	CHAOS_PARTICLE_ALL_FRIENDS

public:

	/** change the bitmap atlas */
	void SetTextureAtlas(chaos::BitmapAtlas::TextureArrayAtlas * in_atlas);
	/** display all the particles */
	void Display(chaos::GPUProgramVariableProvider * uniform_provider);

protected:

protected:

	/** the texture atlas */
	chaos::BitmapAtlas::TextureArrayAtlas * atlas = nullptr;
};






// ==============================================================
// CALLBACKS
// ==============================================================

class ParticleExample
{
	glm::vec3 position;

};

class VertexExample
{

};

class ParticleExampleTrait : public ParticleLayerTrait<ParticleExample, VertexExample>
{
public:

	ParticleExampleTrait()
	{
		lifetime_particles = true;
		dynamic_particles = true;
	}

	bool IsParticleObsolet(ParticleExample * p)
	{
		return false;
	}
	void UpdateParticle(float delta_time, ParticleExample * particle)
	{
		particle = particle;

	}
};



class ParticleLayerDescExample : public TParticleLayerDesc<ParticleExampleTrait>
{


};


// undefine macros
#undef CHAOS_PARTICLE_CLASSES
#undef CHAOS_PARTICLE_FORWARD_DECL
#undef CHAOS_PARTICLE_FRIEND_DECL
#undef CHAOS_PARTICLE_ALL_FRIENDS