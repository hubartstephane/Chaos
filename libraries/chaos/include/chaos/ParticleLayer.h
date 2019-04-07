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



		BOOST_DECLARE_HAS_MEMBER(has_dynamic_particles, dynamic_particles);
	BOOST_DECLARE_HAS_MEMBER(has_dynamic_vertices, dynamic_vertices);
	BOOST_DECLARE_HAS_MEMBER(has_vertices_per_particle, vertices_per_particle);
	CHAOS_GENERATE_HAS_FUNCTION_METACLASS(Tick)
		CHAOS_GENERATE_HAS_FUNCTION_METACLASS(UpdateParticle)
		CHAOS_GENERATE_HAS_FUNCTION_METACLASS(BeginUpdateParticles)
		CHAOS_GENERATE_HAS_FUNCTION_METACLASS(BeginParticlesToVertices)


		// ==============================================================
		// ParticleAllocationBase
		// ==============================================================

		class ParticleAllocationBase : public Tickable
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

	protected:

		/** transforms the particles into vertices in the buffer */
		virtual size_t ParticlesToVertices(void * vertices) const { return 0; }
		/** remove the allocation from its layer */
		void RemoveFromLayer();
		/** called whenever the allocation is removed from the layer */
		void OnRemovedFromLayer();
		/** require the layer to update the GPU buffer */
		void ConditionalRequireGPUUpdate(bool skip_if_invisible, bool skip_if_empty);

	protected:

		/** the particle layer that contains the range */
		ParticleLayerBase * layer = nullptr;
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

		using allocation_trait = ALLOCATION_TRAIT;
		using particle_type = typename allocation_trait::particle_type;
		using vertex_type = typename allocation_trait::vertex_type;

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
		virtual bool DoTick(double delta_time)
		{
			bool destroy_allocation = false;

			if (TickTrait(delta_time, has_function_Tick<allocation_trait>::type()))
			{
				destroy_allocation = true;
				return true;
			}
			destroy_allocation = UpdateParticles(delta_time, has_function_UpdateParticle<allocation_trait>::type());
			return true;
		}

		bool TickTrait(double delta_time, boost::mpl::false_ HAS_TICK)
		{
			return false; // do not destroy the allocation
		}

		bool TickTrait(double delta_time, boost::mpl::true_ HAS_TICK)
		{
			return trait.Tick(delta_time); // let the trait decide whether the allocation is to be destroyed
		}

		bool UpdateParticles(double delta_time, boost::mpl::false_ HAS_UPDATE_PARTICLE)
		{
			return false; // do not destroy the allocation
		}

		bool UpdateParticles(double delta_time, boost::mpl::true_ HAS_UPDATE_PARTICLE)
		{
			bool destroy_allocation = false;

			size_t remaining_particles = DoUpdateParticles(delta_time, (particle_type *)GetParticleBuffer(), GetParticleCount(), has_function_BeginUpdateParticle<allocation_trait>::type());
			if (remaining_particles == 0 && GetDestroyWhenEmpty())
				destroy_allocation = true;
			else if (remaining_particles != particle_count) // clean buffer of all particles that have been destroyed
				allocation->Resize(remaining_particles);

			return destroy_allocation;
		}

		size_t DoUpdateParticles(float delta_time, particle_type * particles, size_t particle_count, boost::mpl::false_ HAS_BEGIN_UPDATE_PARTICLES)
		{
			// No BeginUpdateParticles(...) call

			// tick all particles. overide all particles that have been destroyed by next on the array
			size_t j = 0;
			for (size_t i = 0; i < particle_count; ++i)
			{
				if (!trait.UpdateParticle(delta_time, &particles[i])) // particle not destroyed ?
				{
					if (i != j)
						particles[j] = particles[i];
					++j;
				}
			}
			return j; // final number of particles
		}

		size_t DoUpdateParticles(float delta_time, particle_type * particles, size_t particle_count, ParticleAllocation * allocation, boost::mpl::true_ HAS_BEGIN_UPDATE_PARTICLES)
		{
			// => the extra call !!!
			auto extra_param = trait.BeginUpdateParticles(delta_time, particles, particle_count);

			// tick all particles. overide all particles that have been destroyed by the next on the array
			size_t j = 0;
			for (size_t i = 0; i < particle_count; ++i)
			{
				if (!trait.UpdateParticle(delta_time, &particles[i], extra_param)) // particle not destroyed ?
				{
					if (i != j)
						particles[j] = particles[i];
					++j;
				}
			}
			return j; // final number of particles
		}


		/** override */
		virtual size_t ParticlesToVertices(void * vertices) const override
		{
			return DoParticlesToVertices((particle_type*)GetParticleBuffer(), GetParticleCount(), (vertex_type*)vertices, GetVerticesPerParticle(), has_function_BeginParticlesToVertices<allocation_trait>::type());
		}

		size_t DoParticlesToVertices(particle_type const * particles, size_t particles_count, vertex_type * vertices, size_t vertices_per_particle, boost::mpl::false_ HAS_BEGIN_PARTICLES_TO_VERTICES) const
		{
			size_t result = 0;

			// No BeginParticlesToVertices(...) call

			// transforms particles to vertices
			vertex_type * v = vertices;
			for (size_t i = 0; i < particles_count; ++i)
			{
				size_t new_vertices = trait.ParticleToVertice(&particles[i], v, vertices_per_particle);
				assert(new_vertices <= vertices_per_particle);
				result += new_vertices;
				v += new_vertices;
			}
			return result;
		}

		size_t DoParticlesToVertices(particle_type const * particles, size_t particles_count, vertex_type * vertices, size_t vertices_per_particle, boost::mpl::true_ HAS_BEGIN_PARTICLES_TO_VERTICES) const
		{
			size_t result = 0;

			// the extra call !
			auto extra_param = trait.BeginParticlesToVertices(particles, particles_count);

			// transforms particles to vertices
			vertex_type * v = vertices;
			for (size_t i = 0; i < particles_count; ++i)
			{
				size_t new_vertices = trait.ParticleToVertice(&particles[i], v, vertices_per_particle, extra_param);
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
		allocation_trait trait;
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
		virtual size_t GetVerticesPerParticles() const { return 2 * 3; } // 2 triangles per particles to have a square = 6 vertices
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

	protected:

		/** ticking the particle system */
		virtual bool DoTick(double delta_time) override;
		/** draw the layer */
		virtual int DoDisplay(Renderer * renderer, GPUProgramProviderBase const * uniform_provider, RenderParams const & render_params) const override;

		/** change the GL rendering state */
		void UpdateRenderingStates(bool begin) const;

		/** unlink all particles allocations */
		void DetachAllParticleAllocations();
		/** internal method to remove a range from the layer */
		void RemoveParticleAllocation(ParticleAllocationBase * allocation);

		/** internal method to update the GPU buffers */
		size_t DoUpdateGPUBuffers(char * buffer, size_t vertex_buffer_size) const;

		/** update the vertex declaration */
		void UpdateVertexDeclaration() const;
		/** the effective rendering */
		int DoDisplayHelper(Renderer * renderer, size_t vcount, GPURenderMaterial const * final_material, GPUProgramProviderBase const * uniform_provider, InstancingInfo const & instancing) const;

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

		using allocation_trait = ALLOCATION_TRAIT;
		using particle_type = typename allocation_trait::particle_type;
		using vertex_type = typename allocation_trait::vertex_type;

		/** returns the size in memory of a particle */
		virtual size_t GetParticleSize() const override { return sizeof(particle_type); }
		/** override */
		virtual size_t GetVertexSize() const override { return sizeof(vertex_type); }
		/** override */
		virtual size_t GetVerticesPerParticles() const override { return DoGetVerticesPerParticles(has_vertices_per_particle<allocation_trait>::type()); }
		/** override */
		virtual bool AreVerticesDynamic() const override { return DoAreVerticesDynamic(has_dynamic_vertices<allocation_trait>::type()); }
		/** override */
		virtual bool AreParticlesDynamic() const override { return DoAreParticlesDynamic(has_dynamic_particles<allocation_trait>::type()); }
		/** override */
		virtual ClassTools::ClassRegistration const * GetParticleClass() const override { return ClassTools::GetClassRegistration<particle_type>(); }
		/** override */
		virtual GPUVertexDeclaration GetVertexDeclaration() const { return GetTypedVertexDeclaration(boost::mpl::identity<vertex_type>()); }

	protected:

		/** override */
		virtual ParticleAllocationBase * DoSpawnParticles(size_t count) override { return new ParticleAllocation<allocation_trait>(this); }

	protected:

		bool DoAreVerticesDynamic(boost::mpl::false_ HAS_DYNAMIC_VERTICES) const { return true; } // default value

		bool DoAreVerticesDynamic(boost::mpl::true_ HAS_DYNAMIC_VERTICES) const { return allocation_trait.dynamic_vertices; }

		bool DoAreParticlesDynamic(boost::mpl::false_ HAS_DYNAMIC_PARTICLES) const { return true; } // default value

		bool DoAreParticlesDynamic(boost::mpl::true_ HAS_DYNAMIC_PARTICLES) const { return allocation_trait.dynamic_particles; }

		bool DoGetVerticesPerParticles(boost::mpl::false_ HAS_VERTICES_PER_PARTICLE) const { return 2 * 3; } // default value

		bool DoGetVerticesPerParticles(boost::mpl::true_ HAS_VERTICES_PER_PARTICLE) const { return allocation_trait.vertices_per_particle; }


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

