namespace chaos
{
#if !defined CHAOS_FORWARD_DECLARATION && !defined CHAOS_TEMPLATE_IMPLEMENTATION

	// =====================================
	// TMTriggerCollisionInfo
	// =====================================

	class CHAOS_API TMTriggerCollisionInfo
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

	class CHAOS_API TMLevelInstance : public LevelInstance
	{
		CHAOS_GAMEPLAY_TM_ALL_FRIENDS;

		CHAOS_DECLARE_OBJECT_CLASS(TMLevelInstance, LevelInstance);

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
		ParticleSpawner* CreateParticleSpawner(ObjectRequest layer_instance_name, PARAMS && ...params)
		{
			TMLayerInstance* layer_instance = FindLayerInstance(layer_instance_name, true);
			if (layer_instance == nullptr)
				return nullptr;
			return layer_instance->CreateParticleSpawner(std::forward<PARAMS>(params)...);
		}

		template<typename ...PARAMS>
		ParticleSpawner GetParticleSpawner(ObjectRequest layer_instance_name, PARAMS && ...params)
		{
			TMLayerInstance* layer_instance = FindLayerInstance(layer_instance_name, true);
			if (layer_instance == nullptr)
				return {};
			return layer_instance->GetParticleSpawner(std::forward<PARAMS>(params)...);
		}

		/** get the bounding box for the level (in worls system obviously) */
		virtual box2 GetBoundingBox() const override;

		/** Get a collision iterator for tiles */
		TMTileCollisionIterator GetTileCollisionIterator(box2 const& in_collision_box, uint64_t in_collision_mask, bool in_open_geometry = false);
		/** Get a collision iterator for triggers */
		TMTriggerCollisionIterator GetTriggerCollisionIterator(box2 const& in_collision_box, uint64_t in_collision_mask, bool in_open_geometry = false);
		/** Get a collision iterator for objects */
		TMObjectCollisionIterator GetObjectCollisionIterator(box2 const& in_collision_box, uint64_t in_collision_mask, bool in_open_geometry = false);

		/** purge all collisions with object deleted */
		void PurgeCollisionInfo();
		/** handle all collision for a given object (TriggerObject) */
		void HandleTriggerCollisions(float delta_time, Object* object, box2 const& box, int mask);


		/** override */
		virtual bool SerializeFromJSON(JSONReadConfiguration config) override;
		/** override */
		virtual bool SerializeIntoJSON(nlohmann::json * json) const override;



		// shu49. Ces getters sont bizarres. On peut modifier les objects dedans ?

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
		virtual int DoDisplay(GPURenderer* renderer, GPUProgramProviderInterface const * uniform_provider, GPURenderParams const& render_params) override;
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

		/** create the main camera */
		virtual void CreateMainCamera() override;
		/** create the main camera components */
		virtual void CreateCameraComponents(Camera* camera, TMCameraTemplate* camera_template);

		/** create the layers instances */
		virtual bool CreateLayerInstances(Game* in_game, TMObjectReferenceSolver &reference_solver);
		/** create the layers instances */
		bool DoCreateLayerInstances(std::vector<shared_ptr<TiledMap::LayerBase>> const& layers, TMObjectReferenceSolver& reference_solver);

		/** override */
		virtual void CreateBackgroundImage() override;
		/** override */
		virtual void SetInGameMusic() override;

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

		/** the layers */
		std::vector<shared_ptr<TMLayerInstance>> layer_instances;
		/** the previous frame trigger collision */
		std::vector<TMTriggerCollisionInfo> collision_info;
	};

#endif

}; // namespace chaos

