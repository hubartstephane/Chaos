#pragma once

#include <death/Level.h>
#include <death/LevelInstance.h>
#include <death/TiledMapParticle.h>
#include <death/GameCheckpoint.h>
#include <death/GameFramework.h>

#include <chaos/StandardHeaders.h>
#include <chaos/GeometryFramework.h>
#include <chaos/ReferencedObject.h>
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
(TiledMapGeometricObject) \
(TiledMapCameraObject) \
(TiledMapPlayerStartObject) \
(TiledMapTriggerObject) \
(TiledMapCheckpointTriggerObject) \
(TiledMapObject) \
(TiledMapLayerInstanceParticlePopulator) \
(TiledMapTriggerCollisionIterator)\
(TiledMapTileCollisionIterator)\
(TiledMapSoundTriggerObject)

		// forward declaration
#define DEATH_TILEDLEVEL_FORWARD_DECL(r, data, elem) class elem;
	BOOST_PP_SEQ_FOR_EACH(DEATH_TILEDLEVEL_FORWARD_DECL, _, DEATH_TILEDLEVEL_CLASSES)

		// friendship macro
#define DEATH_TILEDLEVEL_FRIEND_DECL(r, data, elem) friend class elem;
#define DEATH_TILEDLEVEL_ALL_FRIENDS BOOST_PP_SEQ_FOR_EACH(DEATH_TILEDLEVEL_FRIEND_DECL, _, DEATH_TILEDLEVEL_CLASSES)

		// =====================================
		// TiledMapObjectCheckpoint
		// =====================================

	class TiledMapObjectCheckpoint : public chaos::ReferencedObject
	{
		DEATH_TILEDLEVEL_ALL_FRIENDS

	public:

	};

	// =====================================
	// TiledMapObject : a base object for special game entities
	// =====================================

	class TiledMapObject : public chaos::Tickable, public CheckpointObject<TiledMapObjectCheckpoint>
	{
		DEATH_TILEDLEVEL_ALL_FRIENDS

	public:

		/** constructor */
		TiledMapObject(TiledMapLayerInstance* in_layer_instance);

		/** whether the object is modified */
		bool IsModified() const { return modified; }
		/** raise the modified flag */
		void SetModified(bool in_modified = true) { modified = in_modified; }

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

	protected:

		/** a reference to the layer instance */
		TiledMapLayerInstance* layer_instance = nullptr;
		/** whether the object has been modified from the JSON base data (usefull for checkpoint serialization) */
		bool modified = false;
		/** id of the object (comming from chaos::TiledMap) */
		int id = 0;
	};

	// =====================================
	// TiledMapGeometricObject 
	// =====================================

	class TiledMapGeometricObject : public TiledMapObject
	{
		DEATH_TILEDLEVEL_ALL_FRIENDS

	public:

		/** constructor */
		TiledMapGeometricObject(TiledMapLayerInstance* in_layer_instance);

		/** get the object bounding box */
		chaos::box2 GetBoundingBox(bool world_system) const;
		/** getters on the chaos::GeometricObject that this instance references to */
		chaos::TiledMap::GeometricObject* GetGeometricObject() { return geometric_object.get(); }
		/** getters on the chaos::GeometricObject that this instance references to */
		chaos::TiledMap::GeometricObject const * GetGeometricObject() const { return geometric_object.get(); }

		/** whether the object is to forced be serialized */
		bool IsForcedSerialization() const { return forced_serialization; }
		/** raise the forced serialization flag */
		void SetForcedSerialization(bool in_forced_serialization = true) { forced_serialization = in_forced_serialization; }

	protected:

		/** additionnal initialization */
		virtual bool Initialize(chaos::TiledMap::GeometricObject* in_geometric_object);
		/** enable the creation of additionnal particles */
		virtual bool IsParticleCreationEnabled() const;

	protected:

		/** the bounding box of the object */
		chaos::box2 bounding_box;
		/** a reference to the geometric object (chaos point of view) that was used for initialization */
		chaos::shared_ptr<chaos::TiledMap::GeometricObject> geometric_object;
		/** whether the object is forced to be serialized */
		bool forced_serialization = false;
	};

	// =====================================
	// TiledMapCameraObject : where the player may start
	// =====================================

	class TiledMapCameraObject : public TiledMapGeometricObject
	{
		DEATH_TILEDLEVEL_ALL_FRIENDS

	public:

		/** inherit constructor */
		using TiledMapGeometricObject::TiledMapGeometricObject;

	protected:

		/** override */
		virtual bool Initialize(chaos::TiledMap::GeometricObject* in_geometric_object) override;
	};

	// =====================================
	// TiledMapPlayerStartObject : where the player may start
	// =====================================

	class TiledMapPlayerStartObject : public TiledMapGeometricObject
	{
		DEATH_TILEDLEVEL_ALL_FRIENDS

	public:

		/** inherit constructor */
		using TiledMapGeometricObject::TiledMapGeometricObject;

	protected:

		/** override */
		virtual bool Initialize(chaos::TiledMap::GeometricObject* in_geometric_object) override;

	protected:

		/** the bitmap to use for this player */
		std::string bitmap_name;
	};

	// =====================================
	// TiledMapTriggerObject : an object player can collide with (for moment, rectangle)
	// =====================================

	class TiledMapTriggerObject : public TiledMapGeometricObject
	{
		DEATH_TILEDLEVEL_ALL_FRIENDS

	public:

		/** constructor */
		TiledMapTriggerObject(TiledMapLayerInstance* in_layer_instance);

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

	protected:

		/** override */
		virtual bool Initialize(chaos::TiledMap::GeometricObject* in_geometric_object) override;

		/** override */
		virtual TiledMapObjectCheckpoint* DoCreateCheckpoint() const override;
		/** override */
		virtual bool DoSaveIntoCheckpoint(TiledMapObjectCheckpoint* checkpoint) const override;
		/** override */
		virtual bool DoLoadFromCheckpoint(TiledMapObjectCheckpoint const* checkpoint) override;

		/** called whenever a collision with object is detected (returns true, if collision is handled successfully (=> important for TriggerOnce) */
		virtual bool OnCollisionEvent(float delta_time, chaos::ReferencedObject * object, chaos::CollisionType event_type);

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
	// TiledMapTriggerObjectCheckpoint
	// =====================================

	class TiledMapTriggerObjectCheckpoint : public TiledMapObjectCheckpoint
	{
		DEATH_TILEDLEVEL_ALL_FRIENDS

	public:

		/** flag whether to object is enabled or not */
		bool enabled = true;
		/** flag whether to can only trigger once */
		bool trigger_once = false;
		/** number of time the trigger-enter event happened */
		bool enter_event_triggered = false;
	};

	// =================================================
	// TiledMapNotificationTriggerObject
	// =================================================

	class TiledMapNotificationTriggerObject : public TiledMapTriggerObject
	{
		DEATH_TILEDLEVEL_ALL_FRIENDS

	public:

		/** constructor */
		using TiledMapTriggerObject::TiledMapTriggerObject;
		/** override */
		virtual bool IsParticleCreationEnabled() const override;
		/** override */
		virtual bool Initialize(chaos::TiledMap::GeometricObject* in_geometric_object) override;

	protected:

		/** override */
		virtual bool OnCollisionEvent(float delta_time, chaos::ReferencedObject* object, chaos::CollisionType event_type) override;

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
	// TiledMapCheckpointTriggerObject
	// =================================================

	class TiledMapCheckpointTriggerObject : public TiledMapTriggerObject
	{
		DEATH_TILEDLEVEL_ALL_FRIENDS

	public:

		/** constructor */
		using TiledMapTriggerObject::TiledMapTriggerObject;
		/** override */
		virtual bool IsParticleCreationEnabled() const override;
		/** override */
		virtual bool Initialize(chaos::TiledMap::GeometricObject* in_geometric_object) override;

	protected:

		/** override */
		virtual bool OnCollisionEvent(float delta_time, chaos::ReferencedObject* object, chaos::CollisionType event_type) override;
	};

	// =================================================
	// TiledMapSoundTriggerObject : an object that play a sound
	// =================================================

	class TiledMapSoundTriggerObject : public TiledMapTriggerObject
	{
		DEATH_TILEDLEVEL_ALL_FRIENDS

	public:

		/** constructor */
		using TiledMapTriggerObject::TiledMapTriggerObject;
		/** override */
		virtual bool IsParticleCreationEnabled() const override;

	protected:

		/** override */
		virtual bool Initialize(chaos::TiledMap::GeometricObject* in_geometric_object) override;
		/** override */
		virtual bool OnCollisionEvent(float delta_time, chaos::ReferencedObject* object, chaos::CollisionType event_type) override;

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
	// TiledMapFinishingTriggerObject
	// =================================================

	class TiledMapFinishingTriggerObject : public TiledMapTriggerObject
	{
		DEATH_TILEDLEVEL_ALL_FRIENDS

	public:

		/** inherit constructor */
		using TiledMapTriggerObject::TiledMapTriggerObject;

	protected:

		/** override */
		virtual bool IsParticleCreationEnabled() const override;
		/** override */
		virtual bool OnCollisionEvent(float delta_time, chaos::ReferencedObject* object, chaos::CollisionType event_type) override;
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
	// XXX : beware, the DEATH_MAKE_GEOMETRICOBJECT_FACTORY(...) macro hide the complexity of lambda parameters capture. Use with caution 
	//

	/** a functor for geometric object factory */
	using GeometricObjectFactory = std::function<TiledMapGeometricObject * (chaos::TiledMap::GeometricObject *)>;
	/** an helper to make a lambda inside DoGetGeometricObjectFactory */
#define DEATH_MAKE_GEOMETRICOBJECT_FACTORY(x) [this, in_layer_instance](chaos::TiledMap::GeometricObject *in_geometric_object) { x }

	// =====================================
	// TiledMapLevel : a level described by a tiledmap
	// =====================================

	class TiledMapLevel : public Level
	{
		DEATH_TILEDLEVEL_ALL_FRIENDS

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

		/** create a level instance for that level user specified function */
		virtual LevelInstance* DoCreateLevelInstance(Game* in_game) override;


		/** create a typed object specializable method */
		virtual GeometricObjectFactory DoGetGeometricObjectFactory(TiledMapLayerInstance* in_layer_instance, chaos::TiledMap::TypedObject * in_typed_object);
		/** create a typed object 'entry point' */
		GeometricObjectFactory GetGeometricObjectFactory(TiledMapLayerInstance* in_layer_instance, chaos::TiledMap::TypedObject * in_typed_object);

		/** create a Camera specializable method */
		virtual TiledMapCameraObject* DoCreateCameraObject(TiledMapLayerInstance* in_layer_instance, chaos::TiledMap::GeometricObject* in_geometric_object);
		/** create a PlayerStartObject specializable method */
		virtual TiledMapPlayerStartObject* DoCreatePlayerStartObject(TiledMapLayerInstance* in_layer_instance, chaos::TiledMap::GeometricObject* in_geometric_object);
		/** create a FinishingTriggerObject specializable method */
		virtual TiledMapFinishingTriggerObject* DoCreateFinishingTriggerObject(TiledMapLayerInstance* in_layer_instance, chaos::TiledMap::GeometricObject* in_geometric_object);
		/** create a CheckpointTriggerObject specializable method */
		virtual TiledMapCheckpointTriggerObject* DoCreateCheckpointTriggerObject(TiledMapLayerInstance* in_layer_instance, chaos::TiledMap::GeometricObject* in_geometric_object);
		/** create a NotificationTriggerObject specializable method */
		virtual TiledMapNotificationTriggerObject* DoCreateNotificationTriggerObject(TiledMapLayerInstance* in_layer_instance, chaos::TiledMap::GeometricObject* in_geometric_object);
		/** create a SoundTriggerObject specializable method */
		virtual TiledMapSoundTriggerObject* DoCreateSoundTriggerObject(TiledMapLayerInstance* in_layer_instance, chaos::TiledMap::GeometricObject* in_geometric_object);

		/** create a PlayerStartObject specializable method */
		virtual TiledMapLayerInstance* DoCreateLayerInstance(TiledMapLevelInstance* in_level_instance, chaos::TiledMap::LayerBase* in_layer);

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

	class TiledMapLayerCheckpoint : public chaos::ReferencedObject
	{
		DEATH_TILEDLEVEL_ALL_FRIENDS

	public:

		// the checkpoint per Object
		std::map<int, chaos::shared_ptr<TiledMapObjectCheckpoint>> trigger_checkpoints;
		// the checkpoint per Object
		std::map<int, chaos::shared_ptr<TiledMapObjectCheckpoint>> object_checkpoints;
	};
	
	// =====================================
	// TiledMapLayerInstance : instance of a Layer
	// =====================================

	class TiledMapLayerInstance : public chaos::GPURenderable, public CheckpointObject<TiledMapLayerCheckpoint>
	{
		DEATH_TILEDLEVEL_ALL_FRIENDS

	protected:

		/** protected constructor */
		TiledMapLayerInstance(TiledMapLevelInstance* in_level_instance, chaos::TiledMap::LayerBase* in_layer);

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
		TiledMapPlayerStartObject* FindPlayerStartObject(chaos::NamedObjectRequest request);
		/** find the player start from its name */
		TiledMapPlayerStartObject const* FindPlayerStartObject(chaos::NamedObjectRequest request) const;

		/** find the camera from its name */
		TiledMapCameraObject* FindCameraObject(chaos::NamedObjectRequest request);
		/** find the camera from its name */
		TiledMapCameraObject const* FindCameraObject(chaos::NamedObjectRequest request) const;

		/** find the trigger surface from its name */
		TiledMapTriggerObject* FindTriggerObject(chaos::NamedObjectRequest request);
		/** find the trigger surface from its name */
		TiledMapTriggerObject const* FindTriggerObject(chaos::NamedObjectRequest request) const;

		/** find the geometric object from its name */
		TiledMapGeometricObject* FindGeometricObject(chaos::NamedObjectRequest request);
		/** find the geometric object surface from its name */
		TiledMapGeometricObject const* FindGeometricObject(chaos::NamedObjectRequest request) const;

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
		TiledMapTriggerObject* GetTrigger(size_t index);
		/** returns a trigger surface by its index */
		TiledMapTriggerObject const* GetTrigger(size_t index) const;

		/** returns the number of camera objects */
		size_t GetCameraObjectCount() const;
		/** returns a camera object by its index */
		TiledMapCameraObject* GetCameraObject(size_t index);
		/** returns a camera object by its index */
		TiledMapCameraObject const* GetCameraObject(size_t index) const;

		/** returns the number of player start objects */
		size_t GetPlayerStartObjectCount() const;
		/** returns a player start object by its index */
		TiledMapPlayerStartObject* GetPlayerStartObject(size_t index);
		/** returns a player start object by its index */
		TiledMapPlayerStartObject const* GetPlayerStartObject(size_t index) const;


		/** returns the number of geometric objects */
		size_t GetGeometricObjectCount() const;
		/** returns a geometric object by its index */
		TiledMapGeometricObject* GetGeometricObject(size_t index);
		/** returns a geometric object by its index */
		TiledMapGeometricObject const* GetGeometricObject(size_t index) const;

		/** get the layer ID */
		int GetLayerID() const { return id; }

	protected:

		/** initialization */
		virtual bool Initialize();

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
		GeometricObjectFactory GetGeometricObjectFactory(chaos::TiledMap::TypedObject * in_typed_object);

		/** create an object in an object layer */
		void CreateGeometricObjectParticles(chaos::TiledMap::GeometricObject* geometric_object, TiledMapGeometricObject* object, TiledMapLayerInstanceParticlePopulator* particle_populator);
		/** returns whether a particle should be created for object instance */
		bool ShouldCreateParticleForObject(chaos::TiledMap::PropertyOwner * property_owner, TiledMapGeometricObject* object) const;

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
		std::vector<chaos::shared_ptr<TiledMapPlayerStartObject>> player_start_objects;
		/** the player cameras */
		std::vector<chaos::shared_ptr<TiledMapCameraObject>> camera_objects;
		/** the trigger surface */
		std::vector<chaos::shared_ptr<TiledMapTriggerObject>> trigger_objects;
		/** the geometric objects */
		std::vector<chaos::shared_ptr<TiledMapGeometricObject>> geometric_objects;

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
		bool FindTrigger(TiledMapTriggerObject const * trigger) const;

	public:

		/** the target considered */
		chaos::weak_ptr<chaos::ReferencedObject> object;
		/** all the triggers colliding */
		std::vector<chaos::weak_ptr<TiledMapTriggerObject>> triggers;
	};

	// =====================================
	// TiledMapLevelCheckpoint
	// =====================================

	class TiledMapLevelCheckpoint : public LevelCheckpoint
	{
		DEATH_TILEDLEVEL_ALL_FRIENDS

	public:

		// the checkpoint per LayerBase
		std::map<int, chaos::shared_ptr<TiledMapLayerCheckpoint>> layer_checkpoints;
	};

	// =====================================
	// TiledMapLevelInstance : instance of a Level
	// =====================================

	class TiledMapLevelInstance : public LevelInstance
	{
		DEATH_TILEDLEVEL_ALL_FRIENDS

	public:

		/** get the tiled map */
		chaos::TiledMap::Map* GetTiledMap();
		/** get the tiled map */
		chaos::TiledMap::Map const* GetTiledMap() const;

		/** find the layer instance from its name */
		TiledMapLayerInstance* FindLayerInstance(chaos::NamedObjectRequest request);
		/** find the layer instance from its name */
		TiledMapLayerInstance const* FindLayerInstance(chaos::NamedObjectRequest request) const;
		/** find the camera from its name */
		TiledMapCameraObject* FindCameraObject(chaos::NamedObjectRequest request);
		/** find the camera from its name */
		TiledMapCameraObject const* FindCameraObject(chaos::NamedObjectRequest request) const;
		/** find the player start from its name */
		TiledMapPlayerStartObject* FindPlayerStartObject(chaos::NamedObjectRequest request);
		/** find the player start from its name */
		TiledMapPlayerStartObject const* FindPlayerStartObject(chaos::NamedObjectRequest request) const;
		/** find the trigger surface from its name */
		TiledMapTriggerObject* FindTriggerObject(chaos::NamedObjectRequest request);
		/** find the trigger surface from its name */
		TiledMapTriggerObject const* FindTriggerObject(chaos::NamedObjectRequest request) const;
		/** find the typed object from its name */
		TiledMapGeometricObject* FindGeometricObject(chaos::NamedObjectRequest request);
		/** find the typed object surface from its name */
		TiledMapGeometricObject const* FindGeometricObject(chaos::NamedObjectRequest request) const;

		/** create a particle spawner */
		template<typename ...PARAMS>
		chaos::ParticleSpawner* CreateParticleSpawner(chaos::NamedObjectRequest layer_instance_name, PARAMS... params)
		{
			LayerInstance* layer_instance = FindLayerInstance(layer_instance_name);
			if (layer_instance == nullptr)
				return nullptr;
			return layer_instance->CreateParticleSpawner(params...);
		}

		template<typename ...PARAMS>
		chaos::ParticleSpawner GetParticleSpawner(chaos::NamedObjectRequest layer_instance_name, PARAMS... params)
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
		//TiledMapGeometricObjectCollisionIterator GetGeometricObjectCollisionIterator(chaos::box2 const& in_collision_box, uint64_t in_collision_mask);

		/** purge all collisions with object deleted */
		void PurgeCollisionInfo();
		/** handle all collision for a given object (TriggerObject) */
		void HandleTriggerCollisions(float delta_time, chaos::ReferencedObject* object, chaos::box2 const& box, int mask);

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
		virtual PlayerPawn * CreatePlayerPawn(Player* player, TiledMapPlayerStartObject* player_start, TiledMapLayerInstance* layer_instance);

		/** get the player start used for an incomming player */
		virtual TiledMapPlayerStartObject* GetPlayerStartForPawn(Player* player);

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
		TiledMapTriggerCollisionInfo* FindTriggerCollisionInfo(chaos::ReferencedObject * object);

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

	class TiledMapTriggerCollisionIterator : public TiledMapObjectCollisionIteratorBase<TiledMapTriggerObject>
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

		 // undefine macros
#undef DEATH_TILEDMAP_CLASSES
#undef DEATH_TILEDMAP_FORWARD_DECL
#undef DEATH_TILEDMAP_FRIEND_DECL
#undef DEATH_TILEDMAP_ALL_FRIENDS

}; // namespace death
