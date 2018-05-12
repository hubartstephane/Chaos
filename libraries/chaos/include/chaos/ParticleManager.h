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
#define CHAOS_PARTICLE_CLASSES (ParticleAllocation) (ParticleLayer) (ParticleManager) (ParticleLayerDesc)

	// forward declaration
#define CHAOS_PARTICLE_FORWARD_DECL(r, data, elem) class elem;
	BOOST_PP_SEQ_FOR_EACH(CHAOS_PARTICLE_FORWARD_DECL, _, CHAOS_PARTICLE_CLASSES)

		// friendship macro
#define CHAOS_PARTICLE_FRIEND_DECL(r, data, elem) friend class elem;
#define CHAOS_PARTICLE_ALL_FRIENDS BOOST_PP_SEQ_FOR_EACH(CHAOS_PARTICLE_FRIEND_DECL, _, CHAOS_PARTICLE_CLASSES)

		// ==============================================================
		// ParticleAllocation
		// ==============================================================

	class ParticleAllocation : public ReferencedObject
	{
		CHAOS_PARTICLE_ALL_FRIENDS

	public:

		/** constructor */
		ParticleAllocation(ParticleLayer * in_layer);
		/** destructor */
		virtual ~ParticleAllocation();
		/** remove the allocation from its layer */
		void RemoveFromLayer();

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

		/** get the number of particles */
		virtual size_t GetParticleCount() const;
		/** get the particles */
		virtual void * GetParticleBuffer();
		/** get the particles */
		virtual void const * GetParticleBuffer() const;
		/** resize the particles */
		virtual bool Resize(size_t new_count);

		/** get the layer for this allocation */
		ParticleLayer * GetLayer() { return layer; }
		/** get the layer for this allocation */
		ParticleLayer const * GetLayer() const { return layer; }

	protected:

		/** called whenever the allocation is removed from the layer */
		virtual void OnRemovedFromLayer();
		/** require the layer to update the GPU buffer */
		void ConditionalRequireGPUUpdate(bool ignore_visibility, bool ignore_particle_count);

	protected:

		/** the particle layer that contains the range */
		ParticleLayer * layer = nullptr;
		/** whether the allocation is paused */
		bool paused = false;
		/** whether the allocation is visible */
		bool visible = true;
	};

	// ==============================================================
	// TypedParticleAllocation
	// ==============================================================

	template<typename PARTICLE_TYPE>
	class TypedParticleAllocation : public ParticleAllocation
	{
	public:

		using particle_type = PARTICLE_TYPE;

		/** constructor */
		TypedParticleAllocation(ParticleLayer * in_layer) :
			ParticleAllocation(in_layer)
		{
		}
		/** destructor */
		virtual ~TypedParticleAllocation()
		{
			RemoveFromLayer(); // this call is not in ParticleAllocation::~ParticleAllocation(), because when destructor is incomming, the number of particles becomes invalid (0)
		}
		/** override */
		virtual size_t GetParticleCount() const override
		{
			return particles.size();
		}
		/** override */
		virtual void * GetParticleBuffer() override
		{
			if (particles.size() == 0)
				return nullptr;
			return &particles[0];
		}
		/** override */
		virtual void const * GetParticleBuffer() const override
		{
			if (particles.size() == 0)
				return nullptr;
			return &particles[0];
		}
		/** override */
		virtual bool Resize(size_t new_count) override
		{
			// test whether this is valid
			if (!IsAttachedToLayer())
				return false;
			// early exit
			if (new_count == particles.size())
				return true;
			// increment the number of particles
			particles.resize(new_count);
			// notify the layer
			ConditionalRequireGPUUpdate(false, true);
			return true;
		}

	protected:

		/** override */
		virtual void OnRemovedFromLayer() override
		{
			ParticleAllocation::OnRemovedFromLayer();
			particles.clear();						
		}

	protected:

		/** the particles buffer */
		std::vector<particle_type> particles;
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

	protected:

		/** create an allocation */
		virtual ParticleAllocation * NewAllocation(ParticleLayer * in_layer);

		/** transform the particles into vertices */
		virtual size_t ParticlesToVertices(void const * particles, size_t particles_count, char * vertices, ParticleAllocation * allocation) const;
		/** update all particles */
		virtual size_t UpdateParticles(float delta_time, void * particles, size_t particle_count, ParticleAllocation * allocation);
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

		/** override */
		virtual size_t GetParticleSize() const override
		{
			return sizeof(particle_type);
		}
		/** override */
		virtual size_t GetVertexSize() const override
		{
			return sizeof(vertex_type);
		}
		/** override */
		virtual bool AreParticlesDynamic() const
		{
			return trait.dynamic_particles;
		}
		/** override */
		virtual VertexDeclaration GetVertexDeclaration() const override
		{
			return GetTypedVertexDeclaration(boost::mpl::identity<vertex_type>());
		}
		/** override */
		virtual ParticleAllocation * NewAllocation(ParticleLayer * in_layer) override
		{
			return new TypedParticleAllocation<particle_type>(in_layer);
		}

		/** override */
		virtual size_t UpdateParticles(float delta_time, void * particles, size_t particle_count, ParticleAllocation * allocation) override
		{
			particle_type * p = (particle_type *)particles;

			// tick all particles. overide all particles that have been destroyed by next on the array
			size_t j = 0;
			for (size_t i = 0 ; i < particle_count ; ++i)			
			{
				if (!trait.UpdateParticle(delta_time, &p[i], allocation)) // particle not destroyed ?
				{
					if (i != j)
						p[j] = p[i]; 
					++j;
				}
			}
			return j; // final number of particles
		}

		/** override */
		virtual size_t ParticlesToVertices(void const * particles, size_t particles_count, char * vertices, ParticleAllocation * allocation) const override
		{
			size_t result = 0;

			size_t vertices_per_particle = GetVerticesCountPerParticles();

			particle_type const * p = (particle_type const *)particles;
			vertex_type * v = (vertex_type *)vertices;

			for (size_t i = 0; i < particles_count; ++i)
			{
				size_t new_vertices = trait.ParticleToVertices(&p[i], v, vertices_per_particle, allocation);
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
	// PARTICLE LAYER 
	// ==============================================================

	class ParticleLayer : public ReferencedObject
	{
		CHAOS_PARTICLE_ALL_FRIENDS

	public:

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

		/** returns the number of particle count */
		size_t ComputeMaxParticleCount() const;
		/** returns the size in memory of a particle */
		size_t GetParticleSize() const { return layer_desc->GetParticleSize(); }
		/** returns the size in memory of a vertex */
		size_t GetVertexSize() const { return layer_desc->GetVertexSize(); }
		/** returns the number of vertices required for each particles */
		size_t GetVerticesCountPerParticles() const { return layer_desc->GetVerticesCountPerParticles(); }
		/** returns true whether particles need to be updated */
		bool AreParticlesDynamic() const { return layer_desc->AreParticlesDynamic(); }

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

		/** spawn a given number of particles */
		ParticleAllocation * SpawnParticles(size_t count);

		/** ticking the particle system */
		virtual void TickParticles(float delta_time);
		/** draw the layer */
		void Display(RenderMaterial const * material_override, GPUProgramProviderBase const * uniform_provider, InstancingInfo const & instancing = InstancingInfo()) const;

	protected:

		/** unlink all particles allocations */
		void DetachAllParticleAllocations();
		/** internal method to remove a range from the layer */
		void RemoveParticleAllocation(ParticleAllocation * allocation);

		/** update the GPU buffers (returns the number of vertices inserted) */
		size_t UpdateGPUBuffers() const;
		/** internal method to update the GPU buffers */
		size_t DoUpdateGPUBuffers(char * buffer, size_t vertex_buffer_size) const;

		/** update the vertex declaration */
		void UpdateVertexDeclaration() const;
		/** the effective rendering */
		void DoDisplay(size_t vcount, RenderMaterial const * final_material, GPUProgramProviderBase const * uniform_provider, InstancingInfo const & instancing) const;

		/** internal method to update particles (returns true whether there was real changes) */
		virtual bool UpdateParticles(float delta_time);

	protected:

		/** the name of the layer */
		std::string name;
		/** the ID of the layer */
		int id = 0;

		/** the order of the layer in the manager */
		int render_order = 0;
		/** whether the layer is paused */
		bool paused = false;
		/** whether the layer is visible */
		bool visible = true;

		/** whether there was changes in particles, and a vertex array need to be recomputed */
		mutable bool require_GPU_update = false;

		/** the material used to render the layer */
		boost::intrusive_ptr<RenderMaterial> render_material;

		/** particles allocations */
		std::vector<ParticleAllocation*> particles_allocations;
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
	// ParticleLayerTrait
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
		bool UpdateParticle(float delta_time, particle_type * particle, ParticleAllocation * allocation) const
		{ 
			return false; 
		}
		/** by default, particle to vertex do nothing */
		size_t ParticleToVertices(particle_type const * particle, vertex_type * vertices, size_t vertices_per_particles, ParticleAllocation * allocation) const
		{ 
			return 0; 
		}

	public:

		/** whether the particles are dynamic */
		bool dynamic_particles = true;
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


		/** create a layer and add it to the manager */
		ParticleLayer * AddLayer(ParticleLayerDesc * layer_desc);
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

	// undefine macros
#undef CHAOS_PARTICLE_CLASSES
#undef CHAOS_PARTICLE_FORWARD_DECL
#undef CHAOS_PARTICLE_FRIEND_DECL
#undef CHAOS_PARTICLE_ALL_FRIENDS

}; // namespace chaos

