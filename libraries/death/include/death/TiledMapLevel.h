#pragma once

#include <death/GameLevel.h>
#include <death/GameLevelInstance.h>
#include <death/TiledMapParticle.h>
#include <death/GameCheckpoint.h>

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

namespace death
{
	class Player; // for compilation, nested namespace cause issues

	namespace TiledMap
	{
		// ==============================================================
		// FORWARD DECLARATION / FRIENDSHIP MACROS
		// ==============================================================

		// all classes in this file
#define DEATH_TILEDLEVEL_CLASSES (Level) (LevelInstance) (LayerInstance) (GeometricObject) (CameraObject) (PlayerStartObject) (TriggerObject) (CheckpointTriggerObject) (BaseObject) (LayerInstanceParticlePopulator) (PlayerAndTriggerCollisionRecord) (SoundTriggerObject)

		// forward declaration
#define DEATH_TILEDLEVEL_FORWARD_DECL(r, data, elem) class elem;
		BOOST_PP_SEQ_FOR_EACH(DEATH_TILEDLEVEL_FORWARD_DECL, _, DEATH_TILEDLEVEL_CLASSES)

			// friendship macro
#define DEATH_TILEDLEVEL_FRIEND_DECL(r, data, elem) friend class elem;
#define DEATH_TILEDLEVEL_ALL_FRIENDS BOOST_PP_SEQ_FOR_EACH(DEATH_TILEDLEVEL_FRIEND_DECL, _, DEATH_TILEDLEVEL_CLASSES)

		// =====================================
		// BaseObjectCheckpoint
		// =====================================

		class BaseObjectCheckpoint : public chaos::ReferencedObject
		{
		public:

		};

		// =====================================
		// BaseObject : a base object for special game entities
		// =====================================

		class BaseObject : public chaos::Tickable, public CheckpointObject<BaseObjectCheckpoint>
		{
			DEATH_TILEDLEVEL_ALL_FRIENDS

		public:

			/** constructor */
			BaseObject(LayerInstance * in_layer_instance);

			/** whether the object is modified */
			bool IsModified() const { return modified; }
			/** raise the modified flag */
			void SetModified(bool in_modified = true) { modified = in_modified; }

			/** get the layer instance owning this object */
			LayerInstance * GetLayerInstance() { return layer_instance; }
			/** get the layer instance owning this object */
			LayerInstance const * GetLayerInstance() const { return layer_instance; }

			/** whenever the level is being started */
			virtual void OnLevelStarted() {}
			/** whenever the level is being stopped */
			virtual void OnLevelEnded() {}

		protected:

			/** a reference to the layer instance */
			LayerInstance * layer_instance = nullptr;
			/** whether the object has been modified from the JSON base data (usefull for checkpoint serialization) */
			bool modified = false;
		};

		// =====================================
		// GeometricObject 
		// =====================================

		class GeometricObject : public BaseObject
		{
			DEATH_TILEDLEVEL_ALL_FRIENDS

		public:

			/** constructor */
			GeometricObject(LayerInstance * in_layer_instance, chaos::TiledMap::GeometricObject * in_geometric_object);

			/** get the geometric object corresponding to this */
			chaos::TiledMap::GeometricObject * GetGeometricObject() { return geometric_object.get(); }
			/** get the geometric object corresponding to this */
			chaos::TiledMap::GeometricObject const * GetGeometricObject() const { return geometric_object.get(); }

		protected:

			/** additionnal initialization */
			virtual bool Initialize();
			/** enable the creation of additionnal particles */
			virtual bool IsAdditionalParticlesCreationEnabled() const { return true; }

		protected:

			/** the associated geometric object */
			chaos::shared_ptr<chaos::TiledMap::GeometricObject> geometric_object;
		};

		// =====================================
		// CameraObject : where the player may start
		// =====================================

		class CameraObject : public GeometricObject
		{
			DEATH_TILEDLEVEL_ALL_FRIENDS

		public:

			/** inherit constructor */
			using GeometricObject::GeometricObject;

		protected:

			/** override */
			virtual bool Initialize() override;
		};

		// =====================================
		// PlayerStartObject : where the player may start
		// =====================================

		class PlayerStartObject : public GeometricObject
		{
			DEATH_TILEDLEVEL_ALL_FRIENDS

		public:

			/** inherit constructor */
			using GeometricObject::GeometricObject;

		protected:

			/** override */
			virtual bool Initialize() override;
		};
		
		// =====================================
		// TriggerObject : an object player can collide with (for moment, rectangle)
		// =====================================

		class TriggerObject : public GeometricObject
		{
			DEATH_TILEDLEVEL_ALL_FRIENDS

		public:

			static int const COLLISION_FINISHED = 0;
			static int const COLLISION_STARTED  = 1;
			static int const COLLISION_AGAIN    = 2;

			/** constructor */
			TriggerObject(LayerInstance * in_layer_instance, chaos::TiledMap::GeometricObjectSurface * in_surface_object);

			/** whether it is enabled or not */
			bool IsEnabled() const { return enabled; }
			/** change whether the trigger is enabled or not */
			void SetEnabled(bool in_enabled = true);

			/** whether it should be triggered a single time */
			bool IsTriggerOnce() const { return trigger_once; }
			/** change whether the trigger once is enabled or not */
			void SetTriggerOnce(bool in_trigger_once = true);
		
			/** get the trigger ID */
			int GetTriggerID() const { return trigger_id; }

			/** get the object bounding box */
			chaos::box2 GetBoundingBox(bool world_system) const;

			/** search whether there is a collision given box */
			virtual bool IsCollisionWith(chaos::box2 const & other_box, std::vector<chaos::weak_ptr<TriggerObject>> const * triggers) const;

			/** enable the creation of tile for this trigger */
			virtual bool IsAdditionalParticlesCreationEnabled() const {return true;}

		protected:

			/** override */
			virtual bool Initialize() override;

			/** override */
			virtual BaseObjectCheckpoint * DoCreateCheckpoint() const override;
			/** override */
			virtual bool DoSaveIntoCheckpoint(BaseObjectCheckpoint * checkpoint) const override;
			/** override */
			virtual bool DoLoadFromCheckpoint(BaseObjectCheckpoint const * checkpoint) override;

			/** called whenever a collision with player is detected (returns true, if collision is handled successfully (=> important for TriggerOnce) */
			virtual bool OnPlayerCollisionEvent(double delta_time, class Player * player, chaos::ParticleDefault::Particle * player_particle, int event_type);
			/** called whenever a collision with camera is detected (=> important for TriggerOnce) */
			virtual bool OnCameraCollisionEvent(double delta_time, chaos::box2 const & camera_box, int event_type);

		protected:

			/** flag whether to object is enabled or not */
			bool enabled = true;
			/** flag whether to can only trigger once */
			bool trigger_once = false;
			/** an ID that helps make classification */
			int trigger_id = 0;
			/** outside box factor (a factor applyed to bounding box to detect whether the player is going outside of the range) */
			float outside_box_factor = 1.0f;

			/** whenever the trigger-enter event has happened */
			bool enter_event_triggered = false;
		};

		// =====================================
		// TriggerObjectCheckpoint
		// =====================================

		class TriggerObjectCheckpoint : public BaseObjectCheckpoint
		{
		public:

			/** flag whether to object is enabled or not */
			bool enabled = true;
			/** flag whether to can only trigger once */
			bool trigger_once = false;
			/** number of time the trigger-enter event happened */
			bool enter_event_triggered = false;
		};

		// =================================================
		// NotificationTriggerObject
		// =================================================

		class NotificationTriggerObject : public TriggerObject
		{

		public:

			/** constructor */
			using TriggerObject::TriggerObject;
			/** override */
			virtual bool IsAdditionalParticlesCreationEnabled() const override;
			/** override */
			virtual bool Initialize() override;

		protected:

			/** generic method to handle both collision with player and camera */
			virtual bool OnTriggerCollision(double delta_time, int event_type);
			/** override */
			virtual bool OnCameraCollisionEvent(double delta_time, chaos::box2 const & camera_box, int event_type) override;
			/** override */
			virtual bool OnPlayerCollisionEvent(double delta_time, class Player * player, chaos::ParticleDefault::Particle * player_particle, int event_type) override;

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
		// CheckpointTriggerObject
		// =================================================

		class CheckpointTriggerObject : public TriggerObject
		{

		public:

			/** constructor */
			using TriggerObject::TriggerObject;
			/** override */
			virtual bool IsAdditionalParticlesCreationEnabled() const override;
			/** override */
			virtual bool Initialize() override;

		protected:

			/** override */
			virtual bool OnCameraCollisionEvent(double delta_time, chaos::box2 const & camera_box, int event_type) override;
		};

		// =================================================
		// SoundTriggerObject : an object that play a sound
		// =================================================

		class SoundTriggerObject : public TriggerObject
		{

		public:

			/** constructor */
			using TriggerObject::TriggerObject;
			/** override */
			virtual bool IsAdditionalParticlesCreationEnabled() const override;

		protected:

			/** override */
			virtual bool Initialize() override;
			/** override */
			virtual bool OnCameraCollisionEvent(double delta_time, chaos::box2 const & camera_box, int event_type) override;

			/** the sound creation method */
			chaos::Sound * CreateSound() const;

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
		// FinishingTriggerObject
		// =================================================

		class FinishingTriggerObject : public TriggerObject
		{

		public:

			/** inherit constructor */
			using TriggerObject::TriggerObject;

		protected:

			/** override */
			virtual bool IsAdditionalParticlesCreationEnabled() const override;
			/** override */
			virtual bool OnPlayerCollisionEvent(double delta_time, Player * player, chaos::ParticleDefault::Particle * player_particle, int event_type) override;
		};


		// =====================================
		// Level : a level described by a tiledmap
		// =====================================

		class Level : public GameLevel
		{
			DEATH_TILEDLEVEL_ALL_FRIENDS

		public:

			/** constructor */
			Level();

			/** initialization from tiled_map */
			virtual bool Initialize(chaos::TiledMap::Map * in_tiled_map);

			/** get the tiled map */
			chaos::TiledMap::Map * GetTiledMap() { return tiled_map.get(); }
			/** get the tiled map */
			chaos::TiledMap::Map const * GetTiledMap() const { return tiled_map.get(); }

		protected:

			/** create a level instance for that level user specified function */
			virtual GameLevelInstance * DoCreateLevelInstance(Game * in_game) override;


			/** create a typed object specializable method */
			virtual GeometricObject * DoCreateGeometricObject(LayerInstance * in_layer_instance, chaos::TiledMap::GeometricObject * in_geometric_object);
			/** create a Camera specializable method */
			virtual CameraObject * DoCreateCamera(LayerInstance * in_layer_instance, chaos::TiledMap::GeometricObject * in_geometric_object);
			/** create a PlayerStartObject specializable method */
			virtual PlayerStartObject * DoCreatePlayerStart(LayerInstance * in_layer_instance, chaos::TiledMap::GeometricObject * in_geometric_object);
			/** create a PlayerStartObject specializable method */
			virtual LayerInstance * DoCreateLayerInstance(LevelInstance * in_level_instance, chaos::TiledMap::LayerBase * in_layer);

			/** create a typed object 'entry point' */
			GeometricObject * CreateGeometricObject(LayerInstance * in_layer_instance, chaos::TiledMap::GeometricObject * in_geometric_object);
			/** create a camera 'entry point' */
			CameraObject * CreateCamera(LayerInstance * in_layer_instance, chaos::TiledMap::GeometricObject * in_geometric_object);
			/** create a player start 'entry point' */
			PlayerStartObject * CreatePlayerStart(LayerInstance * in_layer_instance, chaos::TiledMap::GeometricObject * in_geometric_object);
			/** create a layer instance 'entry point' */
			LayerInstance * CreateLayerInstance(LevelInstance * in_level_instance, chaos::TiledMap::LayerBase * in_layer);

			/** get the folder in which bitmaps are stored in Game::Atlas */
			virtual chaos::BitmapAtlas::FolderInfo const * GetFolderInfo(LayerInstance * layer_instance) const;
			/** get the atlas to use for the rendering */
			virtual chaos::BitmapAtlas::TextureArrayAtlas const * GetTextureAtlas(LayerInstance * layer_instance) const;
			/** create a particle layer */
			virtual chaos::ParticleLayerBase * CreateParticleLayer(LayerInstance * layer_instance);

			/** called after all particles of a layers has been created, so we can plug additionnal data */
			virtual bool FinalizeLayerParticles(LayerInstance * layer_instance, chaos::ParticleAllocationBase * allocation){ return true; }

			/** called whenever a collision between player and tile happens */
			virtual bool OnPlayerTileCollision(double delta_time, class Player * player, chaos::ParticleDefault::Particle * player_particle, TileParticle * particle);

			/** the default program when not specified */
			virtual chaos::GPUProgram * GenDefaultRenderProgram();
			/** the default material when not specified */
			virtual chaos::GPURenderMaterial * GetDefaultRenderMaterial();

		protected:

			/** the tiled map corresponding to this level */
			chaos::shared_ptr<chaos::TiledMap::Map> tiled_map;
		};

		// =====================================
		// PlayerAndTriggerCollisionRecord 
		// =====================================

		class PlayerAndTriggerCollisionRecord
		{
		public:

			/** the player considered */
			chaos::weak_ptr<Player> player;
			/** all the triggers colliding */
			std::vector<chaos::weak_ptr<TriggerObject>> triggers;
		};

		// =====================================
		// TiledLayerCheckpoint
		// =====================================

		class TiledLayerCheckpoint : public chaos::ReferencedObject
		{
			public:

			// the checkpoint per BaseObject (see object_id)
			std::map<int, chaos::shared_ptr<BaseObjectCheckpoint>> trigger_checkpoints;
			// the checkpoint per BaseObject (see object_id)
			std::map<int, chaos::shared_ptr<BaseObjectCheckpoint>> object_checkpoints;
		};


		// =====================================
		// LayerInstance : instance of a Layer
		// =====================================

		class LayerInstance : public chaos::GPURenderable, public CheckpointObject<TiledLayerCheckpoint>
		{
			DEATH_TILEDLEVEL_ALL_FRIENDS

		protected:

			/** protected constructor */
			LayerInstance(LevelInstance * in_level_instance, chaos::TiledMap::LayerBase * in_layer);

		public:

			/** get the tiled layer */
			chaos::TiledMap::LayerBase * GetTiledLayer() { return layer.get(); }
			/** get the tiled layer */
			chaos::TiledMap::LayerBase const * GetTiledLayer() const { return layer.get(); }

			/** get the level (for this layer) */
			Level * GetTiledLevel();
			/** get the level (for this layer) */
			Level const * GetTiledLevel() const;

			/** get the level instance for this layer */
			LevelInstance * GetTiledLevelInstance();
			/** get the level instance for this layer */
			LevelInstance const * GetTiledLevelInstance() const;

			/** get the game */
			Game * GetGame();
			/** get the game */
			Game const * GetGame() const;

			/** find the player start from its name */
			PlayerStartObject * FindPlayerStart(char const * name);
			/** find the player start from its name */
			PlayerStartObject const * FindPlayerStart(char const * name) const;

			/** find the camera from its name */
			CameraObject * FindCamera(char const * name);
			/** find the camera from its name */
			CameraObject const * FindCamera(char const * name) const;

			/** find the trigger surface from its name */
			TriggerObject * FindTrigger(char const * name);
			/** find the trigger surface from its name */
			TriggerObject const * FindTrigger(char const * name) const;

			/** find the geometric object from its name */
			GeometricObject * FindGeometricObject(char const * name);
			/** find the geometric object surface from its name */
			GeometricObject const * FindGeometricObject(char const * name) const;

			/** get the bounding box for the level */
			chaos::box2 GetBoundingBox(bool world_system) const;

			/** create a particle allocation for the layer */
			chaos::ParticleAllocationBase * CreateParticleAllocation();
			/** create the particle layer if required */
			chaos::ParticleLayerBase * CreateParticleLayer();

			/** get whether player collisions are enabled on that layer */
			bool ArePlayerCollisionEnabled() const { return player_collision_enabled; }
			/** change whether collisions with player are to be test on that layer */
			void SetPlayerCollisionEnabled(bool in_player_collision_enabled){ player_collision_enabled = in_player_collision_enabled; }

			/** get whether camera collisions are enabled on that layer */
			bool AreCameraCollisionEnabled() const { return camera_collision_enabled; }
			/** change whether collisions with camera are to be test on that layer */
			void SetCameraCollisionEnabled(bool in_camera_collision_enabled){ camera_collision_enabled = in_camera_collision_enabled; }

			/** get whether trigger surfaces are enabled on that layer */
			bool AreTriggersEnabled() const { return triggers_enabled; }
			/** change whether trigger surfaces are enabled on that layer */
			void SetTriggersEnabled(bool in_triggers_enabled) { triggers_enabled = in_triggers_enabled; }

			/** get whether collisions with tiles are enabled on that layer */
			bool AreTileCollisionsEnabled() const { return tile_collisions_enabled; }
			/** change whether collisions with tiles are enabled on that layer */
			void SetTileCollisionsEnabled(bool in_tile_collisions_enabled) { tile_collisions_enabled = in_tile_collisions_enabled; }

			/** get the layer offset */
			glm::vec2 GetLayerOffset() const { return offset; }
			/** set the layer offset */
			void SetLayerOffset(glm::vec2 const & in_offset){ offset = in_offset; }


			/** get the particle layer */
			chaos::ParticleLayerBase * GetParticleLayer(){ return particle_layer.get();}
			/** get the particle layer */
			chaos::ParticleLayerBase const * GetParticleLayer() const { return particle_layer.get();}

			/** processing collision on all tiles until FUNC returns false to stop */
			template<typename FUNC>
			bool FindTileCollisions(chaos::box2 const & bounding_box, FUNC func)
			{			
				// no particle layer, no collisions
				if (particle_layer == nullptr)
					return true;
				// iterate over all allocations
				size_t allocation_count = particle_layer->GetAllocationCount();
				for (size_t i = 0; i < allocation_count; ++i)
				{
					chaos::ParticleAllocationBase * particle_allocation = particle_layer->GetAllocation(i);
					if (particle_allocation == nullptr)
						continue;

					chaos::ParticleAccessor<TileParticle> particles = particle_allocation->GetParticleAccessor<TileParticle>();

					size_t particle_count = particles.GetCount();
					for (size_t j = 0; j < particle_count; ++j)
					{
						TileParticle & particle = particles[j];
						if (chaos::Collide(bounding_box, particle.bounding_box))
							if (!func(particle)) // stop other collisions
								return false;
					}
				}
				return true; // continue other collisions
			}

			/** returns the number of trigger surfaces */
			size_t GetTriggerCount() const;
			/** returns a trigger surface by its index */
			TriggerObject * GetTrigger(size_t index);
			/** returns a trigger surface by its index */
			TriggerObject const * GetTrigger(size_t index) const;


		protected:

			/** initialization */
			virtual bool Initialize();

			/** override */
			virtual TiledLayerCheckpoint * DoCreateCheckpoint() const override;
			/** override */
			virtual bool DoSaveIntoCheckpoint(TiledLayerCheckpoint * checkpoint) const override;
			/** override */
			virtual bool DoLoadFromCheckpoint(TiledLayerCheckpoint const * checkpoint) override;

			/** find render material according to its name (or create the default) */
			chaos::GPURenderMaterial * FindOrCreateRenderMaterial(char const * material_name);

			/** override */
			virtual bool DoTick(double delta_time) override;
			/** override */
			virtual int DoDisplay(chaos::GPURenderer * renderer, chaos::GPUProgramProviderBase const * uniform_provider, chaos::GPURenderParams const & render_params) const override;

			/** search all collision with the player (tiles/TriggerObject) */
			virtual void ComputePlayerAndCameraCollision(double delta_time);
			/** compute trigger collisions with surface triggers (returns false if if do not want to handle mode player collisions) */
			virtual bool ComputePlayerCollisionWithSurfaceTriggers(double delta_time, class Player * player, chaos::ParticleDefault::Particle * player_particle);
			/** compute trigger collisions with camera */
			virtual bool ComputeCameraCollisionWithSurfaceTriggers(double delta_time, chaos::box2 const & camera_box);
			/** compute collisions between players and tiles (returns false if if do not want to handle mode player collisions) */
			virtual bool ComputePlayerTileCollisions(double delta_time, class Player * player, chaos::ParticleDefault::Particle * player_particle);

			/** specialized layer */
			bool InitializeImageLayer(chaos::TiledMap::ImageLayer * image_layer);
			/** specialized layer */
			bool InitializeObjectLayer(chaos::TiledMap::ObjectLayer * object_layer);
			/** specialized layer */
			bool InitializeTileLayer(chaos::TiledMap::TileLayer * tile_layer);

			/** create an object in an object layer */
			GeometricObject * CreateObjectInstance(chaos::TiledMap::GeometricObject * geometric_object);
			/** create an object in an object layer */
			void CreateAdditionalObjectParticles(chaos::TiledMap::GeometricObject * geometric_object, GeometricObject * object, LayerInstanceParticlePopulator & particle_populator);

			/** finalize the particles created */
			virtual bool FinalizeParticles();
			/** try to search a name and a tag in the chaos::layer,  give them to the particle layer (and some other data as well) */
			virtual bool InitializeParticleLayer(chaos::ParticleLayerBase * in_particle_layer);

			/** find the collision record for a player (clean all records for destroyed player) */
			PlayerAndTriggerCollisionRecord * FindPlayerCollisionRecord(Player * player);

			/** utility methods for loading objects in layer instances */
			template<typename ELEMENT_VECTOR, typename CHECKPOINT_VECTOR>
			bool DoLoadFromCheckpointHelper(ELEMENT_VECTOR & elements, CHECKPOINT_VECTOR const & checkpoints);
			/** utility methods for saving objects in layer instances */
			template<typename ELEMENT_VECTOR, typename CHECKPOINT_VECTOR>
			bool DoSaveIntoCheckpointHelper(ELEMENT_VECTOR const & elements, CHECKPOINT_VECTOR & checkpoints) const;

			/** some callbacks */
			virtual void OnLevelEnded();
			/** some callbacks */
			virtual void OnLevelStarted();

		protected:

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

			/** the level instance this object belongs to */
			LevelInstance * level_instance = nullptr;

			/** the tiled layer corresponding to this object */
			chaos::shared_ptr<chaos::TiledMap::LayerBase> layer;
			/** the particle layer */
			chaos::shared_ptr<chaos::ParticleLayerBase> particle_layer;

			/** the player starts */
			std::vector<chaos::shared_ptr<PlayerStartObject>> player_starts;
			/** the player cameras */
			std::vector<chaos::shared_ptr<CameraObject>> cameras;
			/** the trigger surface */
			std::vector<chaos::shared_ptr<TriggerObject>> triggers;
			/** the geometric objects */
			std::vector<chaos::shared_ptr<GeometricObject>> geometric_objects;

			/** the bounding box of the layer */
			chaos::box2 bounding_box;

			/** whether collision with player are to be tested with that layer */
			bool player_collision_enabled = true;
			/** whether collision with camera are to be tested with that layer */
			bool camera_collision_enabled = true;
			/** whether trigger surfaces are enabled on that layer */
			bool triggers_enabled = true;
			/** whether collisions with tiles are enabled on that layer */
			bool tile_collisions_enabled = true;

			/** the current offset */
			glm::vec2 offset = glm::vec2(0.0f, 0.0f);

			/** the previous frame collision */
			std::vector<PlayerAndTriggerCollisionRecord> collision_records;
			/** the previous frame trigger collision with camera */
			std::vector<chaos::weak_ptr<TriggerObject>> camera_collision_records;
		};

		// =====================================
		// TiledLevelCheckpoint
		// =====================================

		class TiledLevelCheckpoint : public LevelCheckpoint
		{
		public:

			// the checkpoint per LayerBase (see object_id)
			std::map<int, chaos::shared_ptr<TiledLayerCheckpoint>> layer_checkpoints;
		};

		// =====================================
		// LevelInstance : instance of a Level
		// =====================================

		class LevelInstance : public GameLevelInstance
		{
			DEATH_TILEDLEVEL_ALL_FRIENDS

		public:

			/** level getter with a cast */
			Level * GetTiledLevel();
			/** level getter with a cast */
			Level const * GetTiledLevel() const;

			/** get the tiled map */
			chaos::TiledMap::Map * GetTiledMap();
			/** get the tiled map */
			chaos::TiledMap::Map const * GetTiledMap() const;

			/** get the game */
			Game * GetGame() { return game; }
			/** get the game */
			Game const * GetGame() const { return game; }

			/** find the layer instance from its name */
			LayerInstance * FindLayerInstance(char const * name);
			/** find the layer instance from its name */
			LayerInstance const * FindLayerInstance(char const * name) const;
			/** find the camera from its name */
			CameraObject * FindCamera(char const * name);
			/** find the camera from its name */
			CameraObject const * FindCamera(char const * name) const;
			/** find the player start from its name */
			PlayerStartObject * FindPlayerStart(char const * name);
			/** find the player start from its name */
			PlayerStartObject const * FindPlayerStart(char const * name) const;
			/** find the trigger surface from its name */
			TriggerObject * FindTrigger(char const * name);
			/** find the trigger surface from its name */
			TriggerObject const * FindTrigger(char const * name) const;
			/** find the typed object from its name */
			GeometricObject * FindGeometricObject(char const * name);
			/** find the typed object surface from its name */
			GeometricObject const * FindGeometricObject(char const * name) const;

			/** get the bounding box for the level (in worls system obviously) */
			virtual chaos::box2 GetBoundingBox() const override;

		protected:

			/** override */
			virtual bool Initialize(Game * in_game, GameLevel * in_level) override;
			/** override */
			virtual bool DoTick(double delta_time) override;
			/** override */
			virtual int DoDisplay(chaos::GPURenderer * renderer, chaos::GPUProgramProviderBase const * uniform_provider, chaos::GPURenderParams const & render_params) const override;

			/** search all collision with the player (tiles/TriggerObject) */
			virtual void ComputePlayerAndCameraCollision(double delta_time);

			/** override */
			virtual void OnPlayerEntered(Player * player) override;
			/** override */
			virtual void OnPlayerLeaved(Player * player) override;

			/** override */
			virtual void OnLevelEnded() override;
			/** override */
			virtual void OnLevelStarted() override;

			/** create the cameras */
			virtual void CreateCameras() override;

			/** create the particle manager */
			virtual bool CreateParticleManager(Game * in_game);
			/** create the layers instances */
			virtual bool CreateLayerInstances(Game * in_game);

			/** override */
			virtual void CreateBackgroundImage() override;
			/** override */
			virtual void SetInGameMusic() override;

			/** the default material when not specified */
			virtual chaos::GPURenderMaterial * GetDefaultRenderMaterial();

			/** override */
			virtual LevelCheckpoint * DoCreateCheckpoint() const override;
			/** override */
			virtual bool DoSaveIntoCheckpoint(LevelCheckpoint * checkpoint) const override;
			/** override */
			virtual bool DoLoadFromCheckpoint(LevelCheckpoint const * checkpoint) override;


		protected:

			/** explicit bounding box (else it is dynamic with LayerInstance evaluation) */
			chaos::box2 explicit_bounding_box;
			/** whether the explicit_bounding_box is valid (empty is not a good answer) */
			bool has_explicit_bounding_box = false;

			/** the layer of reference for displacement */
			chaos::shared_ptr<TiledMap::LayerInstance> reference_layer;

			/** the particle manager used to render the world */
			chaos::shared_ptr<chaos::ParticleManager> particle_manager;
			/** the layers */
			std::vector<chaos::shared_ptr<LayerInstance>> layer_instances;
			/** the default render material */
			chaos::shared_ptr<chaos::GPURenderMaterial> default_material;
		};

	}; // namespace TiledMap

		 // undefine macros
#undef DEATH_TILEDMAP_CLASSES
#undef DEATH_TILEDMAP_FORWARD_DECL
#undef DEATH_TILEDMAP_FRIEND_DECL
#undef DEATH_TILEDMAP_ALL_FRIENDS

}; // namespace death
