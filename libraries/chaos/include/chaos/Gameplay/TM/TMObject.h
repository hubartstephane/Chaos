namespace chaos
{
#if !defined CHAOS_FORWARD_DECLARATION && !defined CHAOS_TEMPLATE_IMPLEMENTATION

	// =====================================
	// TMObject
	// =====================================

	class CHAOS_API TMObject : public GameEntity
	{
		CHAOS_GAMEPLAY_TM_ALL_FRIENDS;

		CHAOS_DECLARE_OBJECT_CLASS(TMObject, GameEntity);

	public:

		/** get the layer instance owning this object */
		TMLayerInstance* GetLayerInstance() { return layer_instance; }
		/** get the layer instance owning this object */
		TMLayerInstance const* GetLayerInstance() const { return layer_instance; }

		/** whenever the level is being started */
		virtual void OnLevelStarted() {}
		/** whenever the Level is being stopped */
		virtual void OnLevelEnded() {}
		/** whenever the Level is being restarted */
		virtual void OnLevelRestart() {}

		/** get the layer ID (used for Checkpoints) */
		int GetObjectID() const { return id; }

		/** get the object bounding box */

		// shu49 ce n'est pas une bonne id�e mais bon


		virtual box2 GetBoundingBox(bool world_system) const;

		/** override */
		virtual bool SerializeFromJSON(JSONReadConfiguration config) override;
		/** override */
		virtual bool SerializeIntoJSON(nlohmann::json * json) const override;

	protected:

		/** additionnal initialization */
		virtual bool Initialize(TMLayerInstance* in_layer_instance, TiledMap::GeometricObject const* in_geometric_object, TMObjectReferenceSolver & reference_solver);
		/** enable the creation of additionnal particles */
		virtual bool IsParticleCreationEnabled() const;

	protected:

		/** id of the object (comming from TiledMap) */
		int id = 0;
		/** whether the particles created for this object should be under its ownership (instead of the layers) */
		bool particle_ownership = false;

		/** a reference to the layer instance */
		TMLayerInstance* layer_instance = nullptr;
	};

	// =====================================
	// TMPath : a path around which objects may evolve
	// =====================================

	class CHAOS_API TMPath : public TMObject
	{
		CHAOS_GAMEPLAY_TM_ALL_FRIENDS;

		CHAOS_DECLARE_OBJECT_CLASS(TMPath, TMObject);

	protected:

		/** override */
		virtual bool Initialize(TMLayerInstance* in_layer_instance, TiledMap::GeometricObject const* in_geometric_object, TMObjectReferenceSolver& reference_solver) override;
	};


	// =====================================
	// TMCameraTemplate : where the player may start
	// =====================================

	class CHAOS_API TMCameraTemplate : public TMObject
	{
		CHAOS_GAMEPLAY_TM_ALL_FRIENDS;

		CHAOS_DECLARE_OBJECT_CLASS(TMCameraTemplate, TMObject);

	protected:

		/** override */
		virtual bool Initialize(TMLayerInstance* in_layer_instance, TiledMap::GeometricObject const* in_geometric_object, TMObjectReferenceSolver& reference_solver) override;

	protected:

		/** the classes for camera components */
		std::vector<SubClassOf<CameraComponent>> camera_component_classes;
	};

	// =====================================
	// TMPlayerStart : where the player may start
	// =====================================

	class CHAOS_API TMPlayerStart : public TMObject
	{
		CHAOS_GAMEPLAY_TM_ALL_FRIENDS;

		CHAOS_DECLARE_OBJECT_CLASS(TMPlayerStart, TMObject);

	protected:

		/** override */
		virtual bool Initialize(TMLayerInstance* in_layer_instance, TiledMap::GeometricObject const* in_geometric_object, TMObjectReferenceSolver& reference_solver) override;
		/** override */
		virtual bool SerializeFromJSON(JSONReadConfiguration config) override;
		/** override */
		virtual bool SerializeIntoJSON(nlohmann::json * json) const override;

	protected:

		/** the bitmap to use for this player */
		std::string bitmap_name;
	};

	// =====================================
	// TMTrigger : an object player can collide with (for moment, rectangle)
	// =====================================

	class CHAOS_API TMTrigger : public TMObject
	{
		CHAOS_GAMEPLAY_TM_ALL_FRIENDS;

		CHAOS_DECLARE_OBJECT_CLASS(TMTrigger, TMObject);

	public:

		/** whether it is enabled or not */
		bool IsEnabled() const { return enabled; }
		/** change whether the trigger is enabled or not */
		void SetEnabled(bool in_enabled = true);

		/** whether it should be triggered a single time */
		bool IsTriggerOnce() const { return trigger_once; }
		/** change whether the trigger once is enabled or not */
		void SetTriggerOnce(bool in_trigger_once = true);

		/** search whether there is a collision given box */
		virtual bool IsCollisionWith(box2 const& other_box, CollisionType collision_type) const;

		/** override */
		virtual bool SerializeFromJSON(JSONReadConfiguration config) override;
		/** override */
		virtual bool SerializeIntoJSON(nlohmann::json * json) const override;

	protected:

		/** override */
		virtual bool Initialize(TMLayerInstance* in_layer_instance, TiledMap::GeometricObject const* in_geometric_object, TMObjectReferenceSolver& reference_solver) override;
		/** called whenever a collision with object is detected (returns true, if collision is handled successfully (=> important for TriggerOnce) */
		virtual bool OnCollisionEvent(float delta_time, Object* object, CollisionType event_type);

	protected:

		/** flag whether to object is enabled or not */
		bool enabled = true;
		/** flag whether to can only trigger once */
		bool trigger_once = false;
		/** outside box factor (a factor applyed to bounding box to detect whether the player is going outside of the range) */
		float outside_box_factor = 1.0f;

		/** whenever the trigger-enter event has happened */
		bool enter_event_triggered = false;
	};

	// =================================================
	// TMNotificationTrigger
	// =================================================

	class CHAOS_API TMNotificationTrigger : public TMTrigger
	{
		CHAOS_GAMEPLAY_TM_ALL_FRIENDS;

		CHAOS_DECLARE_OBJECT_CLASS(TMNotificationTrigger, TMTrigger);

	public:

		/** override */
		virtual bool IsParticleCreationEnabled() const override;
		/** override */
		virtual bool SerializeFromJSON(JSONReadConfiguration config) override;
		/** override */
		virtual bool SerializeIntoJSON(nlohmann::json * json) const override;

	protected:

		/** override */
		virtual bool Initialize(TMLayerInstance* in_layer_instance, TiledMap::GeometricObject const* in_geometric_object, TMObjectReferenceSolver& reference_solver) override;
		/** override */
		virtual bool OnCollisionEvent(float delta_time, Object* object, CollisionType event_type) override;

	protected:

		/** the notification to display */
		std::string notification_string;
		/** the notification lifetime */
		float notification_lifetime = 5.0f;
		/** destroy whenever collision is finished */
		bool stop_when_collision_over = false;
		/** collision with player instead of camera */
		bool player_collision = true;
	};

	// =================================================
	// TMCheckpointTrigger
	// =================================================

	class CHAOS_API TMCheckpointTrigger : public TMTrigger
	{
		CHAOS_GAMEPLAY_TM_ALL_FRIENDS;

		CHAOS_DECLARE_OBJECT_CLASS(TMCheckpointTrigger, TMTrigger);

	public:

		/** override */
		virtual bool IsParticleCreationEnabled() const override;

	protected:

		/** override */
		virtual bool Initialize(TMLayerInstance* in_layer_instance, TiledMap::GeometricObject const* in_geometric_object, TMObjectReferenceSolver& reference_solver) override;
		/** override */
		virtual bool OnCollisionEvent(float delta_time, Object* object, CollisionType event_type) override;
	};

	// =================================================
	// TMSoundTrigger : an object that play a sound
	// =================================================

	class CHAOS_API TMSoundTrigger : public TMTrigger
	{
		CHAOS_GAMEPLAY_TM_ALL_FRIENDS;

		CHAOS_DECLARE_OBJECT_CLASS(TMSoundTrigger, TMTrigger);

	public:

		/** override */
		virtual bool IsParticleCreationEnabled() const override;
		/** override */
		virtual bool SerializeFromJSON(JSONReadConfiguration config) override;
		/** override */
		virtual bool SerializeIntoJSON(nlohmann::json * json) const override;

	protected:

		/** override */
		virtual bool Initialize(TMLayerInstance* in_layer_instance, TiledMap::GeometricObject const* in_geometric_object, TMObjectReferenceSolver& reference_solver) override;
		/** override */
		virtual bool OnCollisionEvent(float delta_time, Object* object, CollisionType event_type) override;

		/** the sound creation method */
		Sound* CreateSound() const;

	protected:

		/** the name of the sound to play */
		std::string sound_name;

		/** a ratio applyed to max_distance to compute the min_distance of the sound (for 3D sounds) */
		float min_distance_ratio = 0.3f;
		/** 3D sound */
		bool is_3D_sound = false;
		/** whether the sound is to be looping or play only once */
		bool looping = true;
		/** timer for far 3D sound before entering pause */
		float pause_timer_when_too_far = 0.0f;
		/** 3D sound */
		bool stop_when_collision_over = false;

		/** the sound being played */
		shared_ptr<Sound> sound;
	};


	// =================================================
	// TMChangeLevelTrigger
	// =================================================

	class CHAOS_API TMChangeLevelTrigger : public TMTrigger
	{
		CHAOS_GAMEPLAY_TM_ALL_FRIENDS;

		CHAOS_DECLARE_OBJECT_CLASS(TMChangeLevelTrigger, TMTrigger);

	public:

		/** override */
		virtual bool SerializeFromJSON(JSONReadConfiguration config) override;
		/** override */
		virtual bool SerializeIntoJSON(nlohmann::json * json) const override;

	protected:

		/** override */
		virtual bool Initialize(TMLayerInstance* in_layer_instance, TiledMap::GeometricObject const* in_geometric_object, TMObjectReferenceSolver& reference_solver) override;
		/** override */
		virtual bool IsParticleCreationEnabled() const override;
		/** override */
		virtual bool OnCollisionEvent(float delta_time, Object* object, CollisionType event_type) override;

	protected:

		/** the destination level */
		std::string level_name;
		/** the destination player start */
		std::string player_start_name;
	};

#endif

}; // namespace chaos