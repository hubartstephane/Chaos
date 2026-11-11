namespace chaos
{
#if !defined CHAOS_FORWARD_DECLARATION && !defined CHAOS_TEMPLATE_IMPLEMENTATION

	/**
	 * ParticleLayer: specialization of ParticleLayerBase for some layer trait
	 */

	template<typename LAYER_TRAIT>
	class ParticleLayer : public ParticleLayerBase
	{
		static_assert(std::is_base_of_v<ParticleLayerTraitBase, LAYER_TRAIT>);

	public:

		using layer_trait_type = LAYER_TRAIT;
		using particle_type = typename layer_trait_type::particle_type;
		using vertex_type = typename layer_trait_type::vertex_type;

		using allocation_trait_type = typename get_AllocationTrait<layer_trait_type>::type;

		/** constructor */
		ParticleLayer(layer_trait_type const& in_layer_trait = {}) :
			layer_trait(in_layer_trait)
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
			return layer_trait.dynamic_vertices;
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
		virtual AutoCastable<ParticleLayerTraitBase> GetLayerTrait() override { return &layer_trait; }
		/** override */
		virtual AutoConstCastable<ParticleLayerTraitBase> GetLayerTrait() const override { return &layer_trait; }

	protected:

		/** override */
		virtual ParticleAllocationBase* DoCreateParticleAllocation() override { return new ParticleAllocation<layer_trait_type>(this); }

		/** override */
		virtual bool TickAllocation(float delta_time, ParticleAllocationBase* in_allocation)
		{
			ParticleAllocation<layer_trait_type>* allocation = auto_cast(in_allocation);
			if (allocation != nullptr)
				return allocation->TickAllocation(delta_time, &layer_trait);
			return false; // do not destroy the allocation
		}

		/** override */
		virtual void UpdateRenderingStates(GPURenderContext* render_context, bool begin) const override
		{
			if constexpr (check_method_UpdateRenderingStates_v<layer_trait_type const, GPURenderContext*, bool>)
				layer_trait.UpdateRenderingStates(render_context, begin);
			else
				ParticleLayerBase::UpdateRenderingStates(render_context, begin);
		}

		/** override */
		virtual void GenerateMeshData(GPUMesh* in_mesh, GPUVertexDeclaration* in_vertex_declaration, GPURenderMaterial* in_render_material, size_t vertex_requirement_evaluation) override;

		// convert particles into vertices
		void ParticlesToPrimitivesLoop(GPUPrimitiveOutput<vertex_type>& output);

	protected:

		/** the layer trait */
		layer_trait_type layer_trait;
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
			allocation->ParticlesToPrimitives(output, &layer_trait);
		}
		output.Flush();
	}

#endif

}; // namespace chaos