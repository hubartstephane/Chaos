#pragma once

#include <chaos/StandardHeaders.h>
#include <chaos/ReferencedObject.h>
#include <chaos/GPURenderMaterial.h>
#include <chaos/GPUVertexBuffer.h>
#include <chaos/ReferencedObject.h>
#include <chaos/GPUVertexDeclaration.h>
#include <chaos/GPUVertexArrayCache.h>
#include <chaos/GPUProgramProvider.h>
#include <chaos/DrawPrimitive.h>
#include <chaos/TextureArrayAtlas.h>
#include <chaos/ClassTools.h>
#include <chaos/ParticleTools.h>
#include <chaos/Renderable.h>
#include <chaos/Tickable.h>
#include <chaos/EmptyClass.h>
#include <chaos/ParticleAccessor.h>

namespace chaos
{
	// ==============================================================
	// XXX : remark on ParticleAllocation
	//
	//         The ParticleAllocation life time is special. see ReferenceCount remark in .cpp !!! 
	// ==============================================================


	// ==============================================================
	// FORWARD DECLARATION / FRIENDSHIP MACROS
	// ==============================================================


	// all classes in this file
#define CHAOS_PARTICLE_CLASSES (ParticleAllocationBase) (ParticleLayerBase) (ParticleManager)

	// forward declaration
#define CHAOS_PARTICLE_FORWARD_DECL(r, data, elem) class elem;
	BOOST_PP_SEQ_FOR_EACH(CHAOS_PARTICLE_FORWARD_DECL, _, CHAOS_PARTICLE_CLASSES)

		// friendship macro
#define CHAOS_PARTICLE_FRIEND_DECL(r, data, elem) friend class elem;
#define CHAOS_PARTICLE_ALL_FRIENDS BOOST_PP_SEQ_FOR_EACH(CHAOS_PARTICLE_FRIEND_DECL, _, CHAOS_PARTICLE_CLASSES)

// detect whether class has a member named XXX (use has_XXX<T>::value => bool => convert into boost::mpl::bool_)
BOOST_DECLARE_HAS_MEMBER(has_dynamic_particles, dynamic_particles);
BOOST_DECLARE_HAS_MEMBER(has_dynamic_vertices, dynamic_vertices);
BOOST_DECLARE_HAS_MEMBER(has_vertices_per_particle, vertices_per_particle);
// detect whether classes have some functions
CHAOS_GENERATE_HAS_FUNCTION_SIGNATURE(Tick);
CHAOS_GENERATE_HAS_FUNCTION_SIGNATURE(UpdateParticle);
CHAOS_GENERATE_HAS_FUNCTION_SIGNATURE(ParticleToVertices);
CHAOS_GENERATE_HAS_FUNCTION_SIGNATURE(BeginUpdateParticles);
CHAOS_GENERATE_HAS_FUNCTION_SIGNATURE(BeginParticlesToVertices);




CHAOS_GENERATE_HAS_FUNCTION_METACLASS(Tick)
//CHAOS_GENERATE_HAS_FUNCTION_METACLASS(UpdateParticle)
//CHAOS_GENERATE_HAS_FUNCTION_METACLASS(ParticleToVertices)
CHAOS_GENERATE_HAS_FUNCTION_METACLASS(BeginUpdateParticles)
CHAOS_GENERATE_HAS_FUNCTION_METACLASS(BeginParticlesToVertices)
// detect whether class have a nested class
CHAOS_GENERATE_HAS_TRAIT(LayerTrait)

// ==============================================================
// ParticleTraitTools
// ==============================================================

class ParticleTraitTools
{
public:

	/** returns the number of vertices require for one particle */
	template<typename TRAIT_TYPE>
	static size_t GetVerticesPerParticle(TRAIT_TYPE const & trait)
	{ 
		return DoGetVerticesPerParticle(trait, boost::mpl::bool_<has_vertices_per_particle<TRAIT_TYPE>::value>());
	}
	/** returns whether the vertices are dynamic */
	template<typename TRAIT_TYPE>
	static bool AreVerticesDynamic(TRAIT_TYPE const & trait)
	{ 
		return DoAreVerticesDynamic(trait, boost::mpl::bool_<has_dynamic_vertices<TRAIT_TYPE>::value>());
	}
	/** returns whether the particles are dynamic */
	template<typename TRAIT_TYPE>
	static bool AreParticlesDynamic(TRAIT_TYPE const & trait)
	{ 
		return DoAreParticlesDynamic(trait, boost::mpl::bool_<has_dynamic_particles<TRAIT_TYPE>::value>());
	}

	/** returns whether the vertices are dynamic (without an instance to read) */
	template<typename TRAIT_TYPE>
	static bool AreVerticesDynamicStatic()
	{
		return DoAreVerticesDynamicStatic<TRAIT_TYPE>(boost::mpl::bool_<has_dynamic_vertices<TRAIT_TYPE>::value>());
	}
	/** returns whether the particles are dynamic (without an instance to read) */
	template<typename TRAIT_TYPE>
	static bool AreParticlesDynamicStatic()
	{
		return DoAreParticlesDynamicStatic<TRAIT_TYPE>(boost::mpl::bool_<has_dynamic_particles<TRAIT_TYPE>::value>());
	}

protected:

	/** internal method */
	template<typename TRAIT_TYPE>
	static bool DoAreVerticesDynamic(TRAIT_TYPE const & trait, boost::mpl::false_ HAS_DYNAMIC_VERTICES) 
	{ 
		return true; // default value
	} 
	/** internal method */
	template<typename TRAIT_TYPE>
	static bool DoAreVerticesDynamic(TRAIT_TYPE const & trait, boost::mpl::true_ HAS_DYNAMIC_VERTICES) 
	{ 
		return trait.dynamic_vertices; 
	}
	/** internal method */
	template<typename TRAIT_TYPE>
	static bool DoAreParticlesDynamic(TRAIT_TYPE const & trait, boost::mpl::false_ HAS_DYNAMIC_PARTICLES) 
	{ 
		return true; // default value																																																											 
	} 
	/** internal method */
	template<typename TRAIT_TYPE>
	static bool DoAreParticlesDynamic(TRAIT_TYPE const & trait, boost::mpl::true_ HAS_DYNAMIC_PARTICLES) 
	{ 
		return trait.dynamic_particles; 
	}


	/** internal method */
	template<typename TRAIT_TYPE>
	static bool DoAreVerticesDynamicStatic(boost::mpl::false_ HAS_DYNAMIC_VERTICES)
	{
		return true; // default value
	}
	/** internal method */
	template<typename TRAIT_TYPE>
	static bool DoAreVerticesDynamicStatic(boost::mpl::true_ HAS_DYNAMIC_VERTICES)
	{
		return TRAIT_TYPE::dynamic_vertices;
	}
	/** internal method */
	template<typename TRAIT_TYPE>
	static bool DoAreParticlesDynamicStatic(boost::mpl::false_ HAS_DYNAMIC_PARTICLES)
	{
		return true; // default value																																																											 
	}
	/** internal method */
	template<typename TRAIT_TYPE>
	static bool DoAreParticlesDynamicStatic(boost::mpl::true_ HAS_DYNAMIC_PARTICLES)
	{
		return TRAIT_TYPE::dynamic_particles;
	}

	/** internal method */
	template<typename TRAIT_TYPE>
	static size_t DoGetVerticesPerParticle(TRAIT_TYPE const & trait, boost::mpl::false_ HAS_VERTICES_PER_PARTICLE)
	{ 
		return 2 * 3; // default value (2 triangles to render a quad)
	} 
	/** internal method */
	template<typename TRAIT_TYPE>
	static size_t DoGetVerticesPerParticle(TRAIT_TYPE const & trait, boost::mpl::true_ HAS_VERTICES_PER_PARTICLE)
	{ 
		return trait.vertices_per_particle; 
	}
};

		// ==============================================================
		// ParticleAllocationBase
		// ==============================================================

	class ParticleAllocationBase : public ReferencedObject
	{
		CHAOS_PARTICLE_ALL_FRIENDS

	public:

		/** constructor */
		ParticleAllocationBase(ParticleLayerBase * in_layer);
		/** destructor */
		virtual ~ParticleAllocationBase();

		/** Reference count updated : 2 use cases (external reference or not) */
		virtual void SubReference(SharedPointerPolicy policy) override;

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

		/** returns the ID representing the class of the particle */
		virtual ClassTools::ClassRegistration const * GetParticleClass() const { return nullptr; }

		/** get the size of one particle */
		virtual size_t GetParticleSize() const { return 0; }
		/** get the number of particles */
		virtual size_t GetParticleCount() const { return 0; }
		/** get the particles */
		virtual void * GetParticleBuffer() { return nullptr; }
		/** get the particles */
		virtual void const * GetParticleBuffer() const { return nullptr; }
		/** resize the particles */
		virtual bool Resize(size_t new_count) { return true; }

		/** increment number of particles */
		bool AddParticles(size_t extra_count);

		/** set the empty callback */
		void SetDestroyWhenEmpty(bool in_destroy_when_empty) { destroy_when_empty = in_destroy_when_empty; }
		/** returns whether the allocation is to be destroyed when empty */
		bool GetDestroyWhenEmpty() const { return destroy_when_empty; }

		/** returns true whether the class required is compatible with the one store in the buffer */
		template<typename PARTICLE_TYPE>
		bool IsParticleClassCompatible(bool accept_bigger_particle) const
		{
			return ParticleTools::IsParticleClassCompatible<PARTICLE_TYPE>(GetParticleClass(), GetParticleSize(), accept_bigger_particle);
		}

		/** get an accessor for the particles */
		template<typename PARTICLE_TYPE>
		ParticleAccessor<PARTICLE_TYPE> GetParticleAccessor()
		{
			assert(IsParticleClassCompatible<PARTICLE_TYPE>(true));
			return ParticleAccessor<PARTICLE_TYPE>(GetParticleBuffer(), GetParticleCount(), GetParticleSize());
		}

		/** get an accessor for the particles */
		template<typename PARTICLE_TYPE>
		ParticleConstAccessor<PARTICLE_TYPE> GetParticleConstAccessor() const
		{
			assert(IsParticleClassCompatible<PARTICLE_TYPE>(true));
			return ParticleConstAccessor<PARTICLE_TYPE>(GetParticleBuffer(), GetParticleCount(), GetParticleSize());
		}

		/** get an accessor for the particles */
		template<typename PARTICLE_TYPE>
		ParticleConstAccessor<PARTICLE_TYPE> GetParticleAccessor() const
		{
			return GetParticleConstAccessor<PARTICLE_TYPE>();
		}

		/** returns a pointer on the first particle with strict class checking */
		template<typename PARTICLE_TYPE>
		PARTICLE_TYPE * GetParticleCheckedBuffer()
		{
			assert(IsParticleClassCompatible<PARTICLE_TYPE>(false));
			return (PARTICLE_TYPE*)GetParticleBuffer();
		}
		/** returns a pointer on the first particle with strict class checking */
		template<typename PARTICLE_TYPE>
		PARTICLE_TYPE const * GetParticleCheckedBuffer() const
		{
			assert(IsParticleClassCompatible<PARTICLE_TYPE>(false));
			return (PARTICLE_TYPE const*)GetParticleBuffer();
		}

		/** get the layer for this allocation */
		ParticleLayerBase * GetLayer() { return layer; }
		/** get the layer for this allocation */
		ParticleLayerBase const * GetLayer() const { return layer; }

		/** remove the allocation from its layer */
		void RemoveFromLayer();

	protected:

		/** tick the allocation (returns true whether the allocation is to be destroyed) */
		virtual bool TickAllocation(float delta_time, void const * layer_trait) { return false; }
		/** transforms the particles into vertices in the buffer */
		virtual size_t ParticlesToVertices(void * vertices, void const * layer_trait) const { return 0; }
		/** called whenever the allocation is removed from the layer */
		void OnRemovedFromLayer();
		/** require the layer to update the GPU buffer */
		void ConditionalRequireGPUUpdate(bool skip_if_invisible, bool skip_if_empty);

	protected:

		/** the particle layer that contains the range */
		ParticleLayerBase * layer = nullptr;
		/** whether the allocation is paused */
		bool paused = false;
		/** whether the allocation is visible */
		bool visible = true;
		/** a callback called whenever the allocation becomes empty */
		bool destroy_when_empty = false;
	};

	// ==============================================================
	// ParticleAllocation
	// ==============================================================

	template<typename ALLOCATION_TRAIT>
	class ParticleAllocation : public ParticleAllocationBase
	{
	public:

		using allocation_trait_type = ALLOCATION_TRAIT;
		using particle_type = typename allocation_trait_type::particle_type;
		using vertex_type = typename allocation_trait_type::vertex_type;
		using layer_trait_type = typename get_LayerTrait<allocation_trait_type>::type;

		/** constructor */
		ParticleAllocation(ParticleLayerBase * in_layer) : ParticleAllocationBase(in_layer) {}
		/** override */
		virtual ClassTools::ClassRegistration const * GetParticleClass() const override
		{
			return ClassTools::GetClassRegistration<particle_type>();
		}
		/** override */
		virtual size_t GetParticleCount() const override
		{
			return particles.size();
		}
		/** override */
		virtual size_t GetParticleSize() const override
		{
			return sizeof(particle_type);
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
			ConditionalRequireGPUUpdate(true, false);
			return true;
		}

	protected:

		/** override */
		virtual bool TickAllocation(float delta_time, void const * layer_trait) 
		{ 
			layer_trait_type const * typed_layer_trait = (layer_trait_type const *)layer_trait;

			bool destroy_allocation = TickAllocationTrait(
				delta_time, 
				typed_layer_trait, 
				has_function_Tick<allocation_trait_type>::type(), 
				has_LayerTrait<allocation_trait_type>::type());
			if (!destroy_allocation)
			{
				destroy_allocation = UpdateParticles(
					delta_time,
					typed_layer_trait,
					boost::mpl::true_() /*has_function_UpdateParticle<allocation_trait_type>::type()*/); // shuxxx FIXME : this template does not detect template function ! => see ParticleDefault that require a template
			}
			return destroy_allocation;
		}

		/** internal method to tick the AllocationTrait */
		template<typename T>
		bool TickAllocationTrait(float delta_time, layer_trait_type const * layer_trait, boost::mpl::false_ HAS_TICK, T HAS_LAYER_TRAIT)
		{
			return false; // do not destroy the allocation
		}

		bool TickAllocationTrait(float delta_time, layer_trait_type const * layer_trait, boost::mpl::true_ HAS_TICK, boost::mpl::false_ HAS_LAYER_TRAIT)
		{
			return allocation_trait.Tick(delta_time, this); // let the trait decide whether the allocation is to be destroyed
		}

		bool TickAllocationTrait(float delta_time, layer_trait_type const * layer_trait, boost::mpl::true_ HAS_TICK, boost::mpl::true_ HAS_LAYER_TRAIT)
		{
			return allocation_trait.Tick(delta_time, this, layer_trait); // let the trait decide whether the allocation is to be destroyed
		}

		/** internal methods to update the particles */
		bool UpdateParticles(float delta_time, layer_trait_type const * layer_trait, boost::mpl::false_ HAS_UPDATE_PARTICLE)
		{
			return false; // do not destroy the allocation
		}

		bool UpdateParticles(float delta_time, layer_trait_type const * layer_trait, boost::mpl::true_ HAS_UPDATE_PARTICLE)
		{
			size_t particle_count = GetParticleCount();

			size_t remaining_particles = DoUpdateParticles(
				delta_time, 
				(particle_type *)GetParticleBuffer(), 
				particle_count, 
				layer_trait, 
				has_function_BeginUpdateParticles<allocation_trait_type>::type(), 
				has_LayerTrait<allocation_trait_type>::type());
			if (remaining_particles == 0 && GetDestroyWhenEmpty())
				return true; // destroy allocation
			else if (remaining_particles != particle_count) // clean buffer of all particles that have been destroyed
				Resize(remaining_particles);
			return false; // do not destroy allocation
		}

		size_t DoUpdateParticles(float delta_time, particle_type * particles, size_t particle_count, layer_trait_type const * layer_trait, boost::mpl::false_ HAS_BEGIN_UPDATE_PARTICLES, boost::mpl::false_ HAS_LAYER_TRAIT)
		{
			return DoUpdateParticlesLoop(
				delta_time, 
				particles, 
				particle_count);
		}

		size_t DoUpdateParticles(float delta_time, particle_type * particles, size_t particle_count, layer_trait_type const * layer_trait, boost::mpl::false_ HAS_BEGIN_UPDATE_PARTICLES, boost::mpl::true_ HAS_LAYER_TRAIT)
		{
			return DoUpdateParticlesLoop(
				delta_time, 
				particles, 
				particle_count, 
				layer_trait);
		}

		size_t DoUpdateParticles(float delta_time, particle_type * particles, size_t particle_count, layer_trait_type const * layer_trait, boost::mpl::true_ HAS_BEGIN_UPDATE_PARTICLES, boost::mpl::false_ HAS_LAYER_TRAIT)
		{
			return DoUpdateParticlesLoop(
				delta_time, 
				particles, 
				particle_count, 
				allocation_trait.BeginUpdateParticles(delta_time, particles, particle_count)); // do not use a temp variable, so it can be a left-value reference
		}

		size_t DoUpdateParticles(float delta_time, particle_type * particles, size_t particle_count, layer_trait_type const * layer_trait, boost::mpl::true_ HAS_BEGIN_UPDATE_PARTICLES, boost::mpl::true_ HAS_LAYER_TRAIT)
		{
			return DoUpdateParticlesLoop(
				delta_time, 
				particles, 
				particle_count, 
				allocation_trait.BeginUpdateParticles(delta_time, particles, particle_count, layer_trait), // do not use a temp variable, so it can be a left-value reference
				layer_trait);
		}

		template<typename ...PARAMS>
		size_t DoUpdateParticlesLoop(float delta_time, particle_type * particles, size_t particle_count, PARAMS... params)
		{
			// tick all particles. overide all particles that have been destroyed by next on the array
			size_t j = 0;
			for (size_t i = 0; i < particle_count; ++i)
			{
				if (!allocation_trait.UpdateParticle(delta_time, &particles[i], params...)) // particle not destroyed ?
				{
					if (i != j)
						particles[j] = particles[i];
					++j;
				}
			}
			return j; // final number of particles
		}


		/** override */
		virtual size_t ParticlesToVertices(void * vertices, void const * layer_trait) const override
		{ 
			layer_trait_type const * typed_layer_trait = (layer_trait_type const *)layer_trait;

			return ParticlesToVertices(
				(particle_type*)GetParticleBuffer(), 
				GetParticleCount(), 
				(vertex_type*)vertices, 
				ParticleTraitTools::GetVerticesPerParticle(*typed_layer_trait), 
				typed_layer_trait, 
				boost::mpl::true_() /*has_function_ParticleToVertices<allocation_trait_type>::type()*/); //  shuxxx FIXME : this template does not detect template function => see ParticleDefault
		}

		size_t ParticlesToVertices(particle_type const * particles, size_t particle_count, vertex_type * vertices, size_t vertices_per_particle, layer_trait_type const * layer_trait, boost::mpl::false_ HAS_PARTICLE_TO_VERTICES) const 
		{
			return 0;		
		}

		size_t ParticlesToVertices(particle_type const * particles, size_t particle_count, vertex_type * vertices, size_t vertices_per_particle, layer_trait_type const * layer_trait, boost::mpl::true_ HAS_PARTICLE_TO_VERTICES) const 
		{ 
			return DoParticlesToVertices(
				particles, 
				particle_count, 
				vertices, 
				vertices_per_particle, 
				layer_trait, 
				has_function_BeginParticlesToVertices<allocation_trait_type>::type(), 
				has_LayerTrait<allocation_trait_type>::type());	
		}

		size_t DoParticlesToVertices(particle_type const * particles, size_t particle_count, vertex_type * vertices, size_t vertices_per_particle, layer_trait_type const * layer_trait, boost::mpl::false_ HAS_BEGIN_PARTICLES_TO_VERTICES, boost::mpl::false_ HAS_LAYER_TRAIT) const 
		{
			return DoParticlesToVerticesLoop(
				particles, 
				particle_count, 
				vertices, 
				vertices_per_particle);
		}

		size_t DoParticlesToVertices(particle_type const * particles, size_t particle_count, vertex_type * vertices, size_t vertices_per_particle, layer_trait_type const * layer_trait, boost::mpl::false_ HAS_BEGIN_PARTICLES_TO_VERTICES, boost::mpl::true_ HAS_LAYER_TRAIT) const 
		{
			return DoParticlesToVerticesLoop(
				particles, 
				particle_count, 
				vertices, 
				vertices_per_particle, 
				layer_trait);
		}

		size_t DoParticlesToVertices(particle_type const * particles, size_t particle_count, vertex_type * vertices, size_t vertices_per_particle, layer_trait_type const * layer_trait, boost::mpl::true_ HAS_BEGIN_PARTICLES_TO_VERTICES, boost::mpl::false_ HAS_LAYER_TRAIT) const 
		{
			return DoParticlesToVerticesLoop(
				particles, 
				particle_count, 
				vertices, 
				vertices_per_particle, 
				allocation_trait.BeginParticlesToVertices(particles, particle_count)); // do not use a temp variable, so it can be a left-value reference
		}

		size_t DoParticlesToVertices(particle_type const * particles, size_t particle_count, vertex_type * vertices, size_t vertices_per_particle, layer_trait_type const * layer_trait, boost::mpl::true_ HAS_BEGIN_PARTICLES_TO_VERTICES, boost::mpl::true_ HAS_LAYER_TRAIT) const 
		{
			return DoParticlesToVerticesLoop(
				particles, 
				particle_count, 
				vertices, 
				vertices_per_particle, 
				allocation_trait.BeginParticlesToVertices(particles, particle_count, layer_trait), 
				layer_trait); // do not use a temp variable, so it can be a left-value reference
		}

		template<typename ...PARAMS>
		size_t DoParticlesToVerticesLoop(particle_type const * particles, size_t particle_count, vertex_type * vertices, size_t vertices_per_particle, PARAMS... params) const
		{
			size_t result = 0;
			// transforms particles to vertices
			vertex_type * v = vertices;
			for (size_t i = 0; i < particle_count; ++i)
			{
				size_t new_vertices = allocation_trait.ParticleToVertices(&particles[i], v, vertices_per_particle, params...);
				assert(new_vertices <= vertices_per_particle);
				result += new_vertices;
				v += new_vertices;
			}
			return result;
		}

	protected:

		/** the particles buffer */
		std::vector<particle_type> particles;
		/** the trait of the allocation (some extra data + some functions) */
		allocation_trait_type allocation_trait;
	};

	// ==============================================================
	// ParticleLayerBase
	// ==============================================================

	class ParticleLayerBase : public Renderable
	{
		CHAOS_PARTICLE_ALL_FRIENDS

	public:

		/** constructor */
		ParticleLayerBase();
		/** destructor */
		virtual ~ParticleLayerBase();

		/** returns true whether the class required is compatible with the one store in the buffer */
		template<typename PARTICLE_TYPE>
		bool IsParticleClassCompatible(bool accept_bigger_particle) const
		{
			return ParticleTools::IsParticleClassCompatible<PARTICLE_TYPE>(GetParticleClass(), GetParticleSize(), accept_bigger_particle);
		}

		/** returns the number of particle count */
		size_t ComputeMaxParticleCount() const;

		/** returns the size in memory of a particle */
		virtual size_t GetParticleSize() const { return 0; }
		/** returns the size in memory of a vertex */
		virtual size_t GetVertexSize() const { return 0; }
		/** returns the number of vertices required for each particles */
		virtual size_t GetVerticesPerParticle() const { return 2 * 3; } // 2 triangles per particles to have a square = 6 vertices
		/** returns true whether vertices need to be updated */
		virtual bool AreVerticesDynamic() const { return true; }
		/** returns true whether particles need to be updated */
		virtual bool AreParticlesDynamic() const { return true; }

		/** get the particle ID for this system */
		virtual ClassTools::ClassRegistration const * GetParticleClass() const { return nullptr; }

		/** returns true whether the particle type is the one given as template parameter */
		template<typename T>
		bool IsParticleType() const
		{
			return (GetParticleClass() == ClassTools::GetClassRegistration<T>());
		}

		/** change the atlas */
		void SetTextureAtlas(BitmapAtlas::TextureArrayAtlas * in_atlas) { atlas = in_atlas; }
		/** change the material */
		void SetRenderMaterial(GPURenderMaterial * in_render_material) { render_material = in_render_material; }

		/** get the atlas const method */
		BitmapAtlas::TextureArrayAtlas const * GetTextureAtlas() const { return atlas.get(); }
		/** get the material const method */
		GPURenderMaterial const * GetRenderMaterial() const { return render_material.get(); }

		/** spawn a given number of particles */
		ParticleAllocationBase * SpawnParticles(size_t count);

		/** get the number of allocations */
		size_t GetAllocationCount() const;
		/** get the allocation by index */
		ParticleAllocationBase * GetAllocation(size_t index);
		/** get the allocation by index */
		ParticleAllocationBase const * GetAllocation(size_t index) const;

		/** get the vertex declaration */
		virtual GPUVertexDeclaration GetVertexDeclaration() const { return GPUVertexDeclaration(); }

	public:

		/** creation of an allocation */
		virtual ParticleAllocationBase * DoSpawnParticles(size_t count) { return nullptr; }
		/** returns the layer trait */
		virtual void * GetLayerTrait() { return nullptr; }
		/** returns the layer trait */
		virtual void const * GetLayerTrait() const { return nullptr; }

	protected:

		/** ticking the particle system */
		virtual bool DoTick(double delta_time) override;
		/** draw the layer */
		virtual int DoDisplay(Renderer * renderer, GPUProgramProviderBase const * uniform_provider, RenderParams const & render_params) const override;

		/** change the GL rendering state */
		void UpdateRenderingStates(Renderer * renderer, bool begin) const;

		/** unlink all particles allocations */
		void DetachAllParticleAllocations();
		/** internal method to remove a range from the layer */
		void RemoveParticleAllocation(ParticleAllocationBase * allocation);

		/** internal method to update the GPU buffers */
		size_t DoUpdateGPUBuffers(char * buffer, size_t vertex_buffer_size) const;

		/** update the vertex declaration */
		void UpdateVertexDeclaration() const;
		/** the effective rendering */
		int DoDisplayHelper(Renderer * renderer, size_t vcount, GPURenderMaterial const * final_material, GPUProgramProviderBase const * uniform_provider, RenderParams const & render_params) const;

		/** internal method to update particles (returns true whether there was real changes) */
		virtual bool TickAllocations(double delta_time);

		/** override */
		virtual bool DoUpdateGPUResources(Renderer * renderer) const override;

	protected:

		/** the texture atlas */
		shared_ptr<BitmapAtlas::TextureArrayAtlas> atlas;

		/** whether there was changes in particles, and a vertex array need to be recomputed */
		mutable bool require_GPU_update = false;

		/** the material used to render the layer */
		shared_ptr<GPURenderMaterial> render_material;

		/** particles allocations */
		std::vector<shared_ptr<ParticleAllocationBase>> particles_allocations;

		/** the vertex declaration */
		mutable GPUVertexDeclaration vertex_declaration;
		/** the vertex buffer for the rendering */
		mutable shared_ptr<GPUVertexBuffer> vertex_buffer;
		/** the cache for vertex array */
		mutable GPUVertexArrayCache vertex_array_cache;

		/** number of used vertices in the vertex buffer */
		mutable size_t vertices_count = 0;
	};

	// ==============================================================
	// ParticleLayer
	// ==============================================================

	template<typename ALLOCATION_TRAIT>
	class ParticleLayer : public ParticleLayerBase
	{

	public:

		using allocation_trait_type = ALLOCATION_TRAIT;
		using particle_type = typename allocation_trait_type::particle_type;
		using vertex_type = typename allocation_trait_type::vertex_type;
		using layer_trait_type = typename get_LayerTrait<allocation_trait_type>::type;

		/** constructor */
		ParticleLayer(layer_trait_type in_layer_trait = layer_trait_type()):
			layer_trait(in_layer_trait){}

		/** returns the size in memory of a particle */
		virtual size_t GetParticleSize() const override { return sizeof(particle_type); }
		/** override */
		virtual size_t GetVertexSize() const override { return sizeof(vertex_type); }
		/** override */
		virtual size_t GetVerticesPerParticle() const override { return ParticleTraitTools::GetVerticesPerParticle(layer_trait); }
		/** override */
		virtual bool AreVerticesDynamic() const override 
		{ 
			// read the layer property 'dynamic_vertices' if any
			if (!ParticleTraitTools::AreVerticesDynamic(layer_trait))
				return false;
			// read a static allocation value
			if (!ParticleTraitTools::AreVerticesDynamicStatic<allocation_trait_type>()) // AreVerticesDynamic() is used to manage all allocations of the whole layer => we cannot afford to read one instance value
				return false;
			return true;
		}
		/** override */
		virtual bool AreParticlesDynamic() const override 
		{ 
			// read the layer property 'dynamic_vertices' if any
			if (!ParticleTraitTools::AreParticlesDynamic(layer_trait))
				return false;
			// read a static allocation value
			if (!ParticleTraitTools::AreParticlesDynamicStatic<allocation_trait_type>())  // AreParticlesDynamic() is used to manage all allocations of the whole layer => we cannot afford to read one instance value
				return false;
			return true;
		}
		/** override */
		virtual ClassTools::ClassRegistration const * GetParticleClass() const override { return ClassTools::GetClassRegistration<particle_type>(); }
		/** override */
		virtual GPUVertexDeclaration GetVertexDeclaration() const { return GetTypedVertexDeclaration(boost::mpl::identity<vertex_type>()); }

	protected:

		/** override */
		virtual ParticleAllocationBase * DoSpawnParticles(size_t count) override { return new ParticleAllocation<allocation_trait_type>(this); }
		/** override */
		virtual void * GetLayerTrait() { return &layer_trait; }
		/** override */
		virtual void const * GetLayerTrait() const { return &layer_trait; }

	protected:

		/** the trait of the layer */
		layer_trait_type layer_trait;
	};

	// ==============================================================
	// ParticleAllocationTrait
	// ==============================================================

	template<typename PARTICLE_TYPE, typename VERTEX_TYPE, bool DYNAMIC_PARTICLES = true, bool DYNAMIC_VERTICES = true>
	class ParticleAllocationTrait
	{
	public:

		/** the type for one particle */
		using particle_type = PARTICLE_TYPE;
		/** the type for one vertex */
		using vertex_type = VERTEX_TYPE;

		size_t ParticleToVertices(PARTICLE_TYPE const * particle, VERTEX_TYPE * vertices, size_t vertices_per_particle) const
		{
			return 0; // default implementation
		}

		bool UpdateParticle(float delta_time, PARTICLE_TYPE * particle) const
		{
			return false; // default implementation => do not destroy particle
		}

		/** whether the particles are dynamic */
		static bool const dynamic_particles = DYNAMIC_PARTICLES;
		/** whether the vertices are dynamic */
		static bool const dynamic_vertices = DYNAMIC_VERTICES;
	};









	// ==============================================================
	// PARTICLE LAYER DESC
	// ==============================================================

	// ParticleLayerDesc : this is class that deserves to implement how 
	//
	//  - Particles are allocated (create an ParticleAllocation subclass instance corresponding to the read structure for each particles)
	//  - Particles are updated
	//  - Particles are transformed into (6?) vertices fo rendering (a quad?)
	//
	// the main functions to override are:
	//
	//  - NewAllocation(...)
	//  - ParticlesToVertices(...)
	//  - UpdateParticles(...)



	// ==============================================================
	// TypedParticleLayerDesc
	// ==============================================================

	// TypedParticleLayerDesc : 
	//    
	//   this a ParticleLayerDesc derived class. Instead of implementing by hand such a new class for each PARTICLE type, 
	//   we use this template class that use a LAYER_TRAIT argument
	//
	//   => the LAYER_TRAIT implement the very low level necessary functions and 'TypedParticleLayerDesc' helps making the glue to have a valid 'ParticleLayerDesc' class
	//   => PRO: we avoid virtual function calls for each particles, but only a single time for every 'Allocation'
	//
	// requirement of LAYER_TRAIT:
	//
	// 1 - it must implement:
	//
	//    bool UpdateParticle(...) const;    => returns true if the particle must be destroyed
	//
	// and 
	//
	//    size_t ParticleToVertices(...) const;  => returns the number of vertices written (should be 6)
	//
	// 2 - If we have to make a computation on ALL particles before, we can implement the two following functions
	//
	//	  TYPE_XXX BeginUpdateParticles(...)
	//
	//	  TYPE_YYY BeginParticlesToVertices(...)
	//
	// in that case, the previous functions have an additionnal argument
	//
	//    UpdateParticle(... TYPE_XXX)
	//
	//    ParticleToVertices(... TYPE_YYY)
	//
	// Example : we can compute an transform for the whole allocation (single call) and apply it to each particle
	//
	// 3 - if we want each allocation to have embedded data, your trait just has to have an embedded data
	//
	//    class per_allocation_data { ... }
	//
	// in that case, the previous functions have an additionnal argument
	//
	//    UpdateParticle(... per_allocation_data, TYPE_XXX) 
	//
	//    ParticleToVertices(... per_allocation_data, TYPE_YYY)
	//
	//	  TYPE_XXX BeginUpdateParticles(...per_allocation_data)
	//
	//	  TYPE_YYY BeginParticlesToVertices(...per_allocation_data)
	//
	// This per_allocation_data may have :
	//
	//   - a Tick(delta_time) method => returns true if the allocation must be destroyed
	//
	//   - UpdateVertice(vertex *) method



	// undefine macros
	//#undef CHAOS_PARTICLE_CLASSES
	//#undef CHAOS_PARTICLE_FORWARD_DECL
	//#undef CHAOS_PARTICLE_FRIEND_DECL
	//#undef CHAOS_PARTICLE_ALL_FRIENDS

}; // namespace chaos

