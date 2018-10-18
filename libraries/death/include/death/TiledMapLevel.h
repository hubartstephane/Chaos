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

		protected:

			/** create a level instance for that level user specified function */
			virtual class GameLevelInstance * DoCreateLevelInstance() override;

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

			

		protected:

			/** override */
			virtual bool DoTick(double delta_time) override;
			/** override */
			virtual int DoDisplay(chaos::GPUProgramProviderBase const * uniform_provider, chaos::RenderParams const & render_params) const override;

		protected:

			/** the particle manager used to render the world */
			boost::intrusive_ptr<chaos::ParticleManager> particle_manager;
		};

	}; // namespace TiledMap

}; // namespace death
