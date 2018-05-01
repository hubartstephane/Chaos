#pragma once

#include <chaos/StandardHeaders.h>
#include <chaos/ReferencedObject.h>
#include <chaos/RenderMaterial.h>
#include <chaos/BitmapAtlas.h>
#include <chaos/VertexBuffer.h>
#include <chaos/VertexDeclaration.h>
#include <chaos/VertexArrayCache.h>
#include <chaos/GPUProgramProvider.h>
#include <chaos/DrawPrimitive.h>
#include <chaos/TextureArrayAtlas.h>
#include <chaos/ParticleTools.h>

namespace chaos
{
	// ==============================================================
	// FORWARD DECLARATION / FRIENDSHIP MACROS
	// ==============================================================


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

	class ParticleRangeAllocation : public ReferencedObject
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

		/** returns true whether the allocation is attached to a layer */
		bool IsAttachedToLayer() const;

		/** pause/resume the layer */
		void Pause(bool in_paused = true);
		/** returns whether the layer is paused */
		bool IsPaused() const;

		/** show/hide the layer */
		void Show(bool in_visible = true);
		/** returns whether the layer is visible */
		bool IsVisible() const;

		/** resize the particles */
		bool Resize(size_t new_count);

	protected:

		/** get a pointer on the range in layer */
		ParticleRange * GetParticleRangeReference();

		/** the particle layer that contains the range */
		ParticleLayer * layer = nullptr;
		/** the index of the range in its array */
		size_t range_index = std::numeric_limits<size_t>::max();
		/** whether the particles existence in linked to the allocation lifetime */
		bool particles_owner = true;

		/** whether the allocation is paused */
		bool paused = false;
		/** whether the allocation is visible */
		bool visible = true;
	};

	// ==============================================================
	// UpdateParticleData
	// ==============================================================

	class UpdateParticleData
	{

	public:

		/** delta time for the tick */
		float  delta_time = 0.0;

		/** first particle in this batch */
		void * first_particle = nullptr;

		/** number of particle in this batch */
		size_t particle_count = 0;
		/** the deletion vector */
		size_t * deletion_vector = nullptr;

		/** current layer */
		ParticleLayer * layer = nullptr;
	};

	// ==============================================================
	// TypedUpdateParticleData
	// ==============================================================

	template<typename PARTICLE_TYPE>
	class TypedUpdateParticleData
	{
		using particle_type = PARTICLE_TYPE;

	public:

		/** delta time for the tick */
		float  delta_time = 0.0;

		/** current particle */
		particle_type * particle = nullptr;
		/** first particle in this batch */
		particle_type * first_particle = nullptr;

		/** the index of the current particle in the array */
		size_t particle_index = 0;
		/** number of particle in this batch */
		size_t particle_count = 0;
		/** the deletion vector */
		size_t * deletion_vector = nullptr;

		/** current layer */
		ParticleLayer * layer = nullptr;
		/** current allocation */
		ParticleRangeAllocation * allocation = nullptr;
	};

	// ==============================================================
	// PARTICLE LAYER DESC
	// ==============================================================

	class ParticleLayerDesc : public ReferencedObject
	{
		CHAOS_PARTICLE_ALL_FRIENDS

	public:

		/** returns the size in memory of a particle */
		virtual size_t GetParticleSize() const;
		/** returns the size in memory of a vertex */
		virtual size_t GetVertexSize() const;
		/** returns the number of vertices required for each particles */
		virtual size_t GetVerticesCountPerParticles() const;

		/** returns true whether particles need to be updated */
		virtual bool AreParticlesDynamic() const;

		/** get the vertex declaration for that layer */
		virtual VertexDeclaration GetVertexDeclaration() const;

		/** transform the particles into vertices */
		virtual size_t ParticlesToVertices(char const * particles, size_t particles_count, char * vertices) const;

	protected:

		/** create an allocation */
		virtual ParticleRangeAllocation * NewRangeAllocation();

		/** update all particles */
		virtual size_t UpdateParticles(UpdateParticleData & data, ParticleRangeAllocation * allocation);
		/** Test particle life. Destroy particles (move particles on deleted previous ones). returns the number of remaining particles */
		virtual size_t CleanDestroyedParticles(void * particles, size_t particle_count, size_t * deletion_vector);
	};

	// ==============================================================
	// PARTICLE LAYER 
	// ==============================================================

	class ParticleLayer : public ReferencedObject
	{
		CHAOS_PARTICLE_ALL_FRIENDS

	public:

		static size_t const DESTROY_PARTICLE_MARK = std::numeric_limits<size_t>::max();

		/** constructor */
		ParticleLayer(ParticleLayerDesc * in_layer_desc);
		/** destructor */
		virtual ~ParticleLayer();

		/** get the name of the object */
		char const * GetName() const { return name.c_str(); }
		/** get the ID of the object */
		int GetLayerID() const { return id; }

		/** Set the name method */
		void SetLayerName(char const * in_name);
		/** Set the id method */
		void SetLayerID(int in_id);

		/** get the total number of particles */
		size_t GetParticleCount() const;
		/** get the number of particles */
		size_t GetParticleCount(ParticleRange range) const;
		/** get the particles */
		void * GetParticleBuffer(ParticleRange range);
		/** get the particles */
		void const * GetParticleBuffer(ParticleRange range) const;

		/** get the particle by its index */
		void * GetParticle(size_t index);
		/** get the particle by its index */
		void const * GetParticle(size_t index) const;

		/** returns the size in memory of a particle */
		size_t GetParticleSize() const { return particle_size; }
		/** returns the size in memory of a vertex */
		size_t GetVertexSize() const { return vertex_size; }
		/** returns the number of vertices required for each particles */
		size_t GetVerticesCountPerParticles() const { return vertices_count_per_particles; }
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
		void SetRenderMaterial(RenderMaterial * in_render_material) { render_material = in_render_material; }
		/** get the material const method */
		RenderMaterial const * GetRenderMaterial() const { return render_material.get(); }

		/** ticking the particle system */
		virtual void TickParticles(float delta_time);
		/** spawn a given number of particles */
		ParticleRange SpawnParticles(size_t count);
		/** spawn a given number of particles and keep a range */
		ParticleRangeAllocation *SpawnParticlesAndKeepRange(size_t count, bool particles_owner = true);

		/** mark any particle as to be destroyed next tick */
		void MarkParticlesToDestroy(size_t start, size_t count);

		/** draw the layer */
		void Display(RenderMaterial const * material_override, GPUProgramProviderBase const * uniform_provider, InstancingInfo const & instancing = InstancingInfo()) const;

	protected:

		/** unlink all particles allocations */
		void DetachAllParticleAllocations();
		/** internal method to remove a range from the layer */
		void RemoveParticleAllocation(ParticleRangeAllocation * allocation);
		/** internal method fix the ranges after particles destruction */
		void UpdateParticleRanges(size_t new_particle_count);
		/** internal method to update particles */
		size_t UpdateParticles(float delta_time);
		/** internal method to test whether particles should be destroyed (returns the number of particles still in the layer) */
		size_t CleanDestroyedParticles();
		/** update the GPU buffers (returns the number of vertices inserted) */
		size_t UpdateGPUBuffers() const;
		/** internal method to update the GPU buffers */
		size_t DoUpdateGPUBuffers(char * buffer, size_t vertex_buffer_size) const;

		/** update the vertex declaration */
		void UpdateVertexDeclaration() const;
		/** the effective rendering */
		void DoDisplay(size_t vcount, RenderMaterial const * final_material, GPUProgramProviderBase const * uniform_provider, InstancingInfo const & instancing) const;

	protected:

		/** the name of the layer */
		std::string name;
		/** the ID of the layer */
		int id = 0;

		/** the size of one particle */
		size_t particle_size = 0;
		/** the size of one vertex */
		size_t vertex_size = 0;
		/** the number of vertices required to render one particle */
		size_t vertices_count_per_particles = 0;

		/** the order of the layer in the manager */
		int render_order = 0;
		/** whether the layer is paused */
		bool paused = false;
		/** whether the layer is visible */
		bool visible = true;

		/** number of particles waiting for a destruction */
		size_t pending_kill_particles = 0;
		/** whether there was changes in particles, and a vertex array need to be recomputed */
		mutable bool require_GPU_update = false;

		/** the material used to render the layer */
		boost::intrusive_ptr<RenderMaterial> render_material;
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

		/** the vertex declaration */
		mutable VertexDeclaration vertex_declaration;
		/** the vertex buffer for the rendering */
		mutable boost::intrusive_ptr<VertexBuffer> vertex_buffer;
		/** the cache for vertex array */
		mutable VertexArrayCache vertex_array_cache;

		/** number of used vertices in the vertex buffer */
		mutable size_t vertices_count = 0;
	};


	// ==============================================================
	// TypedParticleLayerDesc
	// ==============================================================

	template<typename PARTICLE_TYPE, typename VERTEX_TYPE>
	class ParticleLayerTrait
	{
	public:

		/** the type for one particle */
		using particle_type = PARTICLE_TYPE;
		/** the type for one vertex */
		using vertex_type = VERTEX_TYPE;

		/** constructor */
		ParticleLayerTrait(bool in_dynamic_particles = true) :
			dynamic_particles(in_dynamic_particles)
		{
		}

		/** by default, update do nothing */
		bool UpdateParticle(TypedUpdateParticleData<particle_type> & data){ return false; }

	public:

		/** whether the particles are dynamic */
		bool dynamic_particles = true;
	};

	// ==============================================================
	// TypedParticleLayerDesc
	// ==============================================================

	template<typename LAYER_TRAIT>
	class TypedParticleLayerDesc : public ParticleLayerDesc
	{
		CHAOS_PARTICLE_ALL_FRIENDS

	public:

		using trait_type = LAYER_TRAIT;
		using particle_type = typename trait_type::particle_type;
		using vertex_type = typename trait_type::vertex_type;

	public:

		/** constructor */
		TypedParticleLayerDesc(trait_type const & in_trait = trait_type()) :
			trait(in_trait) {}

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
		/** returns true whether particles need to be updated */
		virtual bool AreParticlesDynamic() const
		{
			return trait.dynamic_particles;
		}

		/** loop for updating the particles (returns the number of particles destroyed) */
		virtual size_t UpdateParticles(UpdateParticleData & data, ParticleRangeAllocation * allocation) override
		{
			size_t result = 0;

			TypedUpdateParticleData<particle_type> typed_data;
			typed_data.delta_time = data.delta_time;
			typed_data.first_particle = (particle_type*)data.first_particle;
			typed_data.particle_count = data.particle_count;
			typed_data.layer = data.layer;
			typed_data.deletion_vector = data.deletion_vector;
			typed_data.allocation = allocation;

			for (size_t i = 0; i < typed_data.particle_count; ++i)
			{
				if (typed_data.deletion_vector[i] != ParticleLayer::DESTROY_PARTICLE_MARK)
				{
					typed_data.particle = &typed_data.first_particle[i];
					typed_data.particle_index = i;

					if (trait.UpdateParticle(typed_data)) // update the particle and test whether it is destroyed
					{
						typed_data.deletion_vector[i] = ParticleLayer::DESTROY_PARTICLE_MARK;
						++result;
					}
				}
			}
			return result;
		}
		/** loop for destroying the particles */
		virtual size_t CleanDestroyedParticles(void * particles, size_t particle_count, size_t * deletion_vector) override
		{
			size_t i = 0;
			size_t j = 0;

			particle_type * p = (particle_type*)particles;

			while (i < particle_count)
			{
				if (deletion_vector[i] != ParticleLayer::DESTROY_PARTICLE_MARK) // already destroyed ?
				{
					deletion_vector[i] = j; // the position of particle 'i' is now 'j'
					if (i != j)
						p[j] = p[i]; // keep the particle by copying it 
					++j;
				}
				++i;
			}
			return particle_count + (j - i);
		}

		/** get the vertex declaration */
		virtual VertexDeclaration GetVertexDeclaration() const override
		{
			return GetTypedVertexDeclaration(boost::mpl::identity<vertex_type>());
		}

		/** convert particles in vertices */
		virtual size_t ParticlesToVertices(char const * particles, size_t particles_count, char * vertices) const override
		{
			size_t vertex_size = GetVertexSize();
			size_t vertices_per_particle = GetVerticesCountPerParticles();

			particle_type const * p = (particle_type const *)particles;
			vertex_type * v = (vertex_type *)vertices;

			size_t result = 0;
			for (size_t i = 0; i < particles_count; ++i)
			{
				size_t new_vertices = trait.ParticleToVertex(&p[i], v, vertices_per_particle);
				assert(new_vertices <= vertices_per_particle);				
				result += new_vertices;
				v += new_vertices;
			}
			return result;
		}

	protected:

		/** internal description */
		trait_type trait;
	};

	// ==============================================================
	// ParticleManager
	// ==============================================================

	class ParticleManager : public ReferencedObject
	{
		CHAOS_PARTICLE_ALL_FRIENDS

	public:

		/** change the bitmap atlas */
		void SetTextureAtlas(BitmapAtlas::TextureArrayAtlas * in_atlas);
		/** display all the particles */
		void Display(GPUProgramProviderBase const * uniform_provider, InstancingInfo const & instancing = InstancingInfo()) const;

		/** Search a layer by its name */
		ParticleLayer * FindLayer(char const * name);
		/** Search a layer by its name */
		ParticleLayer const * FindLayer(char const * name) const;
		/** Search a layer by its id */
		ParticleLayer * FindLayer(int id);
		/** Search a layer by its id */
		ParticleLayer const * FindLayer(int id) const;

		/** add a layer in the manager */
		void AddLayer(ParticleLayer * layer);
		/** remove a layer from the manager */
		void RemoveLayer(ParticleLayer * layer);

		/** tick the manager */
		void Tick(float delta_time);

	protected:

		/** find the index of a layer */
		size_t FindLayerIndex(ParticleLayer * layer) const;
		/** sort the layers by rendering order */
		void SortLayers(bool test_program_id) const;
		/** test whether the layers are sort correctly */
		bool AreLayersSorted(bool test_program_id) const;
		/** change the GL rendering state */
		virtual void UpdateRenderingStates(bool begin) const;

	protected:

		/** the texture atlas */
		BitmapAtlas::TextureArrayAtlas * atlas = nullptr;
		/** the layers */
		mutable std::vector<boost::intrusive_ptr<ParticleLayer>> layers;
	};

	// ==============================================================
	// Example of Trait usage
	// ==============================================================

#if 0

	/** class for particles */
	class ParticleExample
	{
		...
	};

	/** class for vertices (only GPU usable data) */
	class VertexExample
	{
		...
	};

	VertexDeclaration GetTypedVertexDeclaration(boost::mpl::identity<VertexExample>) const { .... }

	/** the traits */
	class ParticleExampleTrait : public chaos::ParticleLayerTrait<ParticleExample, VertexExample>
	{
	public:

		/** updates the particle. returns true whether the particle is to be destroyed */
		bool UpdateParticle(TypedUpdateParticleData<ParticleExample> & data);
		/** take one particle and transforms it into several vertices (usually 6 for 2 triangles */
		size_t ParticleToVertex(ParticleExample const * particle, VertexExample * vertices, size_t vertices_per_particles) const;
	};

#endif


	// undefine macros
#undef CHAOS_PARTICLE_CLASSES
#undef CHAOS_PARTICLE_FORWARD_DECL
#undef CHAOS_PARTICLE_FRIEND_DECL
#undef CHAOS_PARTICLE_ALL_FRIENDS

}; // namespace chaos

