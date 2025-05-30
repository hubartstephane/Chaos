namespace chaos
{
#if !defined CHAOS_FORWARD_DECLARATION && !defined CHAOS_TEMPLATE_IMPLEMENTATION

	// ==============================================================
	// ParticleLayerBase
	// ==============================================================

	class CHAOS_API ParticleLayerBase : public GPURenderable
	{
		CHAOS_PARTICLE_ALL_FRIENDS

	public:

		/** constructor */
		ParticleLayerBase() = default;
		/** destructor */
		virtual ~ParticleLayerBase();

		/** returns true whether the class required is compatible with the one store in the buffer */
		template<typename PARTICLE_TYPE>
		bool IsParticleClassCompatible() const
		{
			return Class::InheritsFrom(GetParticleClass(), ClassManager::GetDefaultInstance()->FindCPPClass<PARTICLE_TYPE>(), true) == InheritanceType::YES;
		}

		/** returns the number of particle count */
		size_t ComputeMaxParticleCount() const;

		/** returns the size in memory of a particle */
		virtual size_t GetParticleSize() const { return 0; }
		/** returns the number of particles */
		virtual size_t GetParticleCount() const { return 0; }
		/** returns the size in memory of a vertex */
		virtual size_t GetVertexSize() const { return 0; }

		/** returns true whether vertices need to be updated */
		virtual bool AreVerticesDynamic() const { return true; }
		/** returns true whether particles need to be updated */
		virtual bool AreParticlesDynamic() const { return true; }

		/** get the particle ID for this system */
		virtual Class const* GetParticleClass() const { return nullptr; }

		/** returns true whether the particle type is the one given as template parameter */
		template<typename T>
		bool IsParticleType() const
		{
			return (GetParticleClass() == ClassManager::GetDefaultInstance()->FindCPPClass<T>());
		}

		/** change the atlas */
		void SetTextureAtlas(GPUAtlas* in_atlas) { atlas = in_atlas; }
		/** change the material */
		void SetRenderMaterial(GPURenderMaterial* in_render_material) { render_material = in_render_material; }

		/** get the atlas const method */
		GPUAtlas const* GetTextureAtlas() const { return atlas.get(); }
		/** get the material const method */
		GPURenderMaterial const* GetRenderMaterial() const { return render_material.get(); }

		/** spawn a given number of particles */
		SpawnParticleResult SpawnParticles(size_t count, bool new_allocation = true);

		/** create a particle spawner */
		template<typename ...PARAMS>
		ParticleSpawner* CreateParticleSpawner(PARAMS && ...params)
		{
			return new ParticleSpawner(GetTextureAtlas(), this, std::forward<PARAMS>(params)...);
		}

		/** create a particle spawner */
		template<typename ...PARAMS>
		ParticleSpawner GetParticleSpawner(PARAMS && ...params)
		{
			return ParticleSpawner(GetTextureAtlas(), this, std::forward<PARAMS>(params)...);
		}

		/** get the number of allocations */
		size_t GetAllocationCount() const;
		/** get the allocation by index */
		ParticleAllocationBase* GetAllocation(size_t index);
		/** get the allocation by index */
		ParticleAllocationBase const* GetAllocation(size_t index) const;
		/** clear all allocations */
		void ClearAllAllocations();

		/** get the vertex declaration */
		virtual GPUVertexDeclaration* GetVertexDeclaration() const { return nullptr; }

		/** get the trait */
		virtual AutoCastable<ParticleLayerTraitBase> GetLayerTrait() { return nullptr; }
		/** get the trait */
		virtual AutoConstCastable<ParticleLayerTraitBase> GetLayerTrait() const { return nullptr; }

		/** force GPU buffer update */
		void SetGPUBufferDirty() { require_GPU_update = true; }

		/** getter on the extra data */
		template<typename T>
		T* GetOwnedData()
		{
			return GetOwnedData<T>(this);
		}

		/** getter on the extra data */
		template<typename T>
		T const* GetOwnedData() const
		{
			return GetOwnedData<T>(this);
		}

		/** generate the mesh corresponding to this layer. not related to the cached mesh */
		GPUMesh* GenerateMesh(GPUDevice* in_gpu_device);

	protected:

		/** ticking the particle system */
		virtual bool DoTick(float delta_time) override;
		/** draw the layer */
		virtual int DoDisplay(GPURenderContext* render_context, GPUProgramProviderInterface const * uniform_provider, GPURenderParams const& render_params) override;

		/** change the GL rendering state */
		virtual void UpdateRenderingStates(GPURenderContext* render_context, bool begin) const;

		/** unlink all particles allocations */
		void DetachAllParticleAllocations();
		/** internal method to remove a range from the layer */
		void RemoveParticleAllocation(ParticleAllocationBase* allocation);

		/** creation of an allocation */
		virtual ParticleAllocationBase* DoCreateParticleAllocation() { return nullptr; }

		/** the effective rendering */
		int DoDisplayHelper(GPURenderContext* render_context, GPURenderMaterial const* final_material, GPUProgramProviderInterface const * uniform_provider, GPURenderParams const& render_params);

		/** internal method to update particles (returns true whether there was real changes) */
		bool TickAllocations(float delta_time);
		/** internal method to only update one allocation */
		virtual bool TickAllocation(float delta_time, ParticleAllocationBase* allocation) { return false; } // do not destroy the allocation

		/** override */
		virtual bool DoUpdateGPUResources(GPURenderContext* render_context) override;

		/** select the GPUPrimitiveOutput and update the rendering GPU resources */
		virtual void GenerateMeshData(GPUMesh* in_mesh, GPUVertexDeclaration* in_vertex_declaration, GPURenderMaterial* in_render_material, size_t previous_frame_vertices_count) {}

		/** returns the number of vertices used in a dynamic mesh */
		size_t GetDynamicMeshVertexCount(GPUMesh const* in_mesh) const;
		/** evaluate how much memory will be required for GPUMesh (returns number of vertices) */
		size_t EvaluateGPUVertexMemoryRequirement(GPUMesh const* in_mesh) const;

	protected:

		/** the manager */
		ParticleManager* particle_manager = nullptr;
		/** the texture atlas */
		shared_ptr<GPUAtlas> atlas;
		/** particles allocations */
		std::vector<shared_ptr<ParticleAllocationBase>> particles_allocations;

		/** the material used to render the layer */
		shared_ptr<GPURenderMaterial> render_material;
		/** the vertex declaration */
		shared_ptr<GPUVertexDeclaration> vertex_declaration;

		/** the corresponding dynamic mesh */
		shared_ptr<GPUMesh> mesh;
		/** whether there was changes in particles, and a vertex array need to be recomputed */
		bool require_GPU_update = false;
};

	// ==============================================================
	// ParticleLayer
	// ==============================================================

	template<typename LAYER_TRAIT>
	class ParticleLayer : public ParticleLayerBase, public DataOwner<LAYER_TRAIT>
	{
		static_assert(std::is_base_of_v<ParticleLayerTraitBase, LAYER_TRAIT>);

	public:

		using layer_trait_type = LAYER_TRAIT;
		using particle_type = typename layer_trait_type::particle_type;
		using vertex_type = typename layer_trait_type::vertex_type;

		using allocation_trait_type = typename get_AllocationTrait<layer_trait_type>::type;

		/** constructor */
		ParticleLayer(layer_trait_type const& in_layer_trait = {}) :
			DataOwner<layer_trait_type>(in_layer_trait)
		{
			assert(ClassManager::GetDefaultInstance()->FindCPPClass<particle_type>() != nullptr); // ensure class is declared
		}

		/** returns the size in memory of a particle */
		virtual size_t GetParticleSize() const override { return sizeof(particle_type); }
		/** override */
		virtual size_t GetParticleCount() const override
		{
			size_t result = 0;
			for (auto const& allocation : particles_allocations)
				if (allocation != nullptr)
					result += allocation->GetParticleCount();
			return result;
		}
		/** override */
		virtual size_t GetVertexSize() const override { return sizeof(vertex_type); }
		/** override */
		virtual bool AreVerticesDynamic() const override
		{
			return this->data.dynamic_vertices;
		}
		/** override */
		virtual Class const* GetParticleClass() const override { return ClassManager::GetDefaultInstance()->FindCPPClass<particle_type>(); }
		/** override */
		virtual GPUVertexDeclaration* GetVertexDeclaration() const override
		{
			GPUVertexDeclaration* result = new GPUVertexDeclaration;
			if (result != nullptr)
			{
				GetTypedVertexDeclaration(result, boost::mpl::identity<vertex_type>());
				result->SetEffectiveVertexSize(sizeof(vertex_type));
			}
			return result;
		}
		/** override */
		virtual AutoCastable<ParticleLayerTraitBase> GetLayerTrait() override { return &this->data; }
		/** override */
		virtual AutoConstCastable<ParticleLayerTraitBase> GetLayerTrait() const override { return &this->data; }

	protected:

		/** override */
		virtual ParticleAllocationBase* DoCreateParticleAllocation() override { return new ParticleAllocation<layer_trait_type>(this); }

		/** override */
		virtual bool TickAllocation(float delta_time, ParticleAllocationBase* in_allocation)
		{
			ParticleAllocation<layer_trait_type>* allocation = auto_cast(in_allocation);
			if (allocation != nullptr)
				return allocation->TickAllocation(delta_time, &this->data);
			return false; // do not destroy the allocation
		}

		/** override */
		virtual void UpdateRenderingStates(GPURenderContext* render_context, bool begin) const override
		{
			if constexpr (check_method_UpdateRenderingStates_v<layer_trait_type const, GPURenderContext*, bool>)
				this->data.UpdateRenderingStates(render_context, begin);
			else
				ParticleLayerBase::UpdateRenderingStates(render_context, begin);
		}

		/** override */
		virtual void GenerateMeshData(GPUMesh* in_mesh, GPUVertexDeclaration* in_vertex_declaration, GPURenderMaterial* in_render_material, size_t vertex_requirement_evaluation) override;

		// convert particles into vertices
		void ParticlesToPrimitivesLoop(GPUPrimitiveOutput<vertex_type>& output);
	};


#elif defined CHAOS_TEMPLATE_IMPLEMENTATION

	template<typename LAYER_TRAIT>
	void ParticleLayer<LAYER_TRAIT>::GenerateMeshData(GPUMesh* in_mesh, GPUVertexDeclaration* in_vertex_declaration, GPURenderMaterial* in_render_material, size_t vertex_requirement_evaluation)
	{
		GPUPrimitiveOutput<vertex_type> output(in_mesh, in_vertex_declaration, in_render_material, vertex_requirement_evaluation);
		ParticlesToPrimitivesLoop(output);
	}

	template<typename LAYER_TRAIT>
	void ParticleLayer<LAYER_TRAIT>::ParticlesToPrimitivesLoop(GPUPrimitiveOutput<vertex_type>& output)
	{
		size_t count = particles_allocations.size();
		for (size_t i = 0; i < count; ++i)
		{
			// get the allocation, ignore if invisible
			ParticleAllocation<layer_trait_type>* allocation = auto_cast(particles_allocations[i].get());
			if (!allocation->IsVisible())
				continue;
			// transform particles into vertices
			allocation->ParticlesToPrimitives(output, &this->data);
		}
		output.Flush();
	}

#endif

}; // namespace chaos