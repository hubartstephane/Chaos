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
		// =====================================
		// Layer : a layer in the game point of view
		// =====================================

		class Layer : public chaos::ReferencedObject
		{
			friend class Level;

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
		// LevelInstance : instance of a Level
		// =====================================

		class LevelInstance : public GameLevelInstance
		{
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
			/** create the layers */
			virtual bool CreateLayers(death::Game * in_game);

		protected:

			/** the particle manager used to render the world */
			boost::intrusive_ptr<chaos::ParticleManager> particle_manager;
		};

	}; // namespace TiledMap

}; // namespace death
