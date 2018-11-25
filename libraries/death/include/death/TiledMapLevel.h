#pragma once

#include <death/GameLevel.h>

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

	namespace TiledMap
	{
		// ==============================================================
		// FORWARD DECLARATION / FRIENDSHIP MACROS
		// ==============================================================

		// all classes in this file
#define DEATH_TILEDLEVEL_CLASSES (Level) (LevelInstance) (LayerInstance) (PlayerStartObject) (BaseObject) (LayerInstanceParticlePopulator)

		// forward declaration
#define DEATH_TILEDLEVEL_FORWARD_DECL(r, data, elem) class elem;
		BOOST_PP_SEQ_FOR_EACH(DEATH_TILEDLEVEL_FORWARD_DECL, _, DEATH_TILEDLEVEL_CLASSES)

			// friendship macro
#define DEATH_TILEDLEVEL_FRIEND_DECL(r, data, elem) friend class elem;
#define DEATH_TILEDLEVEL_ALL_FRIENDS BOOST_PP_SEQ_FOR_EACH(DEATH_TILEDLEVEL_FRIEND_DECL, _, DEATH_TILEDLEVEL_CLASSES)

			// =====================================
			// TileParticleData : a aggregate of data for flushing particles in an allocation
			// =====================================
			
		class TileParticleData
		{
		public:

			/** the particle data, can be unriched by framework */
			chaos::ParticleDefault::Particle particle;
			/** information concerning the tile */
			chaos::TiledMap::TileInfo tile_info;
			/** the bitmap info corresponding to the tile */
			chaos::BitmapAtlas::BitmapInfo const * bitmap_info;
		};

		// =====================================
		// LayerInstanceParticlePopulator : utility class to generate particles for a layer with a cache
		// =====================================

		class LayerInstanceParticlePopulator
		{
			static size_t const PARTICLE_BUFFER_SIZE = 100;

		public:

			/** constructor */
			LayerInstanceParticlePopulator(LayerInstance * in_layer_instance);
			/** initialize the object */
			bool Initialize(Level * level);
			/** insert a particle */
			bool AddParticle(char const * bitmap_name, chaos::box2 const & particle_box, glm::vec4 const & color = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f));
			/** flush remaining particles */
			void FlushParticles();

			/** get the final bounding box */
			chaos::box2 const & GetBoundingBox() const { return bounding_box; }

		protected:

			/** the concerned layer instance */
			LayerInstance * layer_instance = nullptr;
			/** the texture atlas required */
			chaos::BitmapAtlas::TextureArrayAtlas const * texture_atlas = nullptr;
			/** the folder containing the bitmaps */
			chaos::BitmapAtlas::FolderInfo const * folder_info = nullptr;

			/** the allocation for all those particles */
			chaos::ParticleAllocation * allocation = nullptr;

			/** a cache of particles */
			chaos::ParticleDefault::Particle particles[PARTICLE_BUFFER_SIZE];
			/** the cached number of particles */
			size_t particle_count = 0;
			/** a bounding box */
			chaos::box2 bounding_box;
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
		// PlayerStartObject : where the player may start
		// =====================================

		class PlayerStartObject : public BaseObject
		{
			DEATH_TILEDLEVEL_ALL_FRIENDS

		public:

			/** constructor */
			PlayerStartObject(LayerInstance * in_layer_instance);

			/** get the geometric object corresponding to this */
			chaos::TiledMap::GeometricObject * GetGeometricObject() { return geometric_object.get(); }
			/** get the geometric object corresponding to this */
			chaos::TiledMap::GeometricObject const * GetGeometricObject() const { return geometric_object.get(); }

		protected:

			/** initialization */
			virtual bool Initialize(chaos::TiledMap::GeometricObject * in_geometric_object);

		protected:

			/** the associated geometric object */
			boost::intrusive_ptr<chaos::TiledMap::GeometricObject> geometric_object;
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

			/** create a PlayerStartObject specializable method */
			virtual PlayerStartObject * DoCreatePlayerStart(LayerInstance * in_layer_instance);
			/** create a PlayerStartObject specializable method */
			virtual LayerInstance * DoCreateLayerInstance(LevelInstance * in_level_instance);

			/** create a player start 'entry point' */
			PlayerStartObject * CreatePlayerStart(LayerInstance * in_layer_instance, chaos::TiledMap::GeometricObject * in_geometric_object);
			/** create a layer instance 'entry point' */
			LayerInstance * CreateLayerInstance(LevelInstance * in_level_instance, chaos::TiledMap::LayerBase * in_layer);

			/** get the folder in which bitmaps are stored in Game::Atlas */
			virtual chaos::BitmapAtlas::FolderInfo const * GetFolderInfo(LayerInstance * layer_instance) const;
			/** get the atlas to use for the rendering */
			virtual chaos::BitmapAtlas::TextureArrayAtlas const * GetTextureAtlas(LayerInstance * layer_instance) const;
			/** create a particle layer */
			virtual chaos::ParticleLayer * CreateParticleLayer(LayerInstance * layer_instance);

			/** the default program when not specified */
			virtual chaos::GPUProgram * GenDefaultRenderProgram();
			/** the default material when not specified */
			virtual chaos::GPURenderMaterial * GetDefaultRenderMaterial();

		protected:

			/** the tiled map corresponding to this level */
			boost::intrusive_ptr<chaos::TiledMap::Map> tiled_map;
		};

		// =====================================
		// LayerInstance : instance of a Layer
		// =====================================

		class LayerInstance : public chaos::Renderable
		{
			DEATH_TILEDLEVEL_ALL_FRIENDS

		protected:

			/** protected constructor */
			LayerInstance(LevelInstance * in_level_instance);

		public:

			/** get the tiled layer */
			chaos::TiledMap::LayerBase * GetTiledLayer() { return layer.get(); }
			/** get the tiled layer */
			chaos::TiledMap::LayerBase const * GetTiledLayer() const { return layer.get(); }

			/** get the level (for this layer) */
			Level * GetTypedLevel();
			/** get the level (for this layer) */
			Level const * GetTypedLevel() const;

			/** get the game */
			Game * GetGame();
			/** get the game */
			Game const * GetGame() const;

			/** find the player start from its name */
			PlayerStartObject * FindPlayerStart(char const * name);
			/** find the player start from its name */
			PlayerStartObject const * FindPlayerStart(char const * name) const;

			/** get the bounding box for the level */
			chaos::box2 const & GetBoundingBox() const { return bounding_box; }

			/** create a particle allocation for the layer */
			chaos::ParticleAllocation * CreateParticleAllocation();

		protected:

			/** initialization */
			virtual bool Initialize(chaos::TiledMap::LayerBase * in_layer);
			/** find render material according to its name */
			chaos::GPURenderMaterial * FindRenderMaterial(char const * material_name);

			/** override */
			virtual bool DoTick(double delta_time) override;
			/** override */
			virtual int DoDisplay(chaos::GPUProgramProviderBase const * uniform_provider, chaos::RenderParams const & render_params) const override;

			/** specialized layer */
			bool InitializeLayer(chaos::TiledMap::ImageLayer * image_layer);
			/** specialized layer */
			bool InitializeLayer(chaos::TiledMap::ObjectLayer * object_layer);
			/** specialized layer */
			bool InitializeLayer(chaos::TiledMap::TileLayer * tile_layer);

		protected:

			/** displacement ratio relatively to the main layer */
			float displacement_ratio = 1.0f;
			/** whether the layer is to be repeated infinitely in X direction */
			bool wrap_x = false;
			/** whether the layer is to be repeated infinitely in Y direction */
			bool wrap_y = false;
			/** material name */
			std::string material_name;

			/** the level instance this object belongs to */
			LevelInstance * level_instance = nullptr;

			/** the tiled layer corresponding to this object */
			boost::intrusive_ptr<chaos::TiledMap::LayerBase> layer;
			/** the particle layer */
			boost::intrusive_ptr<chaos::ParticleLayer> particle_layer;
			/** the player starts */
			std::vector<boost::intrusive_ptr<PlayerStartObject>> player_starts;

			/** the bounding box of the layer */
			chaos::box2 bounding_box;
		};

		// =====================================
		// LevelInstance : instance of a Level
		// =====================================

		class LevelInstance : public GameLevelInstance
		{
			DEATH_TILEDLEVEL_ALL_FRIENDS

		public:

			/** level getter with a cast */
			Level * GetTypedLevel();
			/** level getter with a cast */
			Level const * GetTypedLevel() const;

			/** get the tiled map */
			chaos::TiledMap::Map * GetTiledMap();
			/** get the tiled map */
			chaos::TiledMap::Map const * GetTiledMap() const;

			/** get the game */
			Game * GetGame() { return game; }
			/** get the game */
			Game const * GetGame() const { return game; }

			/** find the player start from its name */
			PlayerStartObject * FindPlayerStart(char const * name);
			/** find the player start from its name */
			PlayerStartObject const * FindPlayerStart(char const * name) const;

			/** get the bounding box for the level */
			chaos::box2 const & GetBoundingBox() const { return bounding_box; }

		protected:

			/** override */
			virtual bool Initialize(Game * in_game) override;
			/** override */
			virtual bool DoTick(double delta_time) override;
			/** override */
			virtual int DoDisplay(chaos::GPUProgramProviderBase const * uniform_provider, chaos::RenderParams const & render_params) const override;

			/** override */
			virtual void OnLevelStarted() override;
			/** override */
			virtual void OnLevelEnded() override;

			/** destroy the player allocation */
			virtual void UnSpawnPlayer();
			/** create the player allocation */
			virtual void SpawnPlayer();

			/** code to generate particle */
			virtual void InitializePlayerParticle(TiledMap::PlayerStartObject * player_start, chaos::BitmapAtlas::TextureArrayAtlas const * texture_atlas, chaos::BitmapAtlas::BitmapInfo const * bitmap_info, chaos::ParticleAllocation * player_allocation);

			/** create the particle manager */
			virtual bool CreateParticleManager(Game * in_game);
			/** create the layers instances */
			virtual bool CreateLayerInstances(Game * in_game);
			/** compute the bounding box of the level */
			virtual void ComputeBoundingBox();

			/** the default material when not specified */
			virtual chaos::GPURenderMaterial * GetDefaultRenderMaterial();

		protected:

			/** a reference uppon the game */
			Game * game = nullptr;

			/** the particle manager used to render the world */
			boost::intrusive_ptr<chaos::ParticleManager> particle_manager;
			/** the layers */
			std::vector<boost::intrusive_ptr<LayerInstance>> layer_instances;
			/** the default render material */
			boost::intrusive_ptr<chaos::GPURenderMaterial> default_material;
		};

	}; // namespace TiledMap

		 // undefine macros
#undef DEATH_TILEDMAP_CLASSES
#undef DEATH_TILEDMAP_FORWARD_DECL
#undef DEATH_TILEDMAP_FRIEND_DECL
#undef DEATH_TILEDMAP_ALL_FRIENDS

}; // namespace death
