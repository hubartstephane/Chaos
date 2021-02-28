#ifdef CHAOS_FORWARD_DECLARATION

namespace chaos
{
	// =====================================
	// TMLevel : utility
	// =====================================

	// XXX : while we want to create GeometricObject during tile creation we will need a fake 'geometric_object' (or a geometric_object on the fly)
	//       we cannot afford to create such a costly object for each tile (considering that 99% of the tiles should not require such a fake object)
	// 
	//       that's why we use factories (even for non tile layers).
	//       for a tile we:
	//         - get a factory
	//         - create the fake geometric_object
	//         - use the factory with this geometric object on the fly
	//
	// XXX : the functions to get factories receive a : TypedObject         (because we just need some properties and a type to know which object we would like to create (and that's what we get in Tile generation method))
	//       the factories receive a                  : GeometricObjectType (because for the real object creation we need more than a TypedObject)
	//
	// XXX : beware, the CHAOS_TM_MAKE_OBJECT_FACTORY(...) macro hide the complexity of lambda parameters capture. Use with caution 
	//

	/** a functor for geometric object factory */
	using TMObjectFactory = std::function<TMObject* (TiledMap::GeometricObject const*, TMObjectReferenceSolver&)>;
	/** an helper to make a lambda inside DoGetObjectFactory */
#define CHAOS_TM_MAKE_OBJECT_FACTORY(x) [this, in_layer_instance](chaos::TiledMap::GeometricObject const *in_geometric_object, chaos::TMObjectReferenceSolver & in_reference_solver) { x }


	template<typename T>
	class PropertyOwnerOverride;

}; // namespace chaos

#else

namespace chaos
{
		// ==========================================
		// PropertyOwnerOverride : an utility class to capture the properties of a source 
		// ==========================================

	template<typename T>
	class PropertyOwnerOverride : public T
	{
	public:

		/** constructor */
		PropertyOwnerOverride(TiledMap::BaseObject* in_owner, TiledMap::PropertyOwner const * in_property_owner) :
			T(in_owner),
			property_owner(in_property_owner) {}

		/** override */
		virtual TiledMap::Property const* FindProperty(char const* name, TiledMap::PropertyType type_id) const override
		{
			TiledMap::Property const* result = nullptr;
			if (property_owner != nullptr)
				result = property_owner->FindProperty(name, type_id);
			if (result == nullptr)
				result = T::FindProperty(name, type_id);
			return result;
		}

	protected:

		/** a substitute property owner to fake the system */
		TiledMap::PropertyOwner const * property_owner = nullptr;
	};


	// =====================================
	// TMLevel : a level described by a tiledmap
	// =====================================

	class TMLevel : public Level
	{
		CHAOS_GAMEPLAY_TM_ALL_FRIENDS;

		CHAOS_DECLARE_OBJECT_CLASS2(TMLevel, Level);

	public:

		/** constructor */
		TMLevel();

		/** initialization from tiled_map */
		virtual bool Initialize(TiledMap::Map * in_tiled_map);

		/** get the tiled map */
		TiledMap::Map* GetTiledMap() { return tiled_map.get(); }
		/** get the tiled map */
		TiledMap::Map const* GetTiledMap() const { return tiled_map.get(); }

	protected:

		/** called to flush some particles into a layer allocation */
		virtual bool FlushParticlesIntoAllocation(TMLayerInstance* layer_instance, ParticleAllocationBase* allocation, TMParticle const* particles, size_t particle_count);

		/** create a typed object based of a 'classname' property */
		virtual TMObjectFactory DoGetExplicitObjectFactory(TMLayerInstance* in_layer_instance, TiledMap::TypedObject const * in_typed_object);
		/** create a typed object specializable method */
		virtual TMObjectFactory DoGetObjectFactory(TMLayerInstance* in_layer_instance, TiledMap::TypedObject const * in_typed_object);
		/** create a typed object 'entry point' */
		TMObjectFactory GetObjectFactory(TMLayerInstance* in_layer_instance, TiledMap::TypedObject const * in_typed_object);

		/** create a Path specializable method */
		virtual TMPath * DoCreatePath();
		/** create a Camera specializable method */
		virtual TMCameraTemplate* DoCreateCameraTemplate();
		/** create a PlayerStartObject specializable method */
		virtual TMPlayerStart* DoCreatePlayerStart();
		/** create a FinishingTriggerObject specializable method */
		virtual TMChangeLevelTrigger* DoCreateChangeLevelTrigger();
		/** create a CheckpointTriggerObject specializable method */
		virtual TMCheckpointTrigger* DoCreateCheckpointTrigger();
		/** create a NotificationTriggerObject specializable method */
		virtual TMNotificationTrigger* DoCreateNotificationTrigger();
		/** create a SoundTriggerObject specializable method */
		virtual TMSoundTrigger* DoCreateSoundTrigger();

		/** create a PlayerStartObject specializable method */
		virtual TMLayerInstance* DoCreateLayerInstance();

		/** create a layer instance 'entry point' */
		TMLayerInstance* CreateLayerInstance(TMLevelInstance* in_level_instance, TiledMap::LayerBase* in_layer, TMLayerInstance* in_parent_layer, TMObjectReferenceSolver& reference_solver);

		/** get the folder in which bitmaps are stored in Game::Atlas */
		virtual BitmapAtlas::FolderInfo const* GetFolderInfo(TMLayerInstance* layer_instance) const;
		/** get the atlas to use for the rendering */
		virtual BitmapAtlas::TextureArrayAtlas const* GetTextureAtlas(TMLayerInstance* layer_instance) const;
		/** create a particle layer */
		virtual ParticleLayerBase* DoCreateParticleLayer(TMLayerInstance* layer_instance);

		/** called after all particles of a layers has been created, so we can plug additionnal data */
		virtual bool FinalizeLayerParticles(TMLayerInstance* layer_instance, ParticleAllocationBase* allocation);

		/** the default material when not specified */
		virtual GPURenderMaterial* GenDefaultRenderMaterial();

	protected:

		/** the tiled map corresponding to this level */
		shared_ptr<TiledMap::Map> tiled_map;
	};

	// =====================================
	// TMLayerInstance : instance of a Layer
	// =====================================

	class TMLayerInstance : public GPURenderable, public JSONSerializable
	{
		CHAOS_GAMEPLAY_TM_ALL_FRIENDS;

		CHAOS_DECLARE_OBJECT_CLASS2(TMLayerInstance, GPURenderable);

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
		ParticleAllocationBase* SpawnParticles(size_t count, bool new_allocation = true);

		/** spawn + user initialization methods */
		template<typename INIT_PARTICLE_FUNC>
		ParticleAllocationBase* SpawnParticles(size_t count, bool new_allocation, INIT_PARTICLE_FUNC init_func)
		{
			ParticleAllocationBase* result = SpawnParticles(count, new_allocation);
			// call user initialization function
			if (result != nullptr)
			{
				size_t allocation_count = result->GetParticleCount();
				init_func(result->GetParticleAccessor(allocation_count - count, count));  // partial accessor, take the last particles in the array
			}
			return result;
		}

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
		GPURenderMaterial* FindOrCreateRenderMaterial(char const* material_name);

		/** override */
		virtual bool DoTick(float delta_time) override;
		/** override */
		virtual int DoDisplay(GPURenderer* renderer, GPUProgramProviderBase const* uniform_provider, GPURenderParams const& render_params) override;

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

		/** whether the layer is to be rendered as 8 bit mode (XXX: this only affects particles the initial particles created) */
		bool height_bits_mode = false;

		/** the parent layer */
		weak_ptr<TMLayerInstance> parent_layer;
		/** the child layers */
		std::vector<shared_ptr<TMLayerInstance>> layer_instances;
	};

	// =====================================
	// TMTriggerCollisionInfo 
	// =====================================

	class TMTriggerCollisionInfo
	{
	public:

		/** search whether a trigger is in the collision list */
		bool FindTrigger(TMTrigger const * trigger) const;

	public:

		/** the target considered */
		weak_ptr<Object> object;
		/** all the triggers colliding */
		std::vector<weak_ptr<TMTrigger>> triggers;
	};

	// =====================================
	// TMLevelInstance : instance of a Level
	// =====================================

	class TMLevelInstance : public LevelInstance
	{
		CHAOS_GAMEPLAY_TM_ALL_FRIENDS;

		CHAOS_DECLARE_OBJECT_CLASS2(TMLevelInstance, LevelInstance);

	public:

		/** get the tiled map */
		TiledMap::Map* GetTiledMap();
		/** get the tiled map */
		TiledMap::Map const* GetTiledMap() const;

		/** find the layer instance from its ID */
		TMLayerInstance * FindLayerInstanceByID(int in_id, bool recursive = false);
		/** find the layer instance from its ID */
		TMLayerInstance const * FindLayerInstanceByID(int in_id, bool recursive = false) const;

		/** find the layer instance from its name */
		TMLayerInstance* FindLayerInstance(ObjectRequest request, bool recursive = false);
		/** find the layer instance from its name */
		TMLayerInstance const* FindLayerInstance(ObjectRequest request, bool recursive = false) const;

		/** find the typed object from its name */
		template<typename CHECK_CLASS = EmptyClass>
		AutoCastable<TMObject> FindObject(ObjectRequest request, bool recursive = false)
		{
			for (auto& layer : layer_instances)
				if (AutoCastable<TMObject> result = layer->FindObject<CHECK_CLASS>(request, recursive))
					return result;
			return nullptr;
		}
		/** find the typed object surface from its name */
		template<typename CHECK_CLASS = EmptyClass>
		AutoConstCastable<TMObject> FindObject(ObjectRequest request, bool recursive = false) const
		{
			for (auto const & layer : layer_instances)
				if (AutoConstCastable<TMObject> result = layer->FindObject<CHECK_CLASS>(request, recursive))
					return result;
			return nullptr;
		}
		/** find the object from its ID */
		template<typename CHECK_CLASS = EmptyClass>
		AutoCastable<TMObject> FindObjectByID(int id, bool recursive = false)
		{
			for (auto & layer : layer_instances)
				if (AutoCastable<TMObject> result = layer->FindObjectByID<CHECK_CLASS>(id, recursive))
					return result;
			return nullptr;
		}
		/** find the object from its ID */
		template<typename CHECK_CLASS = EmptyClass>
		AutoConstCastable<TMObject> FindObjectByID(int id, bool recursive = false) const
		{
			for (auto const& layer : layer_instances)
				if (AutoConstCastable<TMObject> result = layer->FindObjectByID<CHECK_CLASS>(id, recursive))
					return result;
			return nullptr;
		}

		/** create a particle spawner */
		template<typename ...PARAMS>
		ParticleSpawner* CreateParticleSpawner(ObjectRequest layer_instance_name, PARAMS... params)
		{
			LayerInstance* layer_instance = FindLayerInstance(layer_instance_name);
			if (layer_instance == nullptr)
				return nullptr;
			return layer_instance->CreateParticleSpawner(params...);
		}

		template<typename ...PARAMS>
		ParticleSpawner GetParticleSpawner(ObjectRequest layer_instance_name, PARAMS... params)
		{
			TMLayerInstance* layer_instance = FindLayerInstance(layer_instance_name);
			if (layer_instance == nullptr)
				return ParticleSpawner(nullptr);
			return layer_instance->GetParticleSpawner(params...);
		}

		/** get the bounding box for the level (in worls system obviously) */
		virtual box2 GetBoundingBox() const override;

		/** Get a collision iterator for tiles */
		TMTileCollisionIterator GetTileCollisionIterator(box2 const& in_collision_box, uint64_t in_collision_mask, bool in_open_geometry);
		/** Get a collision iterator for triggers */
		TMTriggerCollisionIterator GetTriggerCollisionIterator(box2 const& in_collision_box, uint64_t in_collision_mask, bool in_open_geometry);
		/** Get a collision iterator for objects */
		TMObjectCollisionIterator GetObjectCollisionIterator(box2 const& in_collision_box, uint64_t in_collision_mask, bool in_open_geometry);

		/** purge all collisions with object deleted */
		void PurgeCollisionInfo();
		/** handle all collision for a given object (TriggerObject) */
		void HandleTriggerCollisions(float delta_time, Object* object, box2 const& box, int mask);

		/** override */
		virtual bool SerializeFromJSON(nlohmann::json const& json) override;
		/** override */
		virtual bool SerializeIntoJSON(nlohmann::json& json) const override;

		/** get the child layer instances */
		std::vector<shared_ptr<TMLayerInstance>>& GetLayerInstances() { return layer_instances; }
		/** get the child layer instances */
		std::vector<shared_ptr<TMLayerInstance>> const& GetLayerInstances() const { return layer_instances; }

	protected:

		/** override */
		virtual bool Initialize(Game* in_game, Level* in_level) override;
		/** override */
		virtual bool DoTick(float delta_time) override;
		/** override */
		virtual int DoDisplay(GPURenderer* renderer, GPUProgramProviderBase const* uniform_provider, GPURenderParams const& render_params) override;
		/** override */
		virtual void OnRestart() override;

		/** initialize some internals */
		virtual bool InitializeLevelInstance(TMObjectReferenceSolver& reference_solver, TiledMap::PropertyOwner const* property_owner);

		/** handle all collisions with the player (TriggerObject) */
		void HandlePlayerTriggerCollisions(float delta_time);
		/** handle all collisions with the camera (TriggerObject) */
		void HandleCameraTriggerCollisions(float delta_time);

		/** override */
		virtual PlayerPawn * CreatePlayerPawn(Player* player) override;
		/** the sub function responsible for player pawn creation */
		virtual PlayerPawn * CreatePlayerPawnAtPlayerStart(Player* player, TMPlayerStart* player_start);

		/** get the player start used for an incomming player */
		virtual TMPlayerStart* GetPlayerStartForPawn(Player* player);

		/** override */
		virtual void OnLevelEnded() override;
		/** override */
		virtual void OnLevelStarted() override;

		/** create the cameras */
		virtual void CreateCameras() override;

		/** create the particle manager */
		virtual bool CreateParticleManager(Game* in_game);
		/** create the layers instances */
		virtual bool CreateLayerInstances(Game* in_game, TMObjectReferenceSolver &reference_solver);
		/** create the layers instances */
		bool DoCreateLayerInstances(std::vector<shared_ptr<TiledMap::LayerBase>> const& layers, TMObjectReferenceSolver& reference_solver);

		/** override */
		virtual void CreateBackgroundImage() override;
		/** override */
		virtual void SetInGameMusic() override;

		/** the default material when not specified */
		virtual GPURenderMaterial* GetDefaultRenderMaterial();

		/** find the collision info for an object */
		TMTriggerCollisionInfo* FindTriggerCollisionInfo(Object * object);

		/** search a collision flag from its name */
		virtual uint64_t GetCollisionFlagByName(char const* name) const;

	protected:

		/** the player start */
		weak_ptr<TMPlayerStart> player_start;
		/** the main camera */
		weak_ptr<TMCameraTemplate> main_camera;

		/** explicit bounding box (else it is dynamic with LayerInstance evaluation) */
		box2 explicit_bounding_box;
		/** whether the explicit_bounding_box is valid (empty is not a good answer) */
		bool has_explicit_bounding_box = false;

		/** the layer of reference for displacement */
		shared_ptr<TMLayerInstance> reference_layer;

		/** the particle manager used to render the world */
		shared_ptr<ParticleManager> particle_manager;
		/** the layers */
		std::vector<shared_ptr<TMLayerInstance>> layer_instances;
		/** the default render material */
		shared_ptr<GPURenderMaterial> default_material;

		/** the previous frame trigger collision */
		std::vector<TMTriggerCollisionInfo> collision_info;
	};


}; // namespace chaos

#endif // CHAOS_FORWARD_DECLARATION