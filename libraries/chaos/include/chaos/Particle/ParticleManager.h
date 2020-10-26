#ifdef CHAOS_FORWARD_DECLARATION

namespace chaos
{
	class ParticleManager;

}; // namespace chaos
#else 

namespace chaos
{

	// ==============================================================
	// ParticleManager
	// ==============================================================

	class ParticleManager : public GPURenderable
	{
		CHAOS_PARTICLE_ALL_FRIENDS

	public:

        /** constructor */
        ParticleManager();

		/** change the bitmap atlas */
		void SetTextureAtlas(BitmapAtlas::TextureArrayAtlas * in_atlas);

		/** Search a layer by its name */
		ParticleLayerBase * FindLayer(ObjectRequest request);
		/** Search a layer by its name */
		ParticleLayerBase const * FindLayer(ObjectRequest request) const;

		/** templated method to add a layer and set some values */
		template<typename LAYER_TRAIT, typename ...PARAMS>
		ParticleLayer<LAYER_TRAIT>* AddLayer(int render_order, ObjectRequest layer_id, ObjectRequest material_request, PARAMS... params)
		{
			// find the optional GPURenderMaterial
			GPURenderMaterial * render_material = nullptr;
			if (!material_request.IsNoneRequest())
			{
				GPUResourceManager * resource_manager = MyGLFW::SingleWindowApplication::GetGPUResourceManagerInstance();
				if (resource_manager == nullptr)
					return nullptr;
				render_material = resource_manager->FindRenderMaterial(material_request);
				if (render_material == nullptr)
					return nullptr;
			}
			// create the layer
			return AddLayer<LAYER_TRAIT>(render_order, layer_id, render_material, params...);
		}

		/** templated method to add a layer and set some values */
		template<typename LAYER_TRAIT, typename ...PARAMS>
		ParticleLayer<LAYER_TRAIT> * AddLayer(int render_order, ObjectRequest layer_id, GPURenderMaterial * render_material, PARAMS... params)
		{
			ParticleLayer<LAYER_TRAIT> * result = new ParticleLayer<LAYER_TRAIT>(params...);
			if (result == nullptr)
				return nullptr;
			result->SetRenderMaterial(render_material);
			DoAddLayer(result, render_order, layer_id);			
			return result;
		}

        /** create a particle spawner */
        template<typename ...PARAMS>
        chaos::ParticleSpawner* CreateParticleSpawner(ObjectRequest layer_name, PARAMS... params)
        {
            ParticleLayerBase* layer = FindLayer(layer_name);
            if (layer == nullptr)
                return nullptr;
            return layer->CreateParticleSpawner(params...);
        }

        template<typename ...PARAMS>
        chaos::ParticleSpawner GetParticleSpawner(ObjectRequest layer_name, PARAMS... params)
        {
            ParticleLayerBase* layer = FindLayer(layer_name);
            if (layer == nullptr)
                return chaos::ParticleSpawner(nullptr);
            return layer->GetParticleSpawner(params...);
        }

		/** remove a layer from the manager */
		void RemoveLayer(ParticleLayerBase * layer);

        /** gets the buffer cache */
        GPUBufferCache& GetBufferCache() { return buffer_cache; }
        /** gets the buffer cache */
        GPUBufferCache const& GetBufferCache() const { return buffer_cache; }
        /** gets the vertex array cache */
        GPUVertexArrayCache* GetVertexArrayCache() { return vertex_array_cache.get();}
        /** gets the vertex array cache */
        GPUVertexArrayCache const * GetVertexArrayCache() const { return vertex_array_cache.get(); }

	protected:
		
		/** tick the manager */
		virtual bool DoTick(float delta_time) override;
		/** display all the particles */
		virtual int DoDisplay(GPURenderer * renderer, GPUProgramProviderBase const * uniform_provider, GPURenderParams const & render_params) override;

		/** insert layer with some initialization */
		void DoAddLayer(ParticleLayerBase * layer, int render_order, ObjectRequest layer_id);

	protected:

		/** the layer system */
		GPURenderableLayerSystem layer_system;
		/** the texture atlas */
		shared_ptr<BitmapAtlas::TextureArrayAtlas> atlas;
        /** the vertex array cache */
        shared_ptr<GPUVertexArrayCache> vertex_array_cache;
        /** a cache for GPUBuffer */
        GPUBufferCache buffer_cache;
	};

}; // namespace chaos

#endif // CHAOS_FORWARD_DECLARATION



