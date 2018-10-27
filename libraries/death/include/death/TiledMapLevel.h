#pragma once

#include <death/GameLevel.h>

#include <chaos/StandardHeaders.h>
#include <chaos/GeometryFramework.h>
#include <chaos/ReferencedObject.h>
#include <chaos/TiledMap.h>
#include <chaos/Renderable.h>
#include <chaos/ParticleManager.h>

namespace death
{

	namespace TiledMap
	{
		// ==============================================================
		// FORWARD DECLARATION / FRIENDSHIP MACROS
		// ==============================================================

		// all classes in this file
#define DEATH_TILEDLEVEL_CLASSES (Level) (Layer) (LevelInstance) (LevelInstance)

		// forward declaration
#define DEATH_TILEDLEVEL_FORWARD_DECL(r, data, elem) class elem;
		BOOST_PP_SEQ_FOR_EACH(DEATH_TILEDLEVEL_FORWARD_DECL, _, DEATH_TILEDLEVEL_CLASSES)

			// friendship macro
#define DEATH_TILEDLEVEL_FRIEND_DECL(r, data, elem) friend class elem;
#define DEATH_TILEDLEVEL_ALL_FRIENDS BOOST_PP_SEQ_FOR_EACH(DEATH_TILEDLEVEL_FRIEND_DECL, _, DEATH_TILEDLEVEL_CLASSES)

		// =====================================
		// Layer : a layer in the game point of view
		// =====================================

		class Layer : public chaos::ReferencedObject
		{
			DEATH_TILEDLEVEL_ALL_FRIENDS

		public:

			/** get the tiled layer */
			chaos::TiledMap::LayerBase * GetTiledLayer() { return tiled_layer.get(); }
			/** get the tiled layer */
			chaos::TiledMap::LayerBase const * GetTiledLayer() const { return tiled_layer.get(); }

		protected:

			/** initialize this instance from chaos::LayerBase */
			bool Initialize(chaos::TiledMap::LayerBase * in_layer);

		protected:

			/** displacement ratio relatively to the main layer */
			float displacement_ratio = 1.0f;
			/** whether the layer is to be repeated infinitely in X direction */
			bool wrap_x = false;
			/** whether the layer is to be repeated infinitely in Y direction */
			bool wrap_y = false;
			/** material name */
			std::string material_name;

			/** the tiled layer corresponding to this object */
			boost::intrusive_ptr<chaos::TiledMap::LayerBase> tiled_layer;
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

			/** initialization from tiledmap */
			virtual bool Initialize(chaos::TiledMap::Map * in_tiled_map);

			/** get the tiled map */
			chaos::TiledMap::Map * GetTiledMap() { return tiled_map.get(); }
			/** get the tiled map */
			chaos::TiledMap::Map const * GetTiledMap() const { return tiled_map.get(); }

		protected:

			/** create a level instance for that level user specified function */
			virtual class GameLevelInstance * DoCreateLevelInstance(Game * in_game) override;

			/** a function that iterate over the layers and extract important informations */
			bool ExtractLayersInformation();

		protected:

			/** the layers for this map */
			std::vector<boost::intrusive_ptr<Layer>> layers;
			/** the tiled map corresponding to this level */
			boost::intrusive_ptr<chaos::TiledMap::Map> tiled_map;
		};

		// =====================================
		// LayerInstance : instance of a Layer
		// =====================================

		class LayerInstance : public chaos::ReferencedObject
		{
			DEATH_TILEDLEVEL_ALL_FRIENDS

		public:

			/** get the tiled layer */
			chaos::TiledMap::LayerBase * GetTiledLayer() { return layer->GetTiledLayer(); }
			/** get the tiled layer */
			chaos::TiledMap::LayerBase const * GetTiledLayer() const { return layer->GetTiledLayer(); }

		protected:

			/** initialization */
			virtual bool Initialize(Layer * in_layer);

		protected:

			/** the layer corresponding to this instance */
			boost::intrusive_ptr<Layer> layer;
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

		protected:

			/** override */
			virtual bool Initialize(death::Game * in_game) override;
			/** override */
			virtual bool DoTick(double delta_time) override;
			/** override */
			virtual int DoDisplay(chaos::GPUProgramProviderBase const * uniform_provider, chaos::RenderParams const & render_params) const override;

			/** create the particle manager */
			virtual bool CreateParticleManager(death::Game * in_game);
			/** create the layers instances */
			virtual bool CreateLayerInstances(death::Game * in_game);

		protected:

			/** the particle manager used to render the world */
			boost::intrusive_ptr<chaos::ParticleManager> particle_manager;
			/** the layers */
			std::vector<boost::intrusive_ptr<LayerInstance>> layer_instances;
		};

	}; // namespace TiledMap

		 // undefine macros
#undef DEATH_TILEDMAP_CLASSES
#undef DEATH_TILEDMAP_FORWARD_DECL
#undef DEATH_TILEDMAP_FRIEND_DECL
#undef DEATH_TILEDMAP_ALL_FRIENDS

}; // namespace death
