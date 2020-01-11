#pragma once

#include <chaos/StandardHeaders.h>
#include <chaos/GPUClasses.h>
#include <chaos/ReferencedObject.h>
#include <chaos/GPURenderMaterial.h>
#include <chaos/GPUBuffer.h>
#include <chaos/GPUBufferCache.h>
#include <chaos/ReferencedObject.h>
#include <chaos/GPUVertexDeclaration.h>
#include <chaos/GPUVertexArrayCache.h>
#include <chaos/GPUProgramProvider.h>
#include <chaos/GPUDrawPrimitive.h>
#include <chaos/GPUDynamicMesh.h>
#include <chaos/TextureArrayAtlas.h>
#include <chaos/ClassTools.h>
#include <chaos/ParticleTools.h>
#include <chaos/GPURenderable.h>
#include <chaos/ParticleAccessor.h>
#include <chaos/ParticleAllocationTrait.h>
#include <chaos/PrimitiveOutput.h>

#define OLD_RENDERING 1

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
#define CHAOS_PARTICLE_CLASSES \
(ParticleAllocationBase) \
(ParticleLayerBase) \
(ParticleManager)

	// forward declaration
#define CHAOS_PARTICLE_FORWARD_DECL(r, data, elem) class elem;
	BOOST_PP_SEQ_FOR_EACH(CHAOS_PARTICLE_FORWARD_DECL, _, CHAOS_PARTICLE_CLASSES)

		// friendship macro
#define CHAOS_PARTICLE_FRIEND_DECL(r, data, elem) friend class elem;
#define CHAOS_PARTICLE_ALL_FRIENDS BOOST_PP_SEQ_FOR_EACH(CHAOS_PARTICLE_FRIEND_DECL, _, CHAOS_PARTICLE_CLASSES)

// detect whether class has a member named XXX (use has_XXX<T>::value => bool => convert into boost::mpl::bool_)
CHAOS_GENERATE_HAS_MEMBER(dynamic_particles);
CHAOS_GENERATE_HAS_MEMBER(dynamic_vertices);
CHAOS_GENERATE_HAS_MEMBER(vertices_per_particle);
CHAOS_GENERATE_HAS_MEMBER(primitive_type);

// detect whether classes have some functions
CHAOS_GENERATE_HAS_FUNCTION_SIGNATURE(Tick);
CHAOS_GENERATE_HAS_FUNCTION_SIGNATURE(UpdateParticle);
CHAOS_GENERATE_HAS_FUNCTION_SIGNATURE(ParticleToVertices);
CHAOS_GENERATE_HAS_FUNCTION_SIGNATURE(BeginUpdateParticles);
CHAOS_GENERATE_HAS_FUNCTION_SIGNATURE(BeginParticlesToVertices);

// detect whether classes have some functions
CHAOS_GENERATE_HAS_FUNCTION_METACLASS(Tick)
CHAOS_GENERATE_HAS_FUNCTION_METACLASS(UpdateParticle)
CHAOS_GENERATE_HAS_FUNCTION_METACLASS(ParticleToVertices)
CHAOS_GENERATE_HAS_FUNCTION_METACLASS(BeginUpdateParticles)
CHAOS_GENERATE_HAS_FUNCTION_METACLASS(BeginParticlesToVertices)

// detect whether class have a nested class
CHAOS_GENERATE_HAS_TRAIT(LayerTrait)

// ==============================================================
// ParticleTraitTools
// ==============================================================

class ParticleTraitTools
{

	// shuxxx : there is no way YET to know whether the flags 'has_vertices_per_particle' ... are static or per instance
	//			look at the following code, we use the   
	//
	//				has_vertices_per_particle<...>   
	//
	//			template in all cases
	
	
	public:

	/** returns the number of vertices require for one particle */
	template<typename TRAIT_TYPE>
	static size_t GetVerticesPerParticle(TRAIT_TYPE const & trait)
	{ 
        if constexpr (has_vertices_per_particle_v<TRAIT_TYPE>)
            return trait.vertices_per_particle;
        else
            return 2 * 3; // default value (2 triangles to render a quad)
	}
	/** returns whether the vertices are dynamic */
	template<typename TRAIT_TYPE>
	static bool AreVerticesDynamic(TRAIT_TYPE const & trait)
	{
        if constexpr (has_dynamic_vertices_v<TRAIT_TYPE>)
            return trait.dynamic_vertices;
        else
            return true;
	}
	/** returns whether the particles are dynamic */
	template<typename TRAIT_TYPE>
	static bool AreParticlesDynamic(TRAIT_TYPE const & trait)
	{
        if constexpr (has_dynamic_particles_v<TRAIT_TYPE>)
            return trait.dynamic_particles;
        else
            return true;
	}

	/** returns whether the vertices are dynamic (without an instance to read) */
	template<typename TRAIT_TYPE>
	static bool AreVerticesDynamicStatic()
	{
        if constexpr (has_dynamic_vertices_v<TRAIT_TYPE>)
            return TRAIT_TYPE::dynamic_vertices;
        else
            return true;
    }
	/** returns whether the particles are dynamic (without an instance to read) */
	template<typename TRAIT_TYPE>
	static bool AreParticlesDynamicStatic()
	{
        if constexpr (has_dynamic_particles_v<TRAIT_TYPE>)
            return TRAIT_TYPE::dynamic_particles;
        else
            return true;
	}
    /** returns the primitive type used for the rendering */
    template<typename TRAIT_TYPE>
    static constexpr PrimitiveType GetPrimitiveType()
    {
        if constexpr (has_primitive_type_v<TRAIT_TYPE>)
            return TRAIT_TYPE::primitive_type;
        else
            return PrimitiveType::triangle_pair;


        // shuxxx


        return PrimitiveType::quad;






    }
    /** returns the primitive type used for rendering (OpenGL point of view) */
    template<typename TRAIT_TYPE>
    static constexpr GLenum GetGLPrimitiveType()
    {
        return chaos::GetGLPrimitiveType(GetPrimitiveType<TRAIT_TYPE>()); // see PrimitiveOutput.h
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
        /** get an AutoCasting particle accessor */
        AutoCastedParticleAccessor GetParticleAccessor()
        {
            return AutoCastedParticleAccessor(this);
        }
        /** get an AutoCasting particle accessor */
        AutoCastedParticleConstAccessor GetParticleAccessor() const
        {
            return AutoCastedParticleConstAccessor(this);
        }
        /** get an AutoCasting particle accessor */
        AutoCastedParticleConstAccessor GetParticleConstAccessor() const
        {
            return AutoCastedParticleConstAccessor(this);
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
		
		/** get the layer for this allocation */
		ParticleLayerBase * GetLayer() { return layer; }
		/** get the layer for this allocation */
		ParticleLayerBase const * GetLayer() const { return layer; }

		/** remove the allocation from its layer */
		void RemoveFromLayer();

#if !OLD_RENDERING


        /** transforms the particles into vertices in the buffer */
     //   virtual void ParticlesToVertices(PrimitiveOutputBase & output, void const* layer_trait) const { }





#endif

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

    // forward declaration (required for friendship)
    template<typename ALLOCATION_TRAIT>
    class ParticleLayer;

	template<typename ALLOCATION_TRAIT>
	class ParticleAllocation : public ParticleAllocationBase
	{
        friend class ParticleLayer<ALLOCATION_TRAIT>;

	public:

		using allocation_trait_type = ALLOCATION_TRAIT;
		using particle_type = typename allocation_trait_type::particle_type;
		using vertex_type = typename allocation_trait_type::vertex_type;
		using layer_trait_type = typename get_LayerTrait<allocation_trait_type>::type;

		/** constructor */
		ParticleAllocation(ParticleLayerBase* in_layer) : ParticleAllocationBase(in_layer) {}

		/** override */
		virtual ClassTools::ClassRegistration const * GetParticleClass() const override
		{
			return ClassTools::GetClassRegistration<particle_type>();
		}

        /** override */
        virtual void* GetParticleBuffer() override
        { 
            return (particles.size() == 0) ? nullptr : &particles[0];
        }
        /** override */
        virtual void const* GetParticleBuffer() const override
        { 
            return (particles.size() == 0) ? nullptr : &particles[0];
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

#if !OLD_RENDERING
        /** transforms the particles into vertices in the buffer */
        template<typename PRIMITIVE_OUTPUT_TYPE>
        void ParticlesToVertices(PRIMITIVE_OUTPUT_TYPE& output, void const* layer_trait) const
        {
            if constexpr (has_LayerTrait_v<allocation_trait_type>)
            {
                layer_trait_type const* typed_layer_trait = (layer_trait_type const*)layer_trait;

                if constexpr (has_function_BeginParticlesToVertices_v<allocation_trait_type>)
                {
                    DoParticlesToVerticesLoop(
                        output,
                        allocation_trait.BeginParticlesToVertices(GetParticleConstAccessor<particle_type>(), typed_layer_trait), // do not use a temp variable, so it can be a left-value reference
                        typed_layer_trait);
                }
                else
                {
                    DoParticlesToVerticesLoop(
                        output,
                        typed_layer_trait);
                }
            }
            else
            {
                if constexpr (has_function_BeginParticlesToVertices_v<allocation_trait_type>)
                {
                    DoParticlesToVerticesLoop(
                        output,
                        allocation_trait.BeginParticlesToVertices(GetParticleConstAccessor<particle_type>(), typed_layer_trait)); // do not use a temp variable, so it can be a left-value reference
                }
                else
                {
                    DoParticlesToVerticesLoop(output);
                }
            }
        }
#endif

	protected:

		/** override */
		virtual bool TickAllocation(float delta_time, void const * layer_trait) override
		{ 
            layer_trait_type const* typed_layer_trait = (layer_trait_type const*)layer_trait;

            bool destroy_allocation = false;

            // tick the allocation (if the trait has a Tick(...) function)
            if constexpr (has_function_Tick_v<allocation_trait_type>)
            {
                if constexpr (has_LayerTrait_v<allocation_trait_type>)
                    destroy_allocation = allocation_trait.Tick(delta_time, this, typed_layer_trait); // let the trait decide whether the allocation is to be destroyed
                else
                    destroy_allocation = allocation_trait.Tick(delta_time, this); // let the trait decide whether the allocation is to be destroyed
            }
            else
            {
                destroy_allocation = false; // no Tick(...), no destruction
            }

            // Tick the particles
            if (!destroy_allocation)      
            {
                // has_function_UpdateParticle<allocation_trait_type>::type()); // shuxxx FIXME : this template does not detect template function ! => see ParticleDefault that require a template

                if constexpr (boost::mpl::true_().value)
                {
                    if (particles.size() > 0)
                        destroy_allocation = UpdateParticles(delta_time, typed_layer_trait);
                }
            }
            return destroy_allocation;
		}

		bool UpdateParticles(float delta_time, layer_trait_type const * layer_trait)
		{
			size_t particle_count = GetParticleCount();

            size_t remaining_particles = 0;

            ParticleAccessor<particle_type> particle_accessor = GetParticleAccessor();

            if constexpr (has_function_BeginUpdateParticles<allocation_trait_type>::type())
            {
                if constexpr (has_LayerTrait_v<allocation_trait_type>)
                {
                    remaining_particles = DoUpdateParticlesLoop(
                        delta_time,
                        particle_accessor,
                        allocation_trait.BeginUpdateParticles(delta_time, particle_accessor, layer_trait), // do not use a temp variable, so it can be a left-value reference
                        layer_trait);
                }
                else
                {
                    remaining_particles = DoUpdateParticlesLoop(
                        delta_time,
                        particle_accessor,
                        allocation_trait.BeginUpdateParticles(delta_time, particle_accessor, layer_trait)); // do not use a temp variable, so it can be a left-value reference
                }
            }
            else
            {
                if constexpr (has_LayerTrait_v<allocation_trait_type>)
                    remaining_particles = DoUpdateParticlesLoop(delta_time, particle_accessor, layer_trait);
                else
                    remaining_particles = DoUpdateParticlesLoop(delta_time, particle_accessor);
            }

			if (remaining_particles == 0 && GetDestroyWhenEmpty())
				return true; // destroy allocation
			else if (remaining_particles != particle_count) // clean buffer of all particles that have been destroyed
				Resize(remaining_particles);
			return false; // do not destroy allocation
		}

		template<typename ...PARAMS>
		size_t DoUpdateParticlesLoop(float delta_time, ParticleAccessor<particle_type> particle_accessor, PARAMS... params)
		{
            size_t particle_count = particle_accessor.GetCount();

			// tick all particles. overide all particles that have been destroyed by next on the array
			size_t j = 0;
			for (size_t i = 0; i < particle_count; ++i)
			{
				if (!allocation_trait.UpdateParticle(delta_time, &particle_accessor[i], params...)) // particle not destroyed ?
				{
					if (i != j)
                        particle_accessor[j] = particle_accessor[i];
					++j;
				}
			}
			return j; // final number of particles
		}















#if !OLD_RENDERING
  

        template<typename PRIMITIVE_OUTPUT_TYPE, typename ...PARAMS>
        void DoParticlesToVerticesLoop(PRIMITIVE_OUTPUT_TYPE& output, PARAMS... params) const
        {
            ParticleConstAccessor<particle_type> particle_accessor = GetParticleAccessor();

            size_t particle_count = particle_accessor.GetCount();
            for (size_t i = 0; i < particle_count; ++i)
            {
                allocation_trait.ParticleToVertices(particle_accessor[i], output, params...);
            }
        }





#else


		/** override */
		virtual size_t ParticlesToVertices(void * v, void const * layer_trait) const override
		{ 
			layer_trait_type const * typed_layer_trait = (layer_trait_type const *)layer_trait;


            // XXX : has_function_ParticleToVertices<allocation_trait_type>::type()*/); //  shuxxx FIXME : this template does not detect template function => see ParticleDefault

            vertex_type* vertices = (vertex_type*)v;

            size_t vertices_per_particle = ParticleTraitTools::GetVerticesPerParticle(allocation_trait);

            size_t result = 0;
            if constexpr (has_function_BeginParticlesToVertices_v<allocation_trait_type>)
            {
                if constexpr (has_LayerTrait_v<allocation_trait_type>)
                {
                    result = DoParticlesToVerticesLoop(
                        vertices,
                        vertices_per_particle,
                        allocation_trait.BeginParticlesToVertices(GetParticleConstAccessor<particle_type>(), typed_layer_trait), // do not use a temp variable, so it can be a left-value reference
                        typed_layer_trait);
                }
                else
                {
                    result = DoParticlesToVerticesLoop(
                        vertices,
                        vertices_per_particle,
                        allocation_trait.BeginParticlesToVertices(GetParticleConstAccessor<particle_type>())); // do not use a temp variable, so it can be a left-value reference
                }
            }
            else
            {
                if constexpr (has_LayerTrait_v<allocation_trait_type>)
                {
                    result = DoParticlesToVerticesLoop(
                        vertices,
                        vertices_per_particle,
                        typed_layer_trait);
                }
                else
                {
                    result = DoParticlesToVerticesLoop(
                        vertices,
                        vertices_per_particle);
                }
            }            
            return result;
		}

		template<typename ...PARAMS>
		size_t DoParticlesToVerticesLoop(vertex_type * vertices, size_t vertices_per_particle, PARAMS... params) const
		{
            ParticleConstAccessor<particle_type> particle_accessor = GetParticleAccessor();

            size_t particle_count = particle_accessor.GetCount();
			size_t result = 0;
			// transforms particles to vertices
			vertex_type * v = vertices;

			for (size_t i = 0; i < particle_count; ++i)
			{
				size_t new_vertices = allocation_trait.ParticleToVertices(&particle_accessor[i], v, vertices_per_particle, params...);
				assert(new_vertices <= vertices_per_particle);
				result += new_vertices;
				v += new_vertices;
			}
			return result;
		}









#endif








	protected:

		/** the particles buffer */
		std::vector<particle_type> particles;
		/** the trait of the allocation (some extra data + some functions) */
		allocation_trait_type allocation_trait;
	};

	// ==============================================================
	// ParticleLayerBase
	// ==============================================================

	class ParticleLayerBase : public GPURenderable
	{
		CHAOS_PARTICLE_ALL_FRIENDS

	public:

		/** constructor */
        ParticleLayerBase() = default;
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
        /** returns the number of particles */
        virtual size_t GetParticleCount() const { return 0; }
		/** returns the size in memory of a vertex */
		virtual size_t GetVertexSize() const { return 0; }
		/** returns the number of vertices required for each particles */

#if OLD_RENDERING
		virtual size_t GetVerticesPerParticle() const { return 2 * 3; } // 2 triangles per particles to have a square = 6 vertices
#else
        virtual size_t GetVerticesPerParticle() const { return 0; }
#endif



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
		virtual GPUVertexDeclaration * GetVertexDeclaration() const { return nullptr; }

        /** returns the OpenGL primitive type */
        virtual GLenum GetGLPrimitiveType() const { return GL_NONE; }

	public:

		/** creation of an allocation */
		virtual ParticleAllocationBase * DoSpawnParticles() { return nullptr; }
		/** returns the layer trait */
		virtual void * GetLayerTrait() { return nullptr; }
		/** returns the layer trait */
		virtual void const * GetLayerTrait() const { return nullptr; }

		/** clear all allocations */
		void ClearAllAllocations();	

	protected:

		/** ticking the particle system */
		virtual bool DoTick(double delta_time) override;
		/** draw the layer */
		virtual int DoDisplay(GPURenderer * renderer, GPUProgramProviderBase const * uniform_provider, GPURenderParams const & render_params) override;

		/** change the GL rendering state */
		void UpdateRenderingStates(GPURenderer * renderer, bool begin) const;

		/** unlink all particles allocations */
		void DetachAllParticleAllocations();
		/** internal method to remove a range from the layer */
		void RemoveParticleAllocation(ParticleAllocationBase * allocation);

		/** internal method to update the GPU buffers */
		size_t DoUpdateGPUBuffers(char * buffer, size_t vertex_buffer_size);

		/** update the vertex declaration */
		void UpdateVertexDeclaration();
		/** the effective rendering */
		int DoDisplayHelper(GPURenderer * renderer, GPURenderMaterial const * final_material, GPUProgramProviderBase const * uniform_provider, GPURenderParams const & render_params);

		/** internal method to update particles (returns true whether there was real changes) */
		virtual bool TickAllocations(double delta_time);

		/** override */
		virtual bool DoUpdateGPUResources(GPURenderer * renderer) override;

#if !OLD_RENDERING
        /** select the PrimitiveOutput and update the rendering GPU resources */
        virtual void DoUpdateGPUBuffers(GPURenderer* renderer, size_t previous_frame_vertices_count) {}
#endif

        /** returns the number of vertices used in a dynamic mesh */
        size_t GetDynamicMeshVertexCount(GPUDynamicMesh const& mesh) const;
        /** evaluate how much memory will be required for GPUDynamicMesh (returns number of vertices) */
        size_t EvaluateGPUVertexMemoryRequirement() const;

	protected:

        /** the manager */
        ParticleManager * particle_manager = nullptr;
		/** the texture atlas */
		shared_ptr<BitmapAtlas::TextureArrayAtlas> atlas;
        /** particles allocations */
        std::vector<shared_ptr<ParticleAllocationBase>> particles_allocations;

		/** the material used to render the layer */
		shared_ptr<GPURenderMaterial> render_material;
        /** the vertex declaration */
        shared_ptr<GPUVertexDeclaration> vertex_declaration;

        /** whether there was changes in particles, and a vertex array need to be recomputed */
        bool require_GPU_update = false;


        /** the corresponding dynamic mesh */
        GPUDynamicMesh dynamic_mesh;


		/** the vertex buffer for the rendering */
		shared_ptr<GPUBuffer> vertex_buffer;
		/** the cache for vertex array */
		GPUVertexArrayCache vertex_array_cache;

		/** number of used vertices in the vertex buffer */
		size_t vertices_count = 0;
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
        virtual size_t GetParticleCount() const override
        { 
            size_t result = 0;
            size_t count = particles_allocations.size();
            for (size_t i = 0; i < count; ++i)
            {
                ParticleAllocation<allocation_trait_type> const* allocation = auto_cast(particles_allocations[i].get());
                if (allocation == nullptr)
                    continue;
                result += allocation->particles.size();
            }
            return result;
        }
        /** override */
		virtual size_t GetVertexSize() const override { return sizeof(vertex_type); }
		/** override */
#if OLD_RENDERING
		virtual size_t GetVerticesPerParticle() const override 
        { 
            return ParticleTraitTools::GetVerticesPerParticle(layer_trait); 
        }
#else
        virtual size_t GetVerticesPerParticle() const override
        {
            return chaos::GetVerticesPerParticle(ParticleTraitTools::GetPrimitiveType<allocation_trait_type>()); // see PrimitiveOutput.h
        }

#endif
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
		virtual GPUVertexDeclaration * GetVertexDeclaration() const override { return GetTypedVertexDeclaration(boost::mpl::identity<vertex_type>()); }
        /** override */
        virtual GLenum GetGLPrimitiveType() const override { return ParticleTraitTools::GetGLPrimitiveType<allocation_trait_type>(); }

	protected:

		/** override */
		virtual ParticleAllocationBase * DoSpawnParticles() override { return new ParticleAllocation<allocation_trait_type>(this); }
		/** override */
		virtual void * GetLayerTrait() { return &layer_trait; }
		/** override */
		virtual void const * GetLayerTrait() const { return &layer_trait; }

#if !OLD_RENDERING
        /** override */
        virtual void DoUpdateGPUBuffers(GPURenderer* renderer, size_t vertex_requirement_evaluation) override
        {
            // select PrimitiveOutput and collect vertices
            if constexpr (ParticleTraitTools::GetPrimitiveType<ALLOCATION_TRAIT>() == PrimitiveType::triangle)
            {
                TriangleOutput<vertex_type> output(&dynamic_mesh, &particle_manager->GetBufferCache(), vertex_declaration.get(), renderer, vertex_requirement_evaluation);
                ParticlesToVerticesLoop(output);
            }
            else if constexpr (ParticleTraitTools::GetPrimitiveType<ALLOCATION_TRAIT>() == PrimitiveType::triangle_pair)
            {
                TrianglePairOutput<vertex_type> output(&dynamic_mesh, &particle_manager->GetBufferCache(), vertex_declaration.get(), renderer, vertex_requirement_evaluation);
                ParticlesToVerticesLoop(output);
            }
            else if constexpr (ParticleTraitTools::GetPrimitiveType<ALLOCATION_TRAIT>() == PrimitiveType::quad)
            {
                QuadOutput<vertex_type> output(&dynamic_mesh, &particle_manager->GetBufferCache(), vertex_declaration.get(), renderer, vertex_requirement_evaluation);
                ParticlesToVerticesLoop(output);
            }
            else if constexpr (ParticleTraitTools::GetPrimitiveType<ALLOCATION_TRAIT>() == PrimitiveType::triangle_strip)
            {
                TriangleStripOutput<vertex_type> output(&dynamic_mesh, &particle_manager->GetBufferCache(), vertex_declaration.get(), renderer, vertex_requirement_evaluation);
                ParticlesToVerticesLoop(output);
            }
            else if constexpr (ParticleTraitTools::GetPrimitiveType<ALLOCATION_TRAIT>() == PrimitiveType::triangle_fan)
            {
                TriangleFanOutput<vertex_type> output(&dynamic_mesh, &particle_manager->GetBufferCache(), vertex_declaration.get(), renderer, vertex_requirement_evaluation);
                ParticlesToVerticesLoop(output);
            }
        }

        // convert particles into vertices
        template<typename PRIMITIVE_OUTPUT_TYPE>
        void ParticlesToVerticesLoop(PRIMITIVE_OUTPUT_TYPE& output)
        {
            size_t count = particles_allocations.size();
            for (size_t i = 0; i < count; ++i)
            {
                // get the allocation, ignore if invisible
                ParticleAllocation<allocation_trait_type> * allocation = auto_cast(particles_allocations[i].get());
                if (!allocation->IsVisible())
                    continue;
                // transform particles into vertices
                allocation->ParticlesToVertices(output, GetLayerTrait());
            }
            output.Flush();
        }
#endif

	protected:

		/** the trait of the layer */
		layer_trait_type layer_trait;
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

