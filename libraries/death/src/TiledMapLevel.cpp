#include <death/TiledMapLevel.h>
#include <chaos/TiledMapTools.h>

namespace death
{
	namespace TiledMap
	{
		// =====================================
		// Layer implementation
		// =====================================

		bool Layer::Initialize(chaos::TiledMap::LayerBase * in_layer)
		{
			assert(in_layer != nullptr);
			assert(tiled_layer == nullptr);

			
			tiled_layer = in_layer;



			return true;
		}

		// =====================================
		// Level implementation
		// =====================================

		Level::Level()
		{

		}

		bool Level::ExtractLayersInformation()
		{
			// iterate over all LayerBase ordered by zorder
			int count = tiled_map->GetLayerCount();
			for (int i = 0; i < count; ++i)
			{
				// get the layer by zorder
				chaos::TiledMap::LayerBase * layer = tiled_map->FindLayerByZOrder(i);
				if (layer == nullptr)
					continue;

				// create a 'layer' from ::death point of view
				death::TiledMap::Layer * my_layer = new death::TiledMap::Layer;
				if (my_layer == nullptr)
					continue;

				// initialize death::Layer from chaos::Layer
				if (!my_layer->Initialize(layer))
				{
					delete(my_layer);
					continue;
				}

				// store the layer
				layers.push_back(my_layer);

				// LAYER_DISPLACEMENT_FACTOR : the relative displacement factor of the camera
				// LAYER_WARP_X              : whether the layer is to be repeated at infinity along X axis
				// LAYER_WARP_Y              : whether the layer is to be repeated at infinity along Y axis
				// MATERIAL                  : the default material to apply to each object of the layer

				//chaos::TiledMapTools::HasFlag()

				//chaos::TiledMapTools::FindExplicitWorldBounds()
				//layer-




			}
			return true;
		}

		bool Level::Initialize(chaos::TiledMap::Map * in_tiled_map)
		{
			assert(in_tiled_map != nullptr);

			/** already initialized ?*/
			if (tiled_map != nullptr)
				return false;
			/** copy the tile map */
			tiled_map = in_tiled_map;
			/** the real initialization */
			ExtractLayersInformation();



			return true;
		}


		GameLevelInstance * Level::DoCreateLevelInstance()
		{
			LevelInstance * result = new LevelInstance;
			if (result == nullptr)
				return result;


			return result;
		}

		// =====================================
		// LevelInstance implementation
		// =====================================

		bool LevelInstance::DoTick(double delta_time)
		{
			if (particle_manager == nullptr)
				return 0;
			return particle_manager->Tick(delta_time);
		}

		int LevelInstance::DoDisplay(chaos::GPUProgramProviderBase const * uniform_provider, chaos::RenderParams const & render_params) const
		{
			if (particle_manager == nullptr)
				return 0;
			return particle_manager->Display(uniform_provider, render_params);
		}

	}; // namespace TiledMap

}; // namespace death

