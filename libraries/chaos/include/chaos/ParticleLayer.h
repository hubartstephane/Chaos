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
#define CHAOS_PARTICLE_CLASSES (ParticleAllocation) (ParticleLayer) (ParticleManager) (ParticleLayerDesc) (ParticleAllocationEmptyCallback) (ParticleAllocationAutoRemoveEmptyCallback)

	// forward declaration
#define CHAOS_PARTICLE_FORWARD_DECL(r, data, elem) class elem;
	BOOST_PP_SEQ_FOR_EACH(CHAOS_PARTICLE_FORWARD_DECL, _, CHAOS_PARTICLE_CLASSES)

		// friendship macro
#define CHAOS_PARTICLE_FRIEND_DECL(r, data, elem) friend class elem;
#define CHAOS_PARTICLE_ALL_FRIENDS BOOST_PP_SEQ_FOR_EACH(CHAOS_PARTICLE_FRIEND_DECL, _, CHAOS_PARTICLE_CLASSES)

		// ==============================================================
		// ParticleAccessorBase
		// ==============================================================

	template<typename PARTICLE_TYPE, typename BUFFER_TYPE>
	class ParticleAccessorBase
	{
	public:

		/** default constructor */
		ParticleAccessorBase(){}

		/** constructor */
		ParticleAccessorBase(BUFFER_TYPE * in_buffer, size_t in_count, size_t in_particle_size) :
			buffer(in_buffer), count(in_count), particle_size(in_particle_size)
		{
			assert((count > 0) ^ (buffer == nullptr));				
		}
		/** pointer indirection */
		PARTICLE_TYPE & operator * () const
		{
			assert(count > 0);
			return *((PARTICLE_TYPE*)buffer);
		}
		/** pointer inference */
		PARTICLE_TYPE * operator -> () const
		{
			if (count == 0)
				return nullptr;
			return ((PARTICLE_TYPE*)buffer);
		}
		/** array accessor */
		PARTICLE_TYPE & operator [](size_t index) const
		{
			assert(index < count);
			char * b = (char*)buffer;
			return *((PARTICLE_TYPE*)(b + index * particle_size));
		}

		/** get the number of particle */
		size_t GetCount() const 
		{
			return count;
		}
		/** get the particle size */
		size_t GetParticleSize() const
		{
			return particle_size;
		}

	protected:

		/** the start of the buffer */
		BUFFER_TYPE * buffer = nullptr;
		/** the number of particles in that buffer */
		size_t count = 0;
		/** the real particle size (not PARTICLE_TYPE) */
		size_t particle_size = 0;
	};

	// ==============================================================
	// ParticleAccessor / ParticleConstAccessor
	// ==============================================================

	template<typename PARTICLE_TYPE>
	class ParticleConstAccessor : public ParticleAccessorBase<PARTICLE_TYPE const, void const>
	{
	public:

		/** default constructor */
		ParticleConstAccessor() {}
		/** constructor */
		ParticleConstAccessor(void const * in_buffer, size_t in_count, size_t in_particle_size) :
			ParticleAccessorBase<PARTICLE_TYPE const, void const>(in_buffer, in_count, in_particle_size)
		{
		}
	};

	template<typename PARTICLE_TYPE>
	class ParticleAccessor : public ParticleAccessorBase<PARTICLE_TYPE, void>
	{
	public:

		/** default constructor */
		ParticleAccessor() {}
		/** constructor */
		ParticleAccessor(void * in_buffer, size_t in_count, size_t in_particle_size) :
			ParticleAccessorBase<PARTICLE_TYPE, void>(in_buffer, in_count, in_particle_size)
		{
		}
		/** convert into a const accessor */
		operator ParticleConstAccessor<PARTICLE_TYPE>() const
		{
			return ParticleConstAccessor<PARTICLE_TYPE>(buffer, count, particle_size);
		}
	};

	// ==============================================================
	// ParticleAllocationEmptyCallback : a callback called whenever the allocation becomes empty for the very first time
	// ==============================================================

	class ParticleAllocationEmptyCallback : public ReferencedObject
	{
		CHAOS_PARTICLE_ALL_FRIENDS

	public:

		/** destructor */
		virtual ~ParticleAllocationEmptyCallback() = default;

		/** called whenever the allocation becomes empty */
		virtual bool OnAllocationEmpty(ParticleAllocation * allocation);
	};

	// ==============================================================
	// ParticleAllocationAutoRemoveEmptyCallback : remove the allocation from its layer
	// ==============================================================

	class ParticleAllocationAutoRemoveEmptyCallback : public ParticleAllocationEmptyCallback
	{
	public:

		/** override */
		virtual bool OnAllocationEmpty(ParticleAllocation * allocation) override;
	};

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

		/** Decrement the reference count */
		virtual void SubReference() override;

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
		virtual size_t GetParticleSize() const;
		/** get the number of particles */
		virtual size_t GetParticleCount() const;
		/** get the particles */
		virtual void * GetParticleBuffer();
		/** get the particles */
		virtual void const * GetParticleBuffer() const;
		/** resize the particles */
		virtual bool Resize(size_t new_count);

		/** increment number of particles */
		bool AddParticles(size_t extra_count);

		/** set the empty callback */
		void SetEmptyCallback(ParticleAllocationEmptyCallback * in_empty_callback) { empty_callback = in_empty_callback; }

		/** set empty callback to auto remove */
		void SetEmptyCallbackAutoRemove();

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
		ParticleLayer * GetLayer() { return layer; }
		/** get the layer for this allocation */
		ParticleLayer const * GetLayer() const { return layer; }

	protected:

		/** remove the allocation from its layer */
		void RemoveFromLayer();
		/** called whenever the allocation is removed from the layer */
		void OnRemovedFromLayer();
		/** require the layer to update the GPU buffer */
		void ConditionalRequireGPUUpdate(bool skip_if_invisible, bool skip_if_empty);

	protected:

		/** the particle layer that contains the range */
		ParticleLayer * layer = nullptr;
		/** whether the allocation is paused */
		bool paused = false;
		/** whether the allocation is visible */
		bool visible = true;
		/** a callback called whenever the allocation becomes empty */
		shared_ptr<ParticleAllocationEmptyCallback> empty_callback;
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

		}
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

		/** the particles buffer */
		std::vector<particle_type> particles;
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

		/** returns true whether vertices need to be updated */
		virtual bool AreVerticesDynamic() const;
		/** returns true whether particles need to be updated */
		virtual bool AreParticlesDynamic() const;

		/** get the vertex declaration for that layer */
		virtual GPUVertexDeclaration GetVertexDeclaration() const;

		/** returns the class ID for spawn particles */
		virtual ClassTools::ClassRegistration const * GetParticleClass() const { return nullptr; }

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
	// it must implement:
	//
	//    bool UpdateParticle(...) const;    => returns true if the particle must be destroyed
	//
	// and 
	//
	//    size_t ParticleToVertices(...) const;  => returns the number of vertices written (should be 6)
	//
	// If we have to make a computation on ALL particles before, we can implement the two following functions
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

	CHAOS_GENERATE_HAS_FUNCTION_METACLASS(BeginUpdateParticles)
	CHAOS_GENERATE_HAS_FUNCTION_METACLASS(BeginParticlesToVertices)
	
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
		virtual bool AreVerticesDynamic() const
		{
			return trait.AreVerticesDynamic();
		}
		/** override */
		virtual bool AreParticlesDynamic() const
		{
			return trait.AreParticlesDynamic();
		}
		/** override */
		virtual GPUVertexDeclaration GetVertexDeclaration() const override
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
			return DoUpdateParticles(delta_time, p, particle_count, allocation, HasFunction_BeginUpdateParticles<trait_type>::type());
		}

		/** override */
		virtual size_t ParticlesToVertices(void const * particles, size_t particles_count, char * vertices, ParticleAllocation * allocation) const override
		{
			particle_type const * p = (particle_type const *)particles;
			vertex_type * v = (vertex_type *)vertices;

			size_t vertices_per_particle = GetVerticesCountPerParticles();
			return DoParticlesToVertices(p, particles_count, v, vertices_per_particle, allocation, HasFunction_BeginParticlesToVertices<trait_type>::type());
		}

		/** override */
		virtual ClassTools::ClassRegistration const * GetParticleClass() const override
		{
			return trait.GetParticleClass();
		}

	protected:

		size_t DoUpdateParticles(float delta_time, particle_type * particles, size_t particle_count, ParticleAllocation * allocation, boost::mpl::true_)
		{
			auto extra_param = trait.BeginUpdateParticles(delta_time, particles, particle_count, allocation);

			// tick all particles. overide all particles that have been destroyed by next on the array
			size_t j = 0;
			for (size_t i = 0; i < particle_count; ++i)
			{
				if (!trait.UpdateParticle(delta_time, &particles[i], allocation, extra_param)) // particle not destroyed ?
				{
					if (i != j)
						particles[j] = particles[i];
					++j;
				}
			}
			return j; // final number of particles
		}

		size_t DoUpdateParticles(float delta_time, particle_type * particles, size_t particle_count, ParticleAllocation * allocation, boost::mpl::false_)
		{
			// tick all particles. overide all particles that have been destroyed by next on the array
			size_t j = 0;
			for (size_t i = 0; i < particle_count; ++i)
			{
				if (!trait.UpdateParticle(delta_time, &particles[i], allocation)) // particle not destroyed ?
				{
					if (i != j)
						particles[j] = particles[i];
					++j;
				}
			}
			return j; // final number of particles
		}

		size_t DoParticlesToVertices(particle_type const * particles, size_t particles_count, vertex_type * vertices, size_t vertices_per_particle, ParticleAllocation * allocation, boost::mpl::false_) const
		{
			size_t result = 0;
			for (size_t i = 0; i < particles_count; ++i)
			{
				size_t new_vertices = trait.ParticleToVertices(&particles[i], vertices, vertices_per_particle, allocation);
				assert(new_vertices <= vertices_per_particle);
				result += new_vertices;
				vertices += new_vertices;
			}
			return result;
		}

		size_t DoParticlesToVertices(particle_type const * particles, size_t particles_count, vertex_type * vertices, size_t vertices_per_particle, ParticleAllocation * allocation, boost::mpl::true_) const
		{
			auto extra_param = trait.BeginParticlesToVertices(particles, particles_count, allocation);

			size_t result = 0;
			for (size_t i = 0; i < particles_count; ++i)
			{
				size_t new_vertices = trait.ParticleToVertices(&particles[i], vertices, vertices_per_particle, allocation, extra_param);
				assert(new_vertices <= vertices_per_particle);
				result += new_vertices;
				vertices += new_vertices;
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

	class ParticleLayer : public Renderable
	{
		CHAOS_PARTICLE_ALL_FRIENDS

	public:

		/** constructor */
		ParticleLayer(ParticleLayerDesc * in_layer_desc);
		/** destructor */
		virtual ~ParticleLayer();

		/** returns true whether the class required is compatible with the one store in the buffer */
		template<typename PARTICLE_TYPE>
		bool IsParticleClassCompatible(bool accept_bigger_particle) const
		{
			return ParticleTools::IsParticleClassCompatible<PARTICLE_TYPE>(GetParticleClass(), GetParticleSize(), accept_bigger_particle);
		}

		/** returns the number of particle count */
		size_t ComputeMaxParticleCount() const;
		/** returns the size in memory of a particle */
		size_t GetParticleSize() const { return layer_desc->GetParticleSize(); }
		/** returns the size in memory of a vertex */
		size_t GetVertexSize() const { return layer_desc->GetVertexSize(); }
		/** returns the number of vertices required for each particles */
		size_t GetVerticesCountPerParticles() const { return layer_desc->GetVerticesCountPerParticles(); }
		/** returns true whether vertices need to be updated */
		bool AreVerticesDynamic() const { return layer_desc->AreVerticesDynamic(); }
		/** returns true whether particles need to be updated */
		bool AreParticlesDynamic() const { return layer_desc->AreParticlesDynamic(); }

		/** get the particle ID for this system */
		ClassTools::ClassRegistration const * GetParticleClass() const;

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
		ParticleAllocation * SpawnParticles(size_t count);

		/** get the number of allocations */
		size_t GetAllocationCount() const;
		/** get the allocation by index */
		ParticleAllocation * GetAllocation(size_t index);
		/** get the allocation by index */
		ParticleAllocation const * GetAllocation(size_t index) const;

	public:

		/** templated method to create a layer and set some values */
		template<typename TRAIT_TYPE, typename ...PARAMS>
		static ParticleLayer * CreateParticleLayer(char const * material_name, PARAMS... params)
		{
			ParticleLayerDesc * layer_desc = new TypedParticleLayerDesc<TRAIT_TYPE>(params...);
			if (layer_desc == nullptr)
				return nullptr;
			return CreateParticleLayer(layer_desc, material_name);
		}

		/** templated method to create a layer and set some values */
		template<typename TRAIT_TYPE, typename ...PARAMS>
		static ParticleLayer * CreateParticleLayer(GPURenderMaterial * render_material, PARAMS... params)
		{
			ParticleLayerDesc * layer_desc = new TypedParticleLayerDesc<TRAIT_TYPE>(params...);
			if (layer_desc == nullptr)
				return nullptr;
			return CreateParticleLayer(layer_desc, render_material);
		}

		/** create particle layer with a given material */
		static ParticleLayer * CreateParticleLayer(ParticleLayerDesc * layer_desc, GPURenderMaterial * render_material);
		/** create particle layer with a given material */
		static ParticleLayer * CreateParticleLayer(ParticleLayerDesc * layer_desc, char const * material_name);
		
	protected:

		/** ticking the particle system */
		virtual bool DoTick(double delta_time) override;
		/** draw the layer */
		virtual int DoDisplay(GPUProgramProviderBase const * uniform_provider, RenderParams const & render_params) const override;

		/** change the GL rendering state */
		void UpdateRenderingStates(bool begin) const;

		/** unlink all particles allocations */
		void DetachAllParticleAllocations();
		/** internal method to remove a range from the layer */
		void RemoveParticleAllocation(ParticleAllocation * allocation);

		/** internal method to update the GPU buffers */
		size_t DoUpdateGPUBuffers(char * buffer, size_t vertex_buffer_size) const;

		/** update the vertex declaration */
		void UpdateVertexDeclaration() const;
		/** the effective rendering */
		int DoDisplayHelper(size_t vcount, GPURenderMaterial const * final_material, GPUProgramProviderBase const * uniform_provider, InstancingInfo const & instancing) const;

		/** internal method to update particles (returns true whether there was real changes) */
		virtual bool UpdateParticles(float delta_time);

		/** override */
		virtual bool DoUpdateGPUResources() const override;

	protected:

		/** the texture atlas */
		shared_ptr<BitmapAtlas::TextureArrayAtlas> atlas;

		/** whether there was changes in particles, and a vertex array need to be recomputed */
		mutable bool require_GPU_update = false;

		/** the material used to render the layer */
		shared_ptr<GPURenderMaterial> render_material;

		/** particles allocations */
		std::vector<shared_ptr<ParticleAllocation>> particles_allocations;
		/** the behavior description */
		shared_ptr<ParticleLayerDesc> layer_desc;

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
	// ParticleLayerTrait
	// ==============================================================

	template<typename PARTICLE_TYPE, typename VERTEX_TYPE, bool DYNAMIC_PARTICLES = true, bool DYNAMIC_VERTICES = true>
	class ParticleLayerTrait
	{
	public:

		/** the type for one particle */
		using particle_type = PARTICLE_TYPE;
		/** the type for one vertex */
		using vertex_type = VERTEX_TYPE;

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

		/** returns the class ID for spawn particles */
		ClassTools::ClassRegistration const * GetParticleClass() const
		{
			return ClassTools::GetClassRegistration<particle_type>();
		}

	public:

		/** getters : can be override without risk even if not virtual because TRAIT use strong types */
		bool AreVerticesDynamic() const { return dynamic_vertices;}
		/** getters : can be override without risk even if not virtual because TRAIT use strong types */
		bool AreParticlesDynamic() const { return dynamic_particles;}

	public:

		/** whether the particles are dynamic */
		bool dynamic_particles = DYNAMIC_PARTICLES;
		/** whether the vertices are dynamic */
		bool dynamic_vertices = DYNAMIC_VERTICES;
	};

	// undefine macros
//#undef CHAOS_PARTICLE_CLASSES
//#undef CHAOS_PARTICLE_FORWARD_DECL
//#undef CHAOS_PARTICLE_FRIEND_DECL
//#undef CHAOS_PARTICLE_ALL_FRIENDS

}; // namespace chaos

