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
		// ==========================================
		// PropertyOwnerOverride : an utility class to capture the properties of a source 
		// ==========================================

	template<typename T>
	class PropertyOwnerOverride : public T
	{
	public:

		/** constructor */
		PropertyOwnerOverride(chaos::TiledMap::BaseObject* in_owner, chaos::TiledMap::PropertyOwner const * in_property_owner) :
			T(in_owner),
			property_owner(in_property_owner) {}

		/** override */
		virtual chaos::TiledMap::Property const* FindProperty(char const* name, chaos::TiledMap::PropertyType type_id) const override
		{
			chaos::TiledMap::Property const* result = nullptr;
			if (property_owner != nullptr)
				result = property_owner->FindProperty(name, type_id);
			if (result == nullptr)
				result = T::FindProperty(name, type_id);
			return result;
		}

	protected:

		/** a substitute property owner to fake the system */
		chaos::TiledMap::PropertyOwner const * property_owner = nullptr;
	};

	// =====================================
	// TMObject 
	// =====================================

	class TMObject : public chaos::Tickable, public chaos::JSONSerializable
	{
		DEATH_TILEDLEVEL_ALL_FRIENDS;

		CHAOS_OBJECT_DECLARE_CLASS2(TMObject, chaos::Tickable);

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
		chaos::box2 GetBoundingBox(bool world_system) const;

		/** override */
		virtual bool SerializeFromJSON(nlohmann::json const& json) override;
		/** override */
		virtual bool SerializeIntoJSON(nlohmann::json& json) const override;

	protected:

		/** additionnal initialization */
		virtual bool Initialize(TMLayerInstance* in_layer_instance, chaos::TiledMap::GeometricObject const * in_geometric_object);
		/** enable the creation of additionnal particles */
		virtual bool IsParticleCreationEnabled() const;

	protected:

		/** id of the object (comming from chaos::TiledMap) */
		int id = 0;
		/** whether the particles created for this object should be under its ownership (instead of the layers) */
		bool particle_ownership = false;
		/** the bounding box of the object */
		chaos::box2 bounding_box;

		/** the allocations for this object */
		chaos::shared_ptr<chaos::ParticleAllocationBase> allocation;

		/** a reference to the layer instance */
		TMLayerInstance* layer_instance = nullptr;
	};

	// =====================================
	// TMCameraTemplate : where the player may start
	// =====================================

	class TMCameraTemplate : public TMObject
	{
		DEATH_TILEDLEVEL_ALL_FRIENDS;

		CHAOS_OBJECT_DECLARE_CLASS2(TMCameraTemplate, TMObject);

	protected:

		/** override */
		virtual bool Initialize(TMLayerInstance* in_layer_instance, chaos::TiledMap::GeometricObject const * in_geometric_object) override;
	};

	// =====================================
	// TMPlayerStart : where the player may start
	// =====================================

	class TMPlayerStart : public TMObject
	{
		DEATH_TILEDLEVEL_ALL_FRIENDS;

		CHAOS_OBJECT_DECLARE_CLASS2(TMPlayerStart, TMObject);

	protected:

		/** override */
		virtual bool Initialize(TMLayerInstance* in_layer_instance, chaos::TiledMap::GeometricObject const * in_geometric_object) override;
		/** override */
		virtual bool SerializeFromJSON(nlohmann::json const& json) override;
		/** override */
		virtual bool SerializeIntoJSON(nlohmann::json& json) const override;

	protected:

		/** the bitmap to use for this player */
		std::string bitmap_name;
	};

	// =====================================
	// TMTrigger : an object player can collide with (for moment, rectangle)
	// =====================================

	class TMTrigger : public TMObject
	{
		DEATH_TILEDLEVEL_ALL_FRIENDS;

		CHAOS_OBJECT_DECLARE_CLASS2(TMTrigger, TMObject);

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
		virtual bool Initialize(TMLayerInstance* in_layer_instance, chaos::TiledMap::GeometricObject const * in_geometric_object) override;
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

	// =================================================
	// TMNotificationTrigger
	// =================================================

	class TMNotificationTrigger : public TMTrigger
	{
		DEATH_TILEDLEVEL_ALL_FRIENDS;

		CHAOS_OBJECT_DECLARE_CLASS2(TMNotificationTrigger, TMTrigger);

	public:

		/** override */
		virtual bool IsParticleCreationEnabled() const override;
		/** override */
		virtual bool SerializeFromJSON(nlohmann::json const& json) override;
		/** override */
		virtual bool SerializeIntoJSON(nlohmann::json& json) const override;

	protected:

		/** override */
		virtual bool Initialize(TMLayerInstance* in_layer_instance, chaos::TiledMap::GeometricObject const * in_geometric_object) override;
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
	// TMCheckpointTrigger
	// =================================================

	class TMCheckpointTrigger : public TMTrigger
	{
		DEATH_TILEDLEVEL_ALL_FRIENDS;

		CHAOS_OBJECT_DECLARE_CLASS2(TMCheckpointTrigger, TMTrigger);

	public:

		/** override */
		virtual bool IsParticleCreationEnabled() const override;

	protected:

		/** override */
		virtual bool Initialize(TMLayerInstance* in_layer_instance, chaos::TiledMap::GeometricObject const * in_geometric_object) override;
		/** override */
		virtual bool OnCollisionEvent(float delta_time, chaos::Object* object, chaos::CollisionType event_type) override;
	};

	// =================================================
	// TMSoundTrigger : an object that play a sound
	// =================================================

	class TMSoundTrigger : public TMTrigger
	{
		DEATH_TILEDLEVEL_ALL_FRIENDS;

		CHAOS_OBJECT_DECLARE_CLASS2(TMSoundTrigger, TMTrigger);

	public:

		/** override */
		virtual bool IsParticleCreationEnabled() const override;
		/** override */
		virtual bool SerializeFromJSON(nlohmann::json const& json) override;
		/** override */
		virtual bool SerializeIntoJSON(nlohmann::json& json) const override;

	protected:

		/** override */
		virtual bool Initialize(TMLayerInstance* in_layer_instance, chaos::TiledMap::GeometricObject const * in_geometric_object) override;
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
	// TMChangeLevelTrigger
	// =================================================

	class TMChangeLevelTrigger : public TMTrigger
	{
		DEATH_TILEDLEVEL_ALL_FRIENDS;

		CHAOS_OBJECT_DECLARE_CLASS2(TMChangeLevelTrigger, TMTrigger);

	public:

		/** override */
		virtual bool SerializeFromJSON(nlohmann::json const& json) override;
		/** override */
		virtual bool SerializeIntoJSON(nlohmann::json& json) const override;

	protected:

		/** override */
		virtual bool Initialize(TMLayerInstance* in_layer_instance, chaos::TiledMap::GeometricObject const* in_geometric_object) override;
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
	// XXX : beware, the DEATH_MAKE_OBJECT_FACTORY(...) macro hide the complexity of lambda parameters capture. Use with caution 
	//

	/** a functor for geometric object factory */
	using TMObjectFactory = std::function<TMObject * (chaos::TiledMap::GeometricObject const *)>;
	/** an helper to make a lambda inside DoGetObjectFactory */
#define DEATH_MAKE_OBJECT_FACTORY(x) [this, in_layer_instance](chaos::TiledMap::GeometricObject const *in_geometric_object) { x }

	// =====================================
	// TMLevel : a level described by a tiledmap
	// =====================================

	class TMLevel : public Level
	{
		DEATH_TILEDLEVEL_ALL_FRIENDS;

		CHAOS_OBJECT_DECLARE_CLASS2(TMLevel, Level);

	public:

		/** constructor */
		TMLevel();

		/** initialization from tiled_map */
		virtual bool Initialize(chaos::TiledMap::Map * in_tiled_map);

		/** get the tiled map */
		chaos::TiledMap::Map* GetTiledMap() { return tiled_map.get(); }
		/** get the tiled map */
		chaos::TiledMap::Map const* GetTiledMap() const { return tiled_map.get(); }

	protected:

		/** called to flush some particles into a layer allocation */
		virtual bool FlushParticlesIntoAllocation(TMLayerInstance* layer_instance, chaos::ParticleAllocationBase* allocation, TiledMapParticle const* particles, size_t particle_count);

		/** create a typed object based of a 'classname' property */
		virtual TMObjectFactory DoGetExplicitObjectFactory(TMLayerInstance* in_layer_instance, chaos::TiledMap::TypedObject const * in_typed_object);
		/** create a typed object specializable method */
		virtual TMObjectFactory DoGetObjectFactory(TMLayerInstance* in_layer_instance, chaos::TiledMap::TypedObject const * in_typed_object);
		/** create a typed object 'entry point' */
		TMObjectFactory GetObjectFactory(TMLayerInstance* in_layer_instance, chaos::TiledMap::TypedObject const * in_typed_object);

		/** create a Camera specializable method */
		virtual TMCameraTemplate* DoCreateCamera();
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
		TMLayerInstance* CreateLayerInstance(TMLevelInstance* in_level_instance, chaos::TiledMap::LayerBase* in_layer);

		/** get the folder in which bitmaps are stored in Game::Atlas */
		virtual chaos::BitmapAtlas::FolderInfo const* GetFolderInfo(TMLayerInstance* layer_instance) const;
		/** get the atlas to use for the rendering */
		virtual chaos::BitmapAtlas::TextureArrayAtlas const* GetTextureAtlas(TMLayerInstance* layer_instance) const;
		/** create a particle layer */
		virtual chaos::ParticleLayerBase* DoCreateParticleLayer(TMLayerInstance* layer_instance);

		/** called after all particles of a layers has been created, so we can plug additionnal data */
		virtual bool FinalizeLayerParticles(TMLayerInstance* layer_instance, chaos::ParticleAllocationBase* allocation);

		/** the default material when not specified */
		virtual chaos::GPURenderMaterial* GenDefaultRenderMaterial();

	protected:

		/** the tiled map corresponding to this level */
		chaos::shared_ptr<chaos::TiledMap::Map> tiled_map;
	};

	// =====================================
	// TMLayerInstance : instance of a Layer
	// =====================================

	class TMLayerInstance : public chaos::GPURenderable, public chaos::JSONSerializable
	{
		DEATH_TILEDLEVEL_ALL_FRIENDS;

		CHAOS_OBJECT_DECLARE_CLASS2(TMLayerInstance, chaos::GPURenderable);

	public:

		/** get the tiled layer */
		chaos::TiledMap::LayerBase const* GetTiledLayer() const { return layer; }

		// shuclean : lien death:: => chaos (presque pas utilisé. pourrait sauter)
		//            juste le name












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
		TMPlayerStart* FindPlayerStart(chaos::ObjectRequest request);
		/** find the player start from its name */
		TMPlayerStart const* FindPlayerStart(chaos::ObjectRequest request) const;
		/** find the player start from its ID */
		TMPlayerStart* FindPlayerStartByID(int id);
		/** find the player start from its ID */
		TMPlayerStart const* FindPlayerStartByID(int id) const;

		/** find the camera from its name */
		TMCameraTemplate* FindCameraTemplate(chaos::ObjectRequest request);
		/** find the camera from its name */
		TMCameraTemplate const* FindCameraTemplate(chaos::ObjectRequest request) const;
		/** find the camera from its name */
		TMCameraTemplate* FindCameraTemplateByID(int id);
		/** find the camera from its name */
		TMCameraTemplate const* FindCameraTemplateByID(int id) const;

		/** find the trigger surface from its name */
		TMTrigger* FindTrigger(chaos::ObjectRequest request);
		/** find the trigger surface from its name */
		TMTrigger const* FindTrigger(chaos::ObjectRequest request) const;
		/** find the trigger surface from its ID */
		TMTrigger* FindTriggerByID(int id);
		/** find the trigger surface from its ID */
		TMTrigger const* FindTriggerByID(int id) const;

		/** find the object from its name */
		TMObject* FindObject(chaos::ObjectRequest request);
		/** find the object from its name */
		TMObject const* FindObject(chaos::ObjectRequest request) const;
		/** find the object from its ID */
		TMObject* FindObjectByID(int id);
		/** find the object from its ID */
		TMObject const* FindObjectByID(int id) const;

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
		chaos::ParticleAllocationBase* SpawnParticles(size_t count, bool new_allocation = true);

		/** spawn + user initialization methods */
		template<typename INIT_PARTICLE_FUNC>
		chaos::ParticleAllocationBase* SpawnParticles(size_t count, bool new_allocation, INIT_PARTICLE_FUNC init_func)
		{
			chaos::ParticleAllocationBase* result = SpawnParticles(count, new_allocation);
			// call user initialization function
			if (result != nullptr)
			{
				size_t allocation_count = result->GetParticleCount();
				init_func(result->GetParticleAccessor(allocation_count - count, count));  // partial accessor, take the last particles in the array
			}
			return result;
		}

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

		/** returns the number of objects */
		size_t GetObjectCount() const;
		/** returns an object by its index */
		TMObject* GetObject(size_t index);
		/** returns an object by its index */
		TMObject const* GetObject(size_t index) const;

		/** get the layer ID */
		int GetLayerID() const { return id; }

		/** override */
		virtual bool SerializeFromJSON(nlohmann::json const& json) override;
		/** override */
		virtual bool SerializeIntoJSON(nlohmann::json& json) const override;

	protected:

		/** initialization */
		virtual bool Initialize(TMLevelInstance* in_level_instance, chaos::TiledMap::LayerBase const * in_layer);
		/** serialization of all JSON objects into an array */
		virtual bool SerializeObjectListFromJSON(nlohmann::json const& json, char const* attribute_name, std::vector<chaos::shared_ptr<TMObject>>& result);
		/** called whenever level instance is restarted */
		virtual void OnRestart();

		/** find render material according to its name (or create the default) */
		chaos::GPURenderMaterial* FindOrCreateRenderMaterial(char const* material_name);

		/** override */
		virtual bool DoTick(float delta_time) override;
		/** override */
		virtual int DoDisplay(chaos::GPURenderer* renderer, chaos::GPUProgramProviderBase const* uniform_provider, chaos::GPURenderParams const& render_params) override;

		/** specialized layer */
		bool InitializeImageLayer(chaos::TiledMap::ImageLayer const * image_layer);
		/** specialized layer */
		bool InitializeObjectLayer(chaos::TiledMap::ObjectLayer const * object_layer);
		/** specialized layer */
		bool InitializeTileLayer(chaos::TiledMap::TileLayer const * tile_layer);
	
		/** create an object in an object layer */
		TMObjectFactory GetObjectFactory(chaos::TiledMap::TypedObject const * in_typed_object);

		/** create an object in an object layer */
		void CreateObjectParticles(chaos::TiledMap::GeometricObject const * in_geometric_object, TMObject* object, TMParticlePopulator& particle_populator);
		/** returns whether a particle should be created for object instance */
		bool ShouldCreateParticleForObject(chaos::TiledMap::PropertyOwner const * property_owner, TMObject* object) const;

		/** finalize the particles created */
		virtual bool FinalizeParticles(chaos::ParticleAllocationBase * allocation);
		/** try to search a name and a tag in the chaos::layer,  give them to the particle layer (and some other data as well) */
		virtual bool InitializeParticleLayer(chaos::ParticleLayerBase* in_particle_layer);

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
		TMLevelInstance* level_instance = nullptr;

		/** the tiled layer corresponding to this object */
		chaos::TiledMap::LayerBase const* layer = nullptr;
		/** the particle layer */
		chaos::shared_ptr<chaos::ParticleLayerBase> particle_layer;
		/** the objects */
		std::vector<chaos::shared_ptr<TMObject>> objects;

		/** the bounding box of the layer */
		chaos::box2 bounding_box;

		/** the collision mask for that layer */
		uint64_t collision_mask = 0;

		/** the current offset */
		glm::vec2 offset = glm::vec2(0.0f, 0.0f);
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
		chaos::weak_ptr<chaos::Object> object;
		/** all the triggers colliding */
		std::vector<chaos::weak_ptr<TMTrigger>> triggers;
	};

	// =====================================
	// TMLevelInstance : instance of a Level
	// =====================================

	class TMLevelInstance : public LevelInstance
	{
		DEATH_TILEDLEVEL_ALL_FRIENDS;

		CHAOS_OBJECT_DECLARE_CLASS2(TMLevelInstance, LevelInstance);

	public:

		/** get the tiled map */
		chaos::TiledMap::Map* GetTiledMap();
		/** get the tiled map */
		chaos::TiledMap::Map const* GetTiledMap() const;

		/** find the layer instance from its ID */
		TMLayerInstance * FindLayerInstanceByID(int id);
		/** find the layer instance from its ID */
		TMLayerInstance const * FindLayerInstanceByID(int id) const;

		/** find the layer instance from its name */
		TMLayerInstance* FindLayerInstance(chaos::ObjectRequest request);
		/** find the layer instance from its name */
		TMLayerInstance const* FindLayerInstance(chaos::ObjectRequest request) const;
		/** find the camera from its name */
		TMCameraTemplate* FindCameraTemplate(chaos::ObjectRequest request);
		/** find the camera from its name */
		TMCameraTemplate const* FindCameraTemplate(chaos::ObjectRequest request) const;
		/** find the player start from its name */
		TMPlayerStart* FindPlayerStart(chaos::ObjectRequest request);
		/** find the player start from its name */
		TMPlayerStart const* FindPlayerStart(chaos::ObjectRequest request) const;
		/** find the trigger surface from its name */
		TMTrigger* FindTrigger(chaos::ObjectRequest request);
		/** find the trigger surface from its name */
		TMTrigger const* FindTrigger(chaos::ObjectRequest request) const;
		/** find the typed object from its name */
		TMObject* FindObject(chaos::ObjectRequest request);
		/** find the typed object surface from its name */
		TMObject const* FindObject(chaos::ObjectRequest request) const;

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
			TMLayerInstance* layer_instance = FindLayerInstance(layer_instance_name);
			if (layer_instance == nullptr)
				return chaos::ParticleSpawner(nullptr);
			return layer_instance->GetParticleSpawner(params...);
		}

		/** get the bounding box for the level (in worls system obviously) */
		virtual chaos::box2 GetBoundingBox() const override;

		/** Get a collision iterator for tiles */
		TMTileCollisionIterator GetTileCollisionIterator(chaos::box2 const& in_collision_box, uint64_t in_collision_mask);
		/** Get a collision iterator for triggers */
		TMTriggerCollisionIterator GetTriggerCollisionIterator(chaos::box2 const& in_collision_box, uint64_t in_collision_mask);
		/** Get a collision iterator for objects */
		TMObjectCollisionIterator GetObjectCollisionIterator(chaos::box2 const& in_collision_box, uint64_t in_collision_mask);

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
		/** override */
		virtual void OnRestart() override;

		/** handle all collisions with the player (TriggerObject) */
		void HandlePlayerTriggerCollisions(float delta_time);
		/** handle all collisions with the camera (TriggerObject) */
		void HandleCameraTriggerCollisions(float delta_time);

		/** override */
		virtual PlayerPawn * CreatePlayerPawn(Player* player) override;
		/** the sub function responsible for player pawn creation */
		virtual PlayerPawn * CreatePlayerPawn(Player* player, TMPlayerStart* player_start, TMLayerInstance* layer_instance);

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
		virtual bool CreateLayerInstances(Game* in_game);
		/** create the layers instances */
		bool DoCreateLayerInstances(std::vector<chaos::shared_ptr<chaos::TiledMap::LayerBase>> const& layers);

		/** override */
		virtual void CreateBackgroundImage() override;
		/** override */
		virtual void SetInGameMusic() override;

		/** the default material when not specified */
		virtual chaos::GPURenderMaterial* GetDefaultRenderMaterial();

		/** find the collision info for an object */
		TMTriggerCollisionInfo* FindTriggerCollisionInfo(chaos::Object * object);

	protected:

		/** explicit bounding box (else it is dynamic with LayerInstance evaluation) */
		chaos::box2 explicit_bounding_box;
		/** whether the explicit_bounding_box is valid (empty is not a good answer) */
		bool has_explicit_bounding_box = false;

		/** the layer of reference for displacement */
		chaos::shared_ptr<TMLayerInstance> reference_layer;

		/** the particle manager used to render the world */
		chaos::shared_ptr<chaos::ParticleManager> particle_manager;
		/** the layers */
		std::vector<chaos::shared_ptr<TMLayerInstance>> layer_instances;
		/** the default render material */
		chaos::shared_ptr<chaos::GPURenderMaterial> default_material;

		/** the previous frame trigger collision */
		std::vector<TMTriggerCollisionInfo> collision_info;
	};












	// =====================================
	// TileCollisionInfo
	// =====================================

	class TileCollisionInfo
	{
	public:

		/** the layer instance concerned by this collision */
		TMLayerInstance* layer_instance = nullptr;
		/** the allocation concerned by this collision */
		chaos::ParticleAllocationBase* allocation = nullptr;
		/** the particle which with the collision is detected */
		TiledMapParticle* particle = nullptr;
		/** some information about the tile information */
		chaos::TiledMap::TileInfo tile_info;
	};

	// =====================================
	// TMCollisionIteratorBase
	// =====================================

	class TMCollisionIteratorBase
	{

	public:

		/** the default constructor */
		TMCollisionIteratorBase() = default;
		/** the copy constructor */
		TMCollisionIteratorBase(TMCollisionIteratorBase const& src) = default;
		/** the constructor with initialization */
		TMCollisionIteratorBase(TMLevelInstance* in_level_instance, chaos::box2 const& in_collision_box, uint64_t in_collision_mask);

		/** returns whether the iterator is still valid */
		operator bool() const;

	protected:

		/** the collision mask for iterating over layers */
		uint64_t collision_mask = 0;
		/** the collision box */
		chaos::box2 collision_box;

		/** the level instance of concern */
		TMLevelInstance* level_instance = nullptr;
		/** index of the layer */
		size_t layer_instance_index = 0;
		/** whether the iterator finishs with the current layer */
		bool ignore_other_layers = false;
	};

	// =====================================
	// TMTileCollisionIterator
	// =====================================

	class TMTileCollisionIterator : public TMCollisionIteratorBase
	{
	public:

		/** the default constructor */
		TMTileCollisionIterator() = default;
		/** the copy constructor */
		TMTileCollisionIterator(TMTileCollisionIterator const& src) = default;
		/** the constructor with initialization */
		TMTileCollisionIterator(TMLevelInstance* in_level_instance, chaos::box2 const& in_collision_box, uint64_t in_collision_mask);

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
		TMTileCollisionIterator& operator ++ ();
		// post increment iterator
		TMTileCollisionIterator operator ++ (int i);

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
	// TMObjectCollisionIteratorBase
	// =====================================

	template<typename T>
	class TMObjectCollisionIteratorBase : public TMCollisionIteratorBase
	{
	public:

		/** the default constructor */
		TMObjectCollisionIteratorBase() = default;
		/** the copy constructor */
		TMObjectCollisionIteratorBase(TMObjectCollisionIteratorBase const& src) = default;
		/** the constructor with initialization */
		TMObjectCollisionIteratorBase(TMLevelInstance* in_level_instance, chaos::box2 const& in_collision_box, uint64_t in_collision_mask) :
			TMCollisionIteratorBase(in_level_instance, in_collision_box, in_collision_mask)
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
	// TMTriggerCollisionIterator
	// =====================================

	class TMTriggerCollisionIterator : public TMObjectCollisionIteratorBase<TMTrigger>
	{
	public:

		/** the default constructor */
		TMTriggerCollisionIterator() = default;
		/** the copy constructor */
		TMTriggerCollisionIterator(TMTriggerCollisionIterator const& src) = default;
		/** the constructor with initialization */
		TMTriggerCollisionIterator(TMLevelInstance* in_level_instance, chaos::box2 const& in_collision_box, uint64_t in_collision_mask);

		// pre increment iterator
		TMTriggerCollisionIterator& operator ++ ();
		// post increment iterator
		TMTriggerCollisionIterator operator ++ (int i);

		/** go to next layer */
		void NextLayer();
		/** go to next element */
		void Next();

	protected:

		/** find the very first collision from given conditions */
		void FindFirstCollision();
	};

	// =====================================
	// TMObjectCollisionIterator
	// =====================================

	class TMObjectCollisionIterator : public TMObjectCollisionIteratorBase<TMObject>
	{
	public:

		/** the default constructor */
		TMObjectCollisionIterator() = default;
		/** the copy constructor */
		TMObjectCollisionIterator(TMObjectCollisionIterator const& src) = default;
		/** the constructor with initialization */
		TMObjectCollisionIterator(TMLevelInstance* in_level_instance, chaos::box2 const& in_collision_box, uint64_t in_collision_mask);

		// pre increment iterator
		TMObjectCollisionIterator& operator ++ ();
		// post increment iterator
		TMObjectCollisionIterator operator ++ (int i);

		/** go to next layer */
		void NextLayer();
		/** go to next element */
		void Next();

	protected:

		/** find the very first collision from given conditions */
		void FindFirstCollision();
	};

}; // namespace death
