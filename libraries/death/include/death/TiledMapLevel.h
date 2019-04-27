#pragma once

#include <death/GameLevel.h>
#include <death/GameLevelInstance.h>
#include <death/TiledMapParticle.h>

#include <chaos/StandardHeaders.h>
#include <chaos/GeometryFramework.h>
#include <chaos/ReferencedObject.h>
#include <chaos/TiledMap.h>
#include <chaos/Renderable.h>
#include <chaos/Tickable.h>
#include <chaos/ParticleManager.h>
#include <chaos/ParticleDefault.h>
#include <chaos/BitmapAtlas.h>

namespace death
{
	class Player; // for compilation, nested namespace cause issues

	namespace TiledMap
	{
		// ==============================================================
		// FORWARD DECLARATION / FRIENDSHIP MACROS
		// ==============================================================

		// all classes in this file
#define DEATH_TILEDLEVEL_CLASSES (Level) (LevelInstance) (LayerInstance) (GeometricObject) (CameraObject) (PlayerStartObject) (TriggerSurfaceObject) (BaseObject) (LayerInstanceParticlePopulator) (PlayerAndTriggerCollisionRecord)

		// forward declaration
#define DEATH_TILEDLEVEL_FORWARD_DECL(r, data, elem) class elem;
		BOOST_PP_SEQ_FOR_EACH(DEATH_TILEDLEVEL_FORWARD_DECL, _, DEATH_TILEDLEVEL_CLASSES)

			// friendship macro
#define DEATH_TILEDLEVEL_FRIEND_DECL(r, data, elem) friend class elem;
#define DEATH_TILEDLEVEL_ALL_FRIENDS BOOST_PP_SEQ_FOR_EACH(DEATH_TILEDLEVEL_FRIEND_DECL, _, DEATH_TILEDLEVEL_CLASSES)

		// =====================================
		// BoxScissoringWithRepetitionResult : an utility object to compute instances in 2D of a box that collide a given scissor
		// =====================================

		class BoxScissoringWithRepetitionResult
		{
		public:

			/** constructor */
			BoxScissoringWithRepetitionResult(chaos::box2 const & in_target_box, chaos::box2 const & in_scissor_box, bool in_wrap_x, bool in_wrap_y);

			/** offset of a given instance */
			glm::vec2 GetInstanceOffset(glm::ivec2 const & index) const;

		public:

			/** the 'index' of the first instance to render/collide ... (included) */
			glm::ivec2 start_instance = glm::ivec2(0, 0);
			/** the 'index' of the last instance to render/collide ... (excluded) */
			glm::ivec2 last_instance = glm::ivec2(0, 0);
			/** copy of the construction parameters */
			chaos::box2 target_box;;
			/** copy of the construction parameters */
			chaos::box2 scissor_box;
			/** copy of the construction parameters */
			bool wrap_x = false;
			/** copy of the construction parameters */
			bool wrap_y = false;
		};

		// =====================================
		// BaseObject : a base object for special game entities
		// =====================================

		class BaseObject : public chaos::ReferencedObject, public chaos::NamedObject
		{
			DEATH_TILEDLEVEL_ALL_FRIENDS

		public:

			/** constructor */
			BaseObject(LayerInstance * in_layer_instance);

			/** get the layer instance owning this object */
			LayerInstance * GetLayerInstance() { return layer_instance; }
			/** get the layer instance owning this object */
			LayerInstance const * GetLayerInstance() const { return layer_instance; }

		protected:

			/** a reference to the layer instance */
			LayerInstance * layer_instance = nullptr;
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

			/** constructor */
			CameraObject(LayerInstance * in_layer_instance, chaos::TiledMap::GeometricObject * in_geometric_object);

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

			/** constructor */
			PlayerStartObject(LayerInstance * in_layer_instance, chaos::TiledMap::GeometricObject * in_geometric_object);

		protected:

			/** override */
			virtual bool Initialize() override;
		};

		// =====================================
		// TriggerSurfaceObject : an object player can collide with (for moment, rectangle)
		// =====================================

		class TriggerSurfaceObject : public GeometricObject
		{
			DEATH_TILEDLEVEL_ALL_FRIENDS

		public:

			static int const COLLISION_FINISHED = 0;
			static int const COLLISION_STARTED  = 1;
			static int const COLLISION_AGAIN    = 2;

			/** constructor */
			TriggerSurfaceObject(LayerInstance * in_layer_instance, chaos::TiledMap::GeometricObject * in_geometric_object);

			/** whether it is enabled or not */
			bool IsEnabled() const { return enabled; }
			/** change whether the trigger is enabled or not */
			void SetEnabled(bool in_enabled = true) { enabled = in_enabled;}
		
			/** get the trigger ID */
			int GetTriggerID() const { return trigger_id; }

			/** get the object bounding box */
			chaos::box2 GetBoundingBox(bool world_system) const;

			/** search whether there is a collision given box */
			virtual bool IsCollisionWith(chaos::box2 const & other_box, PlayerAndTriggerCollisionRecord const * previous_collisions) const;

		protected:

			/** override */
			virtual bool Initialize() override;

			/** called whenever a collision with player is detected (returns false, if loop is to be broken) */
			virtual bool OnPlayerCollisionEvent(double delta_time, class death::Player * player, chaos::ParticleDefault::Particle * player_particle, int event_type);

		protected:

			/** flag whether to object is enabled or not */
			bool enabled = true;
			/** an ID that helps make classification */
			int trigger_id = 0;
			/** outside box factor (a factor applyed to bounding box to detect whether the player is going outside of the range) */
			float outside_box_factor = 1.0f;
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
			virtual GeometricObject * DoCreateTypedObject(LayerInstance * in_layer_instance, chaos::TiledMap::GeometricObject * in_geometric_object);
			/** create a TriggerSurface specializable method */
			virtual TriggerSurfaceObject * DoCreateTriggerSurface(LayerInstance * in_layer_instance, chaos::TiledMap::GeometricObject * in_geometric_object);
			/** create a Camera specializable method */
			virtual CameraObject * DoCreateCamera(LayerInstance * in_layer_instance, chaos::TiledMap::GeometricObject * in_geometric_object);
			/** create a PlayerStartObject specializable method */
			virtual PlayerStartObject * DoCreatePlayerStart(LayerInstance * in_layer_instance, chaos::TiledMap::GeometricObject * in_geometric_object);
			/** create a PlayerStartObject specializable method */
			virtual LayerInstance * DoCreateLayerInstance(LevelInstance * in_level_instance, chaos::TiledMap::LayerBase * in_layer);

			/** create a typed object 'entry point' */
			GeometricObject * CreateTypedObject(LayerInstance * in_layer_instance, chaos::TiledMap::GeometricObject * in_geometric_object);
			/** create a TriggerSurface 'entry point' */
			TriggerSurfaceObject * CreateTriggerSurface(LayerInstance * in_layer_instance, chaos::TiledMap::GeometricObject * in_geometric_object);
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
			virtual bool OnPlayerTileCollision(double delta_time, class death::Player * player, chaos::ParticleDefault::Particle * player_particle, TileParticle * particle);

			/** the default program when not specified */
			virtual chaos::GPUProgram * GenDefaultRenderProgram();
			/** the default material when not specified */
			virtual chaos::GPURenderMaterial * GetDefaultRenderMaterial();

		protected:

			/** the tiled map corresponding to this level */
			chaos::shared_ptr<chaos::TiledMap::Map> tiled_map;
		};

		// =====================================
		// LayerInstance : instance of a Layer
		// =====================================

		class PlayerAndTriggerCollisionRecord
		{
		public:

			chaos::weak_ptr<death::Player> player;
			std::vector<chaos::weak_ptr<TriggerSurfaceObject>> triggers;
		};


		// =====================================
		// LayerInstance : instance of a Layer
		// =====================================

		class LayerInstance : public chaos::Renderable
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
			TriggerSurfaceObject * FindTriggerSurface(char const * name);
			/** find the trigger surface from its name */
			TriggerSurfaceObject const * FindTriggerSurface(char const * name) const;

			/** find the typed object from its name */
			GeometricObject * FindTypedObject(char const * name);
			/** find the typed object surface from its name */
			GeometricObject const * FindTypedObject(char const * name) const;

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

			/** get whether trigger surfaces are enabled on that layer */
			bool AreTriggerSurfacesEnabled() const { return trigger_surfaces_enabled; }
			/** change whether trigger surfaces are enabled on that layer */
			void SetTriggerSurfacesEnabled(bool in_trigger_surfaces_enabled) { trigger_surfaces_enabled = in_trigger_surfaces_enabled; }

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

		protected:

			/** initialization */
			virtual bool Initialize();
			/** find render material according to its name (or create the default) */
			chaos::GPURenderMaterial * FindOrCreateRenderMaterial(char const * material_name);

			/** override */
			virtual bool DoTick(double delta_time) override;
			/** override */
			virtual int DoDisplay(chaos::Renderer * renderer, chaos::GPUProgramProviderBase const * uniform_provider, chaos::RenderParams const & render_params) const override;

			/** search all collision with the player (tiles/TriggerSurfaceObject) */
			virtual void ComputePlayerCollision(double delta_time);
			/** compute trigger collisions with surface triggers (returns false if if do not want to handle mode player collisions) */
			virtual bool ComputePlayerCollisionWithSurfaceTriggers(double delta_time, class death::Player * player, chaos::ParticleDefault::Particle * player_particle);
			/** compute collisions between players and tiles (returns false if if do not want to handle mode player collisions) */
			virtual bool ComputePlayerTileCollisions(double delta_time, class death::Player * player, chaos::ParticleDefault::Particle * player_particle);

			/** specialized layer */
			bool InitializeLayer(chaos::TiledMap::ImageLayer * image_layer);
			/** specialized layer */
			bool InitializeLayer(chaos::TiledMap::ObjectLayer * object_layer);
			/** specialized layer */
			bool InitializeLayer(chaos::TiledMap::TileLayer * tile_layer);

			/** finalize the particles created */
			virtual bool FinalizeParticles();
			/** try to search a name and a tag in the chaos::layer,  give them to the particle layer */
			virtual bool InitializeParticleLayerNameAndTag(chaos::ParticleLayerBase * in_particle_layer);

			/** find the collision record for a player (clean all records for destroyed player) */
			PlayerAndTriggerCollisionRecord * FindPlayerCollisionRecord(death::Player * player);

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
			std::vector<chaos::shared_ptr<TriggerSurfaceObject>> trigger_surfaces;
			/** the geometric objects */
			std::vector<chaos::shared_ptr<GeometricObject>> typed_objects;

			/** the bounding box of the layer */
			chaos::box2 bounding_box;

			/** whether collision with player are to be tested with that layer */
			bool player_collision_enabled = true;
			/** whether trigger surfaces are enabled on that layer */
			bool trigger_surfaces_enabled = true;
			/** whether collisions with tiles are enabled on that layer */
			bool tile_collisions_enabled = true;

			/** the current offset */
			glm::vec2 offset = glm::vec2(0.0f, 0.0f);

			/** the previous frame collision */
			std::vector<PlayerAndTriggerCollisionRecord> collision_records;
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
			TriggerSurfaceObject * FindTriggerSurface(char const * name);
			/** find the trigger surface from its name */
			TriggerSurfaceObject const * FindTriggerSurface(char const * name) const;
			/** find the typed object from its name */
			GeometricObject * FindTypedObject(char const * name);
			/** find the typed object surface from its name */
			GeometricObject const * FindTypedObject(char const * name) const;

			/** get the bounding box for the level (in worls system obviously) */
			virtual chaos::box2 GetBoundingBox() const override;

			/** get the reference displacement ratio (displacement ratio is a way to make so layer move faster than others) */
			glm::vec2 GetReferenceDisplacementRatio() const { return reference_displacement_ratio; }

		protected:

			/** override */
			virtual bool Initialize(Game * in_game, GameLevel * in_level) override;
			/** override */
			virtual bool DoTick(double delta_time) override;
			/** override */
			virtual int DoDisplay(chaos::Renderer * renderer, chaos::GPUProgramProviderBase const * uniform_provider, chaos::RenderParams const & render_params) const override;

			/** search all collision with the player (tiles/TriggerSurfaceObject) */
			virtual void ComputePlayerCollision(double delta_time);

			/** override */
			virtual void OnLevelStarted() override;
			/** override */
			virtual void OnLevelEnded() override;
			/** override */
			virtual void OnPlayerEntered(Player * player) override;
			/** override */
			virtual void OnPlayerLeaved(Player * player) override;

			/** create the camera */
			virtual void CreateCamera();
			/** destroy the camera */
			virtual void DestroyCamera();

			/** create the particle manager */
			virtual bool CreateParticleManager(Game * in_game);
			/** create the layers instances */
			virtual bool CreateLayerInstances(Game * in_game);

			/** the default material when not specified */
			virtual chaos::GPURenderMaterial * GetDefaultRenderMaterial();

		protected:

			/** the displacement ratio of reference */
			glm::vec2 reference_displacement_ratio = glm::vec2(1.0f, 1.0f);

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
