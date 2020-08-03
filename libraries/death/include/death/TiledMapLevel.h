#pragma once

#include <death/Level.h>
#include <death/LevelInstance.h>
#include <death/TiledMapParticle.h>
#include <death/GameCheckpoint.h>
#include <death/GameFramework.h>

#include <chaos/StandardHeaders.h>
#include <chaos/GeometryFramework.h>
#include <chaos/Object.h>
#include <chaos/TiledMap.h>
#include <chaos/GPURenderable.h>
#include <chaos/Tickable.h>
#include <chaos/ParticleManager.h>
#include <chaos/ParticleDefault.h>
#include <chaos/BitmapAtlas.h>
#include <chaos/CollisionFramework.h>
#include <chaos/SoundManager.h>
#include <chaos/ParticleSpawner.h>
#include <chaos/NamedObject.h>
#include <chaos/JSONSerializable.h>

namespace death
{
	class Player; // for compilation, nested namespace cause issues

		// ==============================================================
		// FORWARD DECLARATION / FRIENDSHIP MACROS
		// ==============================================================

		// all classes in this file
#define DEATH_TILEDLEVEL_CLASSES \
(TiledMapLevel) \
(TiledMapLevelInstance) \
(TiledMapLayerInstance) \
(TiledMapObject) \
(TiledMapCameraTemplate) \
(TiledMapPlayerStart) \
(TiledMapTrigger) \
(TiledMapCheckpointTrigger) \
(TiledMapLayerInstanceParticlePopulator) \
(TiledMapTriggerCollisionIterator)\
(TiledMapObjectCollisionIterator)\
(TiledMapTileCollisionIterator)\
(TiledMapSoundTrigger)

		// forward declaration
#define DEATH_TILEDLEVEL_FORWARD_DECL(r, data, elem) class elem;
	BOOST_PP_SEQ_FOR_EACH(DEATH_TILEDLEVEL_FORWARD_DECL, _, DEATH_TILEDLEVEL_CLASSES)

		// friendship macro
#define DEATH_TILEDLEVEL_FRIEND_DECL(r, data, elem) friend class elem;
#define DEATH_TILEDLEVEL_ALL_FRIENDS BOOST_PP_SEQ_FOR_EACH(DEATH_TILEDLEVEL_FRIEND_DECL, _, DEATH_TILEDLEVEL_CLASSES)

		// =====================================
		// TiledMapObjectCheckpoint
		// =====================================

	class TiledMapObjectCheckpoint : public chaos::Object
	{
		DEATH_TILEDLEVEL_ALL_FRIENDS;

		CHAOS_OBJECT_DECLARE_CLASS2(TiledMapObjectCheckpoint, chaos::Object);

	public:

	};

	// =====================================
	// TiledMapObject 
	// =====================================

	class TiledMapObject : public chaos::Tickable, public CheckpointObject<TiledMapObjectCheckpoint>, public chaos::JSONSerializable
	{
		DEATH_TILEDLEVEL_ALL_FRIENDS;

		CHAOS_OBJECT_DECLARE_CLASS2(TiledMapObject, chaos::Tickable);

	public:

		/** get the layer instance owning this object */
		TiledMapLayerInstance* GetLayerInstance() { return layer_instance; }
		/** get the layer instance owning this object */
		TiledMapLayerInstance const* GetLayerInstance() const { return layer_instance; }

		/** whenever the level is being started */
		virtual void OnLevelStarted() {}
		/** whenever the Level is being stopped */
		virtual void OnLevelEnded() {}

		/** get the layer ID (used for Checkpoints) */
		int GetObjectID() const { return id; }

		/** get the object bounding box */
		chaos::box2 GetBoundingBox(bool world_system) const;

		/** override */
		virtual bool SerializeFromJSON(nlohmann::json const& json) override;
		/** override */
		virtual bool SerializeIntoJSON(nlohmann::json& json) const override;

	protected:

		/** additionnal initialization */
		virtual bool Initialize(TiledMapLayerInstance* in_layer_instance, chaos::TiledMap::GeometricObject* in_geometric_object);
		/** some member initialization */
		virtual void InitializeInternals();
		/** enable the creation of additionnal particles */
		virtual bool IsParticleCreationEnabled() const;

	protected:

		/** id of the object (comming from chaos::TiledMap) */
		int id = 0;

		/** the bounding box of the object */
		chaos::box2 bounding_box;

		/** a reference to the layer instance */
		TiledMapLayerInstance* layer_instance = nullptr;
	};

	// =====================================
	// TiledMapCameraTemplate : where the player may start
	// =====================================

	class TiledMapCameraTemplate : public TiledMapObject
	{
		DEATH_TILEDLEVEL_ALL_FRIENDS;

		CHAOS_OBJECT_DECLARE_CLASS2(TiledMapCameraTemplate, TiledMapObject);

	protected:

		/** override */
		virtual bool Initialize(TiledMapLayerInstance* in_layer_instance, chaos::TiledMap::GeometricObject* in_geometric_object) override;
	};

	// =====================================
	// TiledMapPlayerStart : where the player may start
	// =====================================

	class TiledMapPlayerStart : public TiledMapObject
	{
		DEATH_TILEDLEVEL_ALL_FRIENDS;

		CHAOS_OBJECT_DECLARE_CLASS2(TiledMapPlayerStart, TiledMapObject);

	protected:

		/** override */
		virtual bool Initialize(TiledMapLayerInstance* in_layer_instance, chaos::TiledMap::GeometricObject* in_geometric_object) override;
		/** override */
		virtual bool SerializeFromJSON(nlohmann::json const& json) override;
		/** override */
		virtual bool SerializeIntoJSON(nlohmann::json& json) const override;

	protected:

		/** the bitmap to use for this player */
		std::string bitmap_name;
	};

	// =====================================
	// TiledMapTrigger : an object player can collide with (for moment, rectangle)
	// =====================================

	class TiledMapTrigger : public TiledMapObject
	{
		DEATH_TILEDLEVEL_ALL_FRIENDS;

		CHAOS_OBJECT_DECLARE_CLASS2(TiledMapTrigger, TiledMapObject);

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
		virtual bool IsCollisionWith(chaos::box2 const& other_box, chaos::CollisionType collision_type) const;

		/** override */
		virtual bool SerializeFromJSON(nlohmann::json const& json) override;
		/** override */
		virtual bool SerializeIntoJSON(nlohmann::json& json) const override;

	protected:

		/** override */
		virtual bool Initialize(TiledMapLayerInstance* in_layer_instance, chaos::TiledMap::GeometricObject* in_geometric_object) override;
		/** override */
		virtual void InitializeInternals() override;
		/** override */
		virtual TiledMapObjectCheckpoint* DoCreateCheckpoint() const override;
		/** override */
		virtual bool DoSaveIntoCheckpoint(TiledMapObjectCheckpoint* checkpoint) const override;
		/** override */
		virtual bool DoLoadFromCheckpoint(TiledMapObjectCheckpoint const* checkpoint) override;

		/** called whenever a collision with object is detected (returns true, if collision is handled successfully (=> important for TriggerOnce) */
		virtual bool OnCollisionEvent(float delta_time, chaos::Object * object, chaos::CollisionType event_type);

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

	// =====================================
	// TiledMapTriggerCheckpoint
	// =====================================

	class TiledMapTriggerCheckpoint : public TiledMapObjectCheckpoint
	{
		DEATH_TILEDLEVEL_ALL_FRIENDS;

		CHAOS_OBJECT_DECLARE_CLASS2(TiledMapTriggerCheckpoint, TiledMapObjectCheckpoint);

	public:

		/** flag whether to object is enabled or not */
		bool enabled = true;
		/** flag whether to can only trigger once */
		bool trigger_once = false;
		/** number of time the trigger-enter event happened */
		bool enter_event_triggered = false;
	};

	// =================================================
	// TiledMapNotificationTrigger
	// =================================================

	class TiledMapNotificationTrigger : public TiledMapTrigger
	{
		DEATH_TILEDLEVEL_ALL_FRIENDS;

		CHAOS_OBJECT_DECLARE_CLASS2(TiledMapNotificationTrigger, TiledMapTrigger);

	public:

		/** override */
		virtual bool IsParticleCreationEnabled() const override;
		/** override */
		virtual bool SerializeFromJSON(nlohmann::json const& json) override;
		/** override */
		virtual bool SerializeIntoJSON(nlohmann::json& json) const override;

	protected:

		/** override */
		virtual bool Initialize(TiledMapLayerInstance* in_layer_instance, chaos::TiledMap::GeometricObject* in_geometric_object) override;
		/** override */
		virtual bool OnCollisionEvent(float delta_time, chaos::Object* object, chaos::CollisionType event_type) override;

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
	// TiledMapCheckpointTrigger
	// =================================================

	class TiledMapCheckpointTrigger : public TiledMapTrigger
	{
		DEATH_TILEDLEVEL_ALL_FRIENDS;

		CHAOS_OBJECT_DECLARE_CLASS2(TiledMapCheckpointTrigger, TiledMapTrigger);

	public:

		/** override */
		virtual bool IsParticleCreationEnabled() const override;

	protected:

		/** override */
		virtual bool Initialize(TiledMapLayerInstance* in_layer_instance, chaos::TiledMap::GeometricObject* in_geometric_object) override;
		/** override */
		virtual bool OnCollisionEvent(float delta_time, chaos::Object* object, chaos::CollisionType event_type) override;
	};

	// =================================================
	// TiledMapSoundTrigger : an object that play a sound
	// =================================================

	class TiledMapSoundTrigger : public TiledMapTrigger
	{
		DEATH_TILEDLEVEL_ALL_FRIENDS;

		CHAOS_OBJECT_DECLARE_CLASS2(TiledMapSoundTrigger, TiledMapTrigger);

	public:

		/** override */
		virtual bool IsParticleCreationEnabled() const override;
		/** override */
		virtual bool SerializeFromJSON(nlohmann::json const& json) override;
		/** override */
		virtual bool SerializeIntoJSON(nlohmann::json& json) const override;

	protected:

		/** override */
		virtual bool Initialize(TiledMapLayerInstance* in_layer_instance, chaos::TiledMap::GeometricObject* in_geometric_object) override;
		/** override */
		virtual void InitializeInternals() override;
		/** override */
		virtual bool OnCollisionEvent(float delta_time, chaos::Object* object, chaos::CollisionType event_type) override;

		/** the sound creation method */
		chaos::Sound* CreateSound() const;

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
		chaos::shared_ptr<chaos::Sound> sound;
	};


	// =================================================
	// TiledMapChangeLevelTrigger
	// =================================================

	class TiledMapChangeLevelTrigger : public TiledMapTrigger
	{
		DEATH_TILEDLEVEL_ALL_FRIENDS;

		CHAOS_OBJECT_DECLARE_CLASS2(TiledMapChangeLevelTrigger, TiledMapTrigger);

	public:

		/** override */
		virtual bool SerializeFromJSON(nlohmann::json const& json) override;
		/** override */
		virtual bool SerializeIntoJSON(nlohmann::json& json) const override;

	protected:

		/** override */
		virtual bool Initialize(TiledMapLayerInstance* in_layer_instance, chaos::TiledMap::GeometricObject* in_geometric_object) override;
		/** override */
		virtual bool IsParticleCreationEnabled() const override;
		/** override */
		virtual bool OnCollisionEvent(float delta_time, chaos::Object* object, chaos::CollisionType event_type) override;

	protected:

		/** the destination level */
		std::string level_name;
		/** the destination player start */
		std::string player_start_name;
	};




















	// =====================================
	// TiledMapLevel : utility
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
	// XXX : beware, the DEATH_MAKE_OBJECT_FACTORY(...) macro hide the complexity of lambda parameters capture. Use with caution 
	//

	/** a functor for geometric object factory */
	using TiledMapObjectFactory = std::function<TiledMapObject * (chaos::TiledMap::GeometricObject *)>;
	/** an helper to make a lambda inside DoGetObjectFactory */
#define DEATH_MAKE_OBJECT_FACTORY(x) [this, in_layer_instance](chaos::TiledMap::GeometricObject *in_geometric_object) { x }











	// =====================================
	// TiledMapLevel : a level described by a tiledmap
	// =====================================

	class TiledMapLevel : public Level
	{
		DEATH_TILEDLEVEL_ALL_FRIENDS;

		CHAOS_OBJECT_DECLARE_CLASS2(TiledMapLevel, Level);

	public:

		/** constructor */
		TiledMapLevel();

		/** initialization from tiled_map */
		virtual bool Initialize(chaos::TiledMap::Map* in_tiled_map);

		/** get the tiled map */
		chaos::TiledMap::Map* GetTiledMap() { return tiled_map.get(); }
		/** get the tiled map */
		chaos::TiledMap::Map const* GetTiledMap() const { return tiled_map.get(); }

	protected:

		/** create a typed object specializable method */
		virtual TiledMapObjectFactory DoGetObjectFactory(TiledMapLayerInstance* in_layer_instance, chaos::TiledMap::TypedObject * in_typed_object);
		/** create a typed object 'entry point' */
		TiledMapObjectFactory GetObjectFactory(TiledMapLayerInstance* in_layer_instance, chaos::TiledMap::TypedObject * in_typed_object);

		/** create a Camera specializable method */
		virtual TiledMapCameraTemplate* DoCreateCamera();
		/** create a PlayerStartObject specializable method */
		virtual TiledMapPlayerStart* DoCreatePlayerStart();
		/** create a FinishingTriggerObject specializable method */
		virtual TiledMapChangeLevelTrigger* DoCreateChangeLevelTrigger();
		/** create a CheckpointTriggerObject specializable method */
		virtual TiledMapCheckpointTrigger* DoCreateCheckpointTrigger();
		/** create a NotificationTriggerObject specializable method */
		virtual TiledMapNotificationTrigger* DoCreateNotificationTrigger();
		/** create a SoundTriggerObject specializable method */
		virtual TiledMapSoundTrigger* DoCreateSoundTrigger();

		/** create a PlayerStartObject specializable method */
		virtual TiledMapLayerInstance* DoCreateLayerInstance();

		/** create a layer instance 'entry point' */
		TiledMapLayerInstance* CreateLayerInstance(TiledMapLevelInstance* in_level_instance, chaos::TiledMap::LayerBase* in_layer);

		/** get the folder in which bitmaps are stored in Game::Atlas */
		virtual chaos::BitmapAtlas::FolderInfo const* GetFolderInfo(TiledMapLayerInstance* layer_instance) const;
		/** get the atlas to use for the rendering */
		virtual chaos::BitmapAtlas::TextureArrayAtlas const* GetTextureAtlas(TiledMapLayerInstance* layer_instance) const;
		/** create a particle layer */
		virtual chaos::ParticleLayerBase* DoCreateParticleLayer(TiledMapLayerInstance* layer_instance);

		/** called after all particles of a layers has been created, so we can plug additionnal data */
		virtual bool FinalizeLayerParticles(TiledMapLayerInstance* layer_instance, chaos::ParticleAllocationBase* allocation) { return true; }

		/** the default program when not specified */
		virtual chaos::GPUProgram* GenDefaultRenderProgram();
		/** the default material when not specified */
		virtual chaos::GPURenderMaterial* GetDefaultRenderMaterial();

	protected:

		/** the tiled map corresponding to this level */
		chaos::shared_ptr<chaos::TiledMap::Map> tiled_map;
	};

	// =====================================
	// TiledMapLayerCheckpoint
	// =====================================

	class TiledMapLayerCheckpoint : public chaos::Object
	{
		DEATH_TILEDLEVEL_ALL_FRIENDS;

		CHAOS_OBJECT_DECLARE_CLASS2(TiledMapLayerCheckpoint, chaos::Object);

	public:

		// the checkpoint per Object
		std::map<int, chaos::shared_ptr<TiledMapObjectCheckpoint>> trigger_checkpoints;
		// the checkpoint per Object
		std::map<int, chaos::shared_ptr<TiledMapObjectCheckpoint>> object_checkpoints;
	};
	














	// =====================================
	// TiledMapLayerInstance : instance of a Layer
	// =====================================

	class TiledMapLayerInstance : public chaos::GPURenderable, public CheckpointObject<TiledMapLayerCheckpoint>, public chaos::JSONSerializable
	{
		DEATH_TILEDLEVEL_ALL_FRIENDS;

		CHAOS_OBJECT_DECLARE_CLASS2(TiledMapLayerInstance, chaos::GPURenderable);

	public:

		/** get the tiled layer */
		chaos::TiledMap::LayerBase* GetTiledLayer() { return layer.get(); }
		/** get the tiled layer */
		chaos::TiledMap::LayerBase const* GetTiledLayer() const { return layer.get(); }

		/** get the level (for this layer) */
		chaos::AutoCastable<Level> GetLevel();
		/** get the level (for this layer) */
		chaos::AutoConstCastable<Level> GetLevel() const;

		/** get the level instance for this layer */
		chaos::AutoCastable<LevelInstance> GetLevelInstance();
		/** get the level instance for this layer */
		chaos::AutoConstCastable<LevelInstance> GetLevelInstance() const;

		/** get the game */
		chaos::AutoCastable<Game> GetGame();
		/** get the game */
		chaos::AutoConstCastable<Game> GetGame() const;

		/** find the player start from its name */
		TiledMapPlayerStart* FindPlayerStart(chaos::ObjectRequest request);
		/** find the player start from its name */
		TiledMapPlayerStart const* FindPlayerStart(chaos::ObjectRequest request) const;

		/** find the camera from its name */
		TiledMapCameraTemplate* FindCameraTemplate(chaos::ObjectRequest request);
		/** find the camera from its name */
		TiledMapCameraTemplate const* FindCameraTemplate(chaos::ObjectRequest request) const;

		/** find the trigger surface from its name */
		TiledMapTrigger* FindTrigger(chaos::ObjectRequest request);
		/** find the trigger surface from its name */
		TiledMapTrigger const* FindTrigger(chaos::ObjectRequest request) const;

		/** find the object from its name */
		TiledMapObject* FindObject(chaos::ObjectRequest request);
		/** find the object from its name */
		TiledMapObject const* FindObject(chaos::ObjectRequest request) const;

		/** get the bounding box for the level */
		chaos::box2 GetBoundingBox(bool world_system) const;

		/** create a particle spawner */
		template<typename ...PARAMS>
		chaos::ParticleSpawner* CreateParticleSpawner(PARAMS... params)
		{
			if (!CreateParticleLayer())
				return nullptr;
			return particle_layer->CreateParticleSpawner(params...);
		}

		template<typename ...PARAMS>
		chaos::ParticleSpawner GetParticleSpawner(PARAMS... params)
		{
			if (!CreateParticleLayer())
				return chaos::ParticleSpawner(nullptr);
			return particle_layer->GetParticleSpawner(params...);
		}

		/** create a particle allocation for the layer */
		chaos::ParticleAllocationBase* SpawnParticles(size_t count);
		/** create the particle layer if required */
		chaos::ParticleLayerBase* CreateParticleLayer();

		/** get the layer offset */
		glm::vec2 GetLayerOffset() const { return offset; }
		/** set the layer offset */
		void SetLayerOffset(glm::vec2 const& in_offset) { offset = in_offset; }

		/** get the particle layer */
		chaos::ParticleLayerBase* GetParticleLayer() { return particle_layer.get(); }
		/** get the particle layer */
		chaos::ParticleLayerBase const* GetParticleLayer() const { return particle_layer.get(); }

		/** returns the number of trigger surfaces */
		size_t GetTriggerCount() const;
		/** returns a trigger surface by its index */
		TiledMapTrigger* GetTrigger(size_t index);
		/** returns a trigger surface by its index */
		TiledMapTrigger const* GetTrigger(size_t index) const;

		/** returns the number of camera objects */
		size_t GetCameraTemplateCount() const;
		/** returns a camera object by its index */
		TiledMapCameraTemplate* GetCameraTemplate(size_t index);
		/** returns a camera object by its index */
		TiledMapCameraTemplate const* GetCameraTemplate(size_t index) const;

		/** returns the number of player start objects */
		size_t GetPlayerStartCount() const;
		/** returns a player start object by its index */
		TiledMapPlayerStart* GetPlayerStart(size_t index);
		/** returns a player start object by its index */
		TiledMapPlayerStart const* GetPlayerStart(size_t index) const;


		/** returns the number of objects */
		size_t GetObjectCount() const;
		/** returns an object by its index */
		TiledMapObject* GetObject(size_t index);
		/** returns an object by its index */
		TiledMapObject const* GetObject(size_t index) const;

		/** get the layer ID */
		int GetLayerID() const { return id; }

		/** override */
		virtual bool SerializeFromJSON(nlohmann::json const& json) override;
		/** override */
		virtual bool SerializeIntoJSON(nlohmann::json& json) const override;

	protected:

		/** initialization */
		virtual bool Initialize(TiledMapLevelInstance* in_level_instance, chaos::TiledMap::LayerBase* in_layer);

		/** override */
		virtual TiledMapLayerCheckpoint* DoCreateCheckpoint() const override;
		/** override */
		virtual bool DoSaveIntoCheckpoint(TiledMapLayerCheckpoint* checkpoint) const override;
		/** override */
		virtual bool DoLoadFromCheckpoint(TiledMapLayerCheckpoint const* checkpoint) override;

		/** find render material according to its name (or create the default) */
		chaos::GPURenderMaterial* FindOrCreateRenderMaterial(char const* material_name);

		/** override */
		virtual bool DoTick(float delta_time) override;
		/** override */
		virtual int DoDisplay(chaos::GPURenderer* renderer, chaos::GPUProgramProviderBase const* uniform_provider, chaos::GPURenderParams const& render_params) override;

		/** create a particle populator so that each layer may have its own particle type */
		virtual TiledMapLayerInstanceParticlePopulator* CreateParticlePopulator();

		/** specialized layer */
		bool InitializeImageLayer(chaos::TiledMap::ImageLayer* image_layer);
		/** specialized layer */
		bool InitializeObjectLayer(chaos::TiledMap::ObjectLayer* object_layer);
		/** specialized layer */
		bool InitializeTileLayer(chaos::TiledMap::TileLayer* tile_layer);
	
		/** create an object in an object layer */
		TiledMapObjectFactory GetObjectFactory(chaos::TiledMap::TypedObject * in_typed_object);

		/** create an object in an object layer */
		void CreateObjectParticles(chaos::TiledMap::GeometricObject* in_geometric_object, TiledMapObject* object, TiledMapLayerInstanceParticlePopulator* particle_populator);
		/** returns whether a particle should be created for object instance */
		bool ShouldCreateParticleForObject(chaos::TiledMap::PropertyOwner * property_owner, TiledMapObject* object) const;

		/** finalize the particles created */
		virtual bool FinalizeParticles(chaos::ParticleAllocationBase * allocation);
		/** try to search a name and a tag in the chaos::layer,  give them to the particle layer (and some other data as well) */
		virtual bool InitializeParticleLayer(chaos::ParticleLayerBase* in_particle_layer);

		/** utility methods for loading objects in layer instances */
		template<typename ELEMENT_VECTOR, typename CHECKPOINT_VECTOR>
		bool DoLoadFromCheckpointHelper(ELEMENT_VECTOR& elements, CHECKPOINT_VECTOR const& checkpoints);
		/** utility methods for saving objects in layer instances */
		template<typename ELEMENT_VECTOR, typename CHECKPOINT_VECTOR>
		bool DoSaveIntoCheckpointHelper(ELEMENT_VECTOR const& elements, CHECKPOINT_VECTOR& checkpoints) const;

		/** some callbacks */
		virtual void OnLevelEnded();
		/** some callbacks */
		virtual void OnLevelStarted();

		/** compute the collision mask from the formated string */
		void ComputeLayerCollisionMask(char const* mask);
		/** search a collision flag from its name */
		virtual uint64_t GetCollisionFlagByName(char const* name) const;


	protected:

		/** id of the object (comming from chaos::TiledMap) */
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
		TiledMapLevelInstance* level_instance = nullptr;

		/** the tiled layer corresponding to this object */
		chaos::shared_ptr<chaos::TiledMap::LayerBase> layer;
		/** the particle layer */
		chaos::shared_ptr<chaos::ParticleLayerBase> particle_layer;

		/** the player starts */
		std::vector<chaos::shared_ptr<TiledMapPlayerStart>> player_starts;
		/** the player cameras */
		std::vector<chaos::shared_ptr<TiledMapCameraTemplate>> camera_templates;
		/** the trigger surface */
		std::vector<chaos::shared_ptr<TiledMapTrigger>> triggers;
		/** the objects */
		std::vector<chaos::shared_ptr<TiledMapObject>> objects;

		/** the bounding box of the layer */
		chaos::box2 bounding_box;

		/** the collision mask for that layer */
		uint64_t collision_mask = 0;

		/** the current offset */
		glm::vec2 offset = glm::vec2(0.0f, 0.0f);
	};







	// =====================================
	// TiledMapTriggerCollisionInfo 
	// =====================================

	class TiledMapTriggerCollisionInfo
	{
	public:

		/** search whether a trigger is in the collision list */
		bool FindTrigger(TiledMapTrigger const * trigger) const;

	public:

		/** the target considered */
		chaos::weak_ptr<chaos::Object> object;
		/** all the triggers colliding */
		std::vector<chaos::weak_ptr<TiledMapTrigger>> triggers;
	};

	// =====================================
	// TiledMapLevelCheckpoint
	// =====================================

	class TiledMapLevelCheckpoint : public LevelCheckpoint
	{
		DEATH_TILEDLEVEL_ALL_FRIENDS;

		CHAOS_OBJECT_DECLARE_CLASS2(TiledMapLevelCheckpoint, LevelCheckpoint);

	public:

		// the checkpoint per LayerBase
		std::map<int, chaos::shared_ptr<TiledMapLayerCheckpoint>> layer_checkpoints;
	};

	// =====================================
	// TiledMapLevelInstance : instance of a Level
	// =====================================

	class TiledMapLevelInstance : public LevelInstance
	{
		DEATH_TILEDLEVEL_ALL_FRIENDS;

		CHAOS_OBJECT_DECLARE_CLASS2(TiledMapLevelInstance, LevelInstance);

	public:

		/** get the tiled map */
		chaos::TiledMap::Map* GetTiledMap();
		/** get the tiled map */
		chaos::TiledMap::Map const* GetTiledMap() const;

		/** find the layer instance from its name */
		TiledMapLayerInstance* FindLayerInstance(chaos::ObjectRequest request);
		/** find the layer instance from its name */
		TiledMapLayerInstance const* FindLayerInstance(chaos::ObjectRequest request) const;
		/** find the camera from its name */
		TiledMapCameraTemplate* FindCameraTemplate(chaos::ObjectRequest request);
		/** find the camera from its name */
		TiledMapCameraTemplate const* FindCameraTemplate(chaos::ObjectRequest request) const;
		/** find the player start from its name */
		TiledMapPlayerStart* FindPlayerStart(chaos::ObjectRequest request);
		/** find the player start from its name */
		TiledMapPlayerStart const* FindPlayerStart(chaos::ObjectRequest request) const;
		/** find the trigger surface from its name */
		TiledMapTrigger* FindTrigger(chaos::ObjectRequest request);
		/** find the trigger surface from its name */
		TiledMapTrigger const* FindTrigger(chaos::ObjectRequest request) const;
		/** find the typed object from its name */
		TiledMapObject* FindObject(chaos::ObjectRequest request);
		/** find the typed object surface from its name */
		TiledMapObject const* FindObject(chaos::ObjectRequest request) const;

		/** create a particle spawner */
		template<typename ...PARAMS>
		chaos::ParticleSpawner* CreateParticleSpawner(chaos::ObjectRequest layer_instance_name, PARAMS... params)
		{
			LayerInstance* layer_instance = FindLayerInstance(layer_instance_name);
			if (layer_instance == nullptr)
				return nullptr;
			return layer_instance->CreateParticleSpawner(params...);
		}

		template<typename ...PARAMS>
		chaos::ParticleSpawner GetParticleSpawner(chaos::ObjectRequest layer_instance_name, PARAMS... params)
		{
			TiledMapLayerInstance* layer_instance = FindLayerInstance(layer_instance_name);
			if (layer_instance == nullptr)
				return chaos::ParticleSpawner(nullptr);
			return layer_instance->GetParticleSpawner(params...);
		}

		/** get the bounding box for the level (in worls system obviously) */
		virtual chaos::box2 GetBoundingBox() const override;

		/** Get a collision iterator for tiles */
		TiledMapTileCollisionIterator GetTileCollisionIterator(chaos::box2 const& in_collision_box, uint64_t in_collision_mask);
		/** Get a collision iterator for triggers */
		TiledMapTriggerCollisionIterator GetTriggerCollisionIterator(chaos::box2 const& in_collision_box, uint64_t in_collision_mask);
		/** Get a collision iterator for objects */
		TiledMapObjectCollisionIterator GetObjectCollisionIterator(chaos::box2 const& in_collision_box, uint64_t in_collision_mask);

		/** purge all collisions with object deleted */
		void PurgeCollisionInfo();
		/** handle all collision for a given object (TriggerObject) */
		void HandleTriggerCollisions(float delta_time, chaos::Object* object, chaos::box2 const& box, int mask);

		/** override */
		virtual bool SerializeFromJSON(nlohmann::json const& json) override;
		/** override */
		virtual bool SerializeIntoJSON(nlohmann::json& json) const override;

	protected:

		/** override */
		virtual bool Initialize(Game* in_game, death::Level* in_level) override;
		/** override */
		virtual bool DoTick(float delta_time) override;
		/** override */
		virtual int DoDisplay(chaos::GPURenderer* renderer, chaos::GPUProgramProviderBase const* uniform_provider, chaos::GPURenderParams const& render_params) override;

		/** handle all collisions with the player (TriggerObject) */
		void HandlePlayerTriggerCollisions(float delta_time);
		/** handle all collisions with the camera (TriggerObject) */
		void HandleCameraTriggerCollisions(float delta_time);

		/** override */
		virtual PlayerPawn * CreatePlayerPawn(Player* player) override;
		/** the sub function responsible for player pawn creation */
		virtual PlayerPawn * CreatePlayerPawn(Player* player, TiledMapPlayerStart* player_start, TiledMapLayerInstance* layer_instance);

		/** get the player start used for an incomming player */
		virtual TiledMapPlayerStart* GetPlayerStartForPawn(Player* player);

		/** override */
		virtual void OnLevelEnded() override;
		/** override */
		virtual void OnLevelStarted() override;

		/** create the cameras */
		virtual void CreateCameras() override;

		/** create the particle manager */
		virtual bool CreateParticleManager(Game* in_game);
		/** create the layers instances */
		virtual bool CreateLayerInstances(Game* in_game);
		/** create the layers instances */
		bool DoCreateLayerInstances(std::vector<chaos::shared_ptr<chaos::TiledMap::LayerBase>> const& layers);

		/** override */
		virtual void CreateBackgroundImage() override;
		/** override */
		virtual void SetInGameMusic() override;

		/** the default material when not specified */
		virtual chaos::GPURenderMaterial* GetDefaultRenderMaterial();

		/** override */
		virtual LevelCheckpoint* DoCreateCheckpoint() const override;
		/** override */
		virtual bool DoSaveIntoCheckpoint(LevelCheckpoint* checkpoint) const override;
		/** override */
		virtual bool DoLoadFromCheckpoint(LevelCheckpoint const* checkpoint) override;


		/** find the collision info for an object */
		TiledMapTriggerCollisionInfo* FindTriggerCollisionInfo(chaos::Object * object);

	protected:

		/** explicit bounding box (else it is dynamic with LayerInstance evaluation) */
		chaos::box2 explicit_bounding_box;
		/** whether the explicit_bounding_box is valid (empty is not a good answer) */
		bool has_explicit_bounding_box = false;

		/** the layer of reference for displacement */
		chaos::shared_ptr<TiledMapLayerInstance> reference_layer;

		/** the particle manager used to render the world */
		chaos::shared_ptr<chaos::ParticleManager> particle_manager;
		/** the layers */
		std::vector<chaos::shared_ptr<TiledMapLayerInstance>> layer_instances;
		/** the default render material */
		chaos::shared_ptr<chaos::GPURenderMaterial> default_material;

		/** the previous frame trigger collision */
		std::vector<TiledMapTriggerCollisionInfo> collision_info;
	};












	// =====================================
	// TileCollisionInfo
	// =====================================

	class TileCollisionInfo
	{
	public:

		/** the layer instance concerned by this collision */
		TiledMapLayerInstance* layer_instance = nullptr;
		/** the allocation concerned by this collision */
		chaos::ParticleAllocationBase* allocation = nullptr;
		/** the particle which with the collision is detected */
		TiledMapParticle* particle = nullptr;
		/** some information about the tile information */
		chaos::TiledMap::TileInfo tile_info;
	};

	// =====================================
	// TiledMapCollisionIteratorBase
	// =====================================

	class TiledMapCollisionIteratorBase
	{

	public:

		/** the default constructor */
		TiledMapCollisionIteratorBase() = default;
		/** the copy constructor */
		TiledMapCollisionIteratorBase(TiledMapCollisionIteratorBase const& src) = default;
		/** the constructor with initialization */
		TiledMapCollisionIteratorBase(TiledMapLevelInstance* in_level_instance, chaos::box2 const& in_collision_box, uint64_t in_collision_mask);

		/** returns whether the iterator is still valid */
		operator bool() const;

	protected:

		/** the collision mask for iterating over layers */
		uint64_t collision_mask = 0;
		/** the collision box */
		chaos::box2 collision_box;

		/** the level instance of concern */
		TiledMapLevelInstance* level_instance = nullptr;
		/** index of the layer */
		size_t layer_instance_index = 0;
		/** whether the iterator finishs with the current layer */
		bool ignore_other_layers = false;
	};

	// =====================================
	// TiledMapTileCollisionIterator
	// =====================================

	class TiledMapTileCollisionIterator : public TiledMapCollisionIteratorBase
	{
	public:

		/** the default constructor */
		TiledMapTileCollisionIterator() = default;
		/** the copy constructor */
		TiledMapTileCollisionIterator(TiledMapTileCollisionIterator const& src) = default;
		/** the constructor with initialization */
		TiledMapTileCollisionIterator(TiledMapLevelInstance* in_level_instance, chaos::box2 const& in_collision_box, uint64_t in_collision_mask);

		/** next layer */
		void NextLayer();
		/** next allocation */
		void NextAllocation();
		/** next particle (i.e operator ++) */
		void Next();
		
		// indirection
		TileCollisionInfo const & operator *() const;
		// indirection
		TileCollisionInfo const * operator ->() const;
		// pre increment iterator
		TiledMapTileCollisionIterator& operator ++ ();
		// post increment iterator
		TiledMapTileCollisionIterator operator ++ (int i);

	protected:

		/** find the very first collision from given conditions */
		void FindFirstCollision();
		/** called to set the iterator at its end */
		void EndIterator();

	protected:

		/** allocation index in that layer */
		size_t allocation_index = 0;
		/** index of the particle in that layer */
		size_t particle_index = 0;

		/** the collision data */
		TileCollisionInfo cached_result;
	};

	// =====================================
	// TiledMapObjectCollisionIteratorBase
	// =====================================

	template<typename T>
	class TiledMapObjectCollisionIteratorBase : public TiledMapCollisionIteratorBase
	{
	public:

		/** the default constructor */
		TiledMapObjectCollisionIteratorBase() = default;
		/** the copy constructor */
		TiledMapObjectCollisionIteratorBase(TiledMapObjectCollisionIteratorBase const& src) = default;
		/** the constructor with initialization */
		TiledMapObjectCollisionIteratorBase(TiledMapLevelInstance* in_level_instance, chaos::box2 const& in_collision_box, uint64_t in_collision_mask) :
			TiledMapCollisionIteratorBase(in_level_instance, in_collision_box, in_collision_mask)
		{
		}
		// indirection
		T & operator *() const
		{
			assert(level_instance != nullptr); // end already reached. cannot indirect
			return *cached_result;
		}
		// indirection
		T * operator ->() const
		{
			assert(level_instance != nullptr); // end already reached. cannot indirect
			return cached_result;
		}

	protected:

		/** called to set the iterator at its end */
		void EndIterator()
		{
			level_instance = nullptr;
			layer_instance_index = 0;
			object_index = 0;
		}

	protected:

		/** index of the layer */
		size_t layer_instance_index = 0;
		/** trigger index in that layer */
		size_t object_index = 0;

		/** the current result of the research */
		T* cached_result = nullptr;
	};


	// =====================================
	// TiledMapTriggerCollisionIterator
	// =====================================

	class TiledMapTriggerCollisionIterator : public TiledMapObjectCollisionIteratorBase<TiledMapTrigger>
	{
	public:

		/** the default constructor */
		TiledMapTriggerCollisionIterator() = default;
		/** the copy constructor */
		TiledMapTriggerCollisionIterator(TiledMapTriggerCollisionIterator const& src) = default;
		/** the constructor with initialization */
		TiledMapTriggerCollisionIterator(TiledMapLevelInstance* in_level_instance, chaos::box2 const& in_collision_box, uint64_t in_collision_mask);

		// pre increment iterator
		TiledMapTriggerCollisionIterator& operator ++ ();
		// post increment iterator
		TiledMapTriggerCollisionIterator operator ++ (int i);

		/** go to next layer */
		void NextLayer();
		/** go to next element */
		void Next();

	protected:

		/** find the very first collision from given conditions */
		void FindFirstCollision();
	};

	// =====================================
	// TiledMapObjectCollisionIterator
	// =====================================

	class TiledMapObjectCollisionIterator : public TiledMapObjectCollisionIteratorBase<TiledMapObject>
	{
	public:

		/** the default constructor */
		TiledMapObjectCollisionIterator() = default;
		/** the copy constructor */
		TiledMapObjectCollisionIterator(TiledMapObjectCollisionIterator const& src) = default;
		/** the constructor with initialization */
		TiledMapObjectCollisionIterator(TiledMapLevelInstance* in_level_instance, chaos::box2 const& in_collision_box, uint64_t in_collision_mask);

		// pre increment iterator
		TiledMapObjectCollisionIterator& operator ++ ();
		// post increment iterator
		TiledMapObjectCollisionIterator operator ++ (int i);

		/** go to next layer */
		void NextLayer();
		/** go to next element */
		void Next();

	protected:

		/** find the very first collision from given conditions */
		void FindFirstCollision();
	};

		 // undefine macros
#undef DEATH_TILEDMAP_CLASSES
#undef DEATH_TILEDMAP_FORWARD_DECL
#undef DEATH_TILEDMAP_FRIEND_DECL
#undef DEATH_TILEDMAP_ALL_FRIENDS

}; // namespace death
