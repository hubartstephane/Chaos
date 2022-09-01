namespace chaos
{
#if !defined CHAOS_FORWARD_DECLARATION && !defined CHAOS_TEMPLATE_IMPLEMENTATION

	// =====================================
	// TMLayerInstance : instance of a Layer
	// =====================================

	class CHAOS_API TMLayerInstance : public GPURenderable, public JSONSerializable
	{
		CHAOS_GAMEPLAY_TM_ALL_FRIENDS;

		CHAOS_DECLARE_OBJECT_CLASS(TMLayerInstance, GPURenderable);

	public:


		/** get the tiled layer */
		TiledMap::LayerBase const* GetTiledLayer() const { return layer; }

		// shuclean : lien  => chaos (presque pas utilisé. pourrait sauter)
		//            juste le name

		/** get the level (for this layer) */
		AutoCastable<Level> GetLevel();
		/** get the level (for this layer) */
		AutoConstCastable<Level> GetLevel() const;

		/** get the level instance for this layer */
		AutoCastable<TMLevelInstance> GetLevelInstance();
		/** get the level instance for this layer */
		AutoConstCastable<TMLevelInstance> GetLevelInstance() const;

		/** get the game */
		AutoCastable<Game> GetGame();
		/** get the game */
		AutoConstCastable<Game> GetGame() const;

		/** find the child layer instance from its ID */
		TMLayerInstance* FindLayerInstanceByID(int in_id, bool recursive = false);
		/** find the child layer instance from its ID */
		TMLayerInstance const* FindLayerInstanceByID(int in_id, bool recursive = false) const;

		/** find the child layer instance from its name */
		TMLayerInstance* FindLayerInstance(ObjectRequest request, bool recursive = false);
		/** find the child layer instance from its name */
		TMLayerInstance const* FindLayerInstance(ObjectRequest request, bool recursive = false) const;

		/** find the object from its name */
		template<typename CHECK_CLASS = EmptyClass>
		AutoCastable<TMObject> FindObject(ObjectRequest request, bool recursive = false)
		{
			if (AutoCastable<TMObject> result = request.FindObject<CHECK_CLASS>(objects))
				return result;
			if (recursive)
				for (auto& layer : layer_instances)
					if (layer != nullptr)
						if (AutoCastable<TMObject> result = layer->FindObject<CHECK_CLASS>(request, recursive))
							return result;
			return nullptr;
		}
		/** find the object from its name */
		template<typename CHECK_CLASS = EmptyClass>
		AutoConstCastable<TMObject> FindObject(ObjectRequest request, bool recursive = false) const
		{
			if (AutoConstCastable<TMObject> result = request.FindObject<CHECK_CLASS>(objects))
				return result;
			if (recursive)
				for (auto& layer : layer_instances)
					if (layer != nullptr)
						if (AutoConstCastable<TMObject> result = layer->FindObject<CHECK_CLASS>(request, recursive))
							return result;
			return nullptr;
		}

		template<typename CHECK_CLASS = EmptyClass>
		AutoCastable<TMObject> FindObjectByID(int id, bool recursive = false)
		{
			for (auto& object : objects)
				if (object->GetObjectID() == id)
					if (ObjectRequest::CheckClass<CHECK_CLASS>(object.get()))
						return object.get();
			if (recursive)
				for (auto& layer : layer_instances)
					if (layer != nullptr)
						if (AutoCastable<TMObject> result = layer->FindObjectByID<CHECK_CLASS>(id, recursive))
							return result;
			return nullptr;
		}

		template<typename CHECK_CLASS = EmptyClass>
		AutoConstCastable<TMObject> FindObjectByID(int id, bool recursive = false) const
		{
			for (auto const & object : objects)
				if (object->GetObjectID() == id)
					if (ObjectRequest::CheckClass<CHECK_CLASS>(object.get()))
						return object.get();
			if (recursive)
				for (auto& layer : layer_instances)
					if (layer != nullptr)
						if (AutoConstCastable<TMObject> result = layer->FindObjectByID<CHECK_CLASS>(id, recursive))
							return result;
			return nullptr;
		}

		/** get the bounding box for the level */
		box2 GetBoundingBox(bool world_system, bool recursive) const;

		/** create a particle spawner */
		template<typename ...PARAMS>
		ParticleSpawner* CreateParticleSpawner(PARAMS... params)
		{
			if (!CreateParticleLayer())
				return nullptr;
			return particle_layer->CreateParticleSpawner(params...);
		}

		template<typename ...PARAMS>
		ParticleSpawner GetParticleSpawner(PARAMS... params)
		{
			if (!CreateParticleLayer())
				return ParticleSpawner(nullptr);
			return particle_layer->GetParticleSpawner(params...);
		}

		/** create a particle allocation for the layer */
		SpawnParticleResult SpawnParticles(size_t count, bool new_allocation = true);

		/** create the particle layer if required */
		ParticleLayerBase* CreateParticleLayer();

		/** get the layer offset */
		glm::vec2 GetLayerOffset() const { return offset; }
		/** set the layer offset */
		void SetLayerOffset(glm::vec2 const& in_offset) { offset = in_offset; }

		/** get the particle layer */
		ParticleLayerBase* GetParticleLayer() { return particle_layer.get(); }
		/** get the particle layer */
		ParticleLayerBase const* GetParticleLayer() const { return particle_layer.get(); }

		/** returns the number of objects */
		size_t GetObjectCount() const;
		/** returns an object by its index */
		AutoCastable<TMObject> GetObject(size_t index);
		/** returns an object by its index */
		AutoConstCastable<TMObject> GetObject(size_t index) const;

		/** get the layer ID */
		int GetLayerID() const { return id; }
		/** get the collision mask */
		uint64_t GetCollisionMask() const { return collision_mask; }
		/** get the child layer instances */
		std::vector<shared_ptr<TMLayerInstance>>& GetLayerInstances() { return layer_instances; }
		/** get the child layer instances */
		std::vector<shared_ptr<TMLayerInstance>> const& GetLayerInstances() const { return layer_instances; }

		/** get the parent layer */
		TMLayerInstance* GetParentLayerInstance() { return parent_layer.get(); }
		/** get the parent layer */
		TMLayerInstance const * GetParentLayerInstance() const { return parent_layer.get(); }

		/** get the rendermaterial */

		// shu49
		GPURenderMaterial* GetRenderMaterial() const;

		/** override */
		virtual bool SerializeFromJSON(nlohmann::json const& json) override;
		/** override */
		virtual bool SerializeIntoJSON(nlohmann::json& json) const override;

	protected:

		/** initialization */
		virtual bool Initialize(TMLevelInstance* in_level_instance, TiledMap::LayerBase const * in_layer, TMLayerInstance* in_parent_layer, TMObjectReferenceSolver & reference_solver);
		/** serialization of all JSON objects into an array */
		virtual bool SerializeObjectListFromJSON(nlohmann::json const& json, char const* attribute_name, std::vector<shared_ptr<TMObject>>& result);
		/** called whenever level instance is restarted */
		virtual void OnRestart();

		/** find render material according to its name (or create the default) */
		GPURenderMaterial* FindOrCreateRenderMaterial(char const* material_name) const;

		/** override */
		virtual bool DoTick(float delta_time) override;
		/** override */
		virtual int DoDisplay(GPURenderer* renderer, GPUProgramProviderInterface const * uniform_provider, GPURenderParams const& render_params) override;

		/** specialized layer */
		bool InitializeImageLayer(TiledMap::ImageLayer const * image_layer, TMObjectReferenceSolver& reference_solver);
		/** specialized layer */
		bool InitializeObjectLayer(TiledMap::ObjectLayer const * object_layer, TMObjectReferenceSolver& reference_solver);
		/** specialized layer */
		bool InitializeTileLayer(TiledMap::TileLayer const * tile_layer, TMObjectReferenceSolver& reference_solver);
		/** specialized layer */
		bool InitializeGroupLayer(TiledMap::GroupLayer const* group_layer, TMObjectReferenceSolver& reference_solver);

		/** create an object in an object layer */
		TMObjectFactory GetObjectFactory(TiledMap::TypedObject const * in_typed_object);

		/** create an object in an object layer */
		void CreateObjectParticles(TiledMap::GeometricObject const * in_geometric_object, TMObject* object, TMParticlePopulator& particle_populator);
		/** returns whether a particle should be created for object instance */
		bool ShouldCreateParticleForObject(TiledMap::PropertyOwner const * property_owner, TMObject* object) const;

		/** finalize the particles created */
		virtual bool FinalizeParticles(ParticleAllocationBase * allocation);
		/** try to search a name and a tag in the layer,  give them to the particle layer (and some other data as well) */
		virtual bool InitializeParticleLayer(ParticleLayerBase* in_particle_layer);

		/** some callbacks */
		virtual void OnLevelEnded();
		/** some callbacks */
		virtual void OnLevelStarted();

		/** compute the collision mask from the formated string */
		void ComputeLayerCollisionMask(char const* mask);

	protected:

		/** id of the object (comming from TiledMap) */
		int id = 0;
		/** displacement ratio relatively to the main layer */
		glm::vec2 displacement_ratio = glm::vec2(1.0f, 1.0f);
		/** whether the layer is to be repeated infinitely in X direction */
		bool wrap_x = false;
		/** whether the layer is to be repeated infinitely in Y direction */
		bool wrap_y = false;
		/** whether we want to ignore the limit of the layer bounding box for camera clamping (usefull for dynamic content) */
		bool infinite_bounding_box = false;
		/** material name */
		std::string material_name;

		/** whether the particles of this layer must be destroyed when LoadingCheckpoint */
		bool autoclean_particles = false;

		/** the level instance this object belongs to */
		TMLevelInstance* level_instance = nullptr;

		/** the tiled layer corresponding to this object */
		TiledMap::LayerBase const* layer = nullptr;
		/** the particle layer */
		shared_ptr<ParticleLayerBase> particle_layer;
		/** the objects */
		std::vector<shared_ptr<TMObject>> objects;

		/** the bounding box of the layer (only its own content, not sub layers) */
		box2 content_bounding_box;

		/** the collision mask for that layer */
		uint64_t collision_mask = 0;

		/** the current offset */
		glm::vec2 offset = glm::vec2(0.0f, 0.0f);

		/** the parent layer */
		weak_ptr<TMLayerInstance> parent_layer;
		/** the child layers */
		std::vector<shared_ptr<TMLayerInstance>> layer_instances;
	};

#endif

}; // namespace chaos