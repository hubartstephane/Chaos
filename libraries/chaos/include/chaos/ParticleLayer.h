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
#define CHAOS_PARTICLE_CLASSES (ParticleAllocationBase) (ParticleLayerBase) (ParticleManager) (ParticleLayerDesc)

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
		virtual size_t ParticlesToVertices(void * vertices, size_t vertices_per_particle) const { return 0; }
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
		using particle_type = allocation_trait::particle_type;
		using vertex_type = allocation_trait::vertex_type;

		/** constructor */
		ParticleAllocation(ParticleLayer * in_layer){}
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
		virtual size_t ParticlesToVertices(void * vertices, size_t vertices_per_particle) const override
		{
			return DoParticlesToVertices((particle_type*)GetParticleBuffer(), GetParticleCount(), (vertex_type*)vertices, vertices_per_particle, has_function_BeginParticlesToVertices<allocation_trait>::type());
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
	virtual size_t GetVerticesPerParticles() const { return 6; } // 2 triangles per particles to have a square = 6 vertices
	/** returns true whether vertices need to be updated */
	virtual bool AreVerticesDynamic() const { return true; }
	/** returns true whether particles need to be updated */
	virtual bool AreParticlesDynamic() const { return true; }

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
	ParticleAllocationBase * SpawnParticles(size_t count);

	/** get the number of allocations */
	size_t GetAllocationCount() const;
	/** get the allocation by index */
	ParticleAllocationBase * GetAllocation(size_t index);
	/** get the allocation by index */
	ParticleAllocationBase const * GetAllocation(size_t index) const;

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
	static ParticleAllocationBase * CreateParticleLayer(ParticleLayerDesc * layer_desc, GPURenderMaterial * render_material);
	/** create particle layer with a given material */
	static ParticleAllocationBase * CreateParticleLayer(ParticleLayerDesc * layer_desc, char const * material_name);

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
	virtual bool UpdateParticles(float delta_time);

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
	using particle_type = allocation_trait::particle_type;
	using vertex_type = allocation_trait::vertex_type;

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

protected:

	bool DoAreVerticesDynamic(boost::mpl::false_ HAS_DYNAMIC_VERTICES) const { return true; } // default value

	bool DoAreVerticesDynamic(boost::mpl::true_ HAS_DYNAMIC_VERTICES) const { return allocation_trait.dynamic_vertices; }

	bool DoAreParticlesDynamic(boost::mpl::false_ HAS_DYNAMIC_PARTICLES) const { return true; } // default value

	bool DoAreParticlesDynamic(boost::mpl::true_ HAS_DYNAMIC_PARTICLES) const { return allocation_trait.dynamic_particles; }

	bool DoGetVerticesPerParticles(boost::mpl::false_ HAS_VERTICES_PER_PARTICLE) const { return 6; } // default value

	bool DoGetVerticesPerParticles(boost::mpl::true_ HAS_VERTICES_PER_PARTICLE) const { return allocation_trait.vertices_per_particle; }


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
		size_t GetVerticesPerParticles() const { return layer_desc->GetVerticesPerParticles(); }
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
		virtual int DoDisplay(Renderer * renderer, GPUProgramProviderBase const * uniform_provider, RenderParams const & render_params) const override;

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
		int DoDisplayHelper(Renderer * renderer, size_t vcount, GPURenderMaterial const * final_material, GPUProgramProviderBase const * uniform_provider, InstancingInfo const & instancing) const;

		/** internal method to update particles (returns true whether there was real changes) */
		virtual bool UpdateParticles(float delta_time);

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

	template<typename PARTICLE_TYPE, typename VERTEX_TYPE, typename PER_ALLOCATION_DATA = EmptyClass, bool DYNAMIC_PARTICLES = true, bool DYNAMIC_VERTICES = true>
	class ParticleLayerTrait
	{
	public:

		/** the type for one particle */
		using particle_type = PARTICLE_TYPE;
		/** the type for one vertex */
		using vertex_type = VERTEX_TYPE;
		/** the type for one allocation */
		using per_allocation_data = PER_ALLOCATION_DATA;

		/** by default, update do nothing */
		bool UpdateParticle(float delta_time, particle_type * particle) const
		{ 
			return false; 
		}
		/** by default, particle to vertex do nothing */
		size_t ParticleToVertices(particle_type const * particle, vertex_type * vertices, size_t vertices_per_particles) const
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
		virtual size_t GetVerticesPerParticles() const;

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
		virtual size_t ParticlesToVertices(void const * particles, size_t particles_count, char * vertices, ParticleAllocation * allocation, void const * extra_allocation_data) const;
		/** update all particles */
		virtual size_t UpdateParticles(float delta_time, void * particles, size_t particle_count, ParticleAllocation * allocation, void const * extra_allocation_data);
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





	template<typename LAYER_TRAIT>
	class TypedParticleLayerDesc : public ParticleLayerDesc
	{
		CHAOS_PARTICLE_ALL_FRIENDS

	public:

		using trait_type = LAYER_TRAIT;
		using particle_type = typename trait_type::particle_type;
		using vertex_type = typename trait_type::vertex_type;
		using per_allocation_data = typename get_per_allocation_data<trait_type>::type;

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
			return new TypedParticleAllocation<particle_type, per_allocation_data>(in_layer);
		}

		/** override */
		virtual size_t UpdateParticles(float delta_time, void * particles, size_t particle_count, ParticleAllocation * allocation, void const * extra_allocation_data) override
		{
			particle_type * p = (particle_type *)particles;
			return DoUpdateParticles(delta_time, p, particle_count, allocation, has_function_BeginUpdateParticles<trait_type>::type());
		}

		/** override */
		virtual size_t ParticlesToVertices(void const * particles, size_t particles_count, char * vertices, ParticleAllocation * allocation, void const * extra_allocation_data) const override
		{
			particle_type const * p = (particle_type const *)particles;
			vertex_type * v = (vertex_type *)vertices;

			size_t vertices_per_particle = GetVerticesPerParticles();
			return DoParticlesToVertices(p, particles_count, v, vertices_per_particle, allocation, has_function_BeginParticlesToVertices<trait_type>::type());
		}

		/** override */
		virtual ClassTools::ClassRegistration const * GetParticleClass() const override
		{
			return trait.GetParticleClass();
		}

	protected:






		/** internal description */
		trait_type trait;
	};




















}; // namespace chaos

