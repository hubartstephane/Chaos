#include <death/TiledMapLevel.h>
#include <death/Game.h>
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

			// keep a copy of the tile layer			
			tiled_layer = in_layer;

			// get the properties of interrest
			displacement_ratio = tiled_layer->FindPropertyFloat("DISPLACEMENT_FACTOR", 1.0f);
			wrap_x = tiled_layer->FindPropertyBool("WRAP_X", false);
			wrap_y = tiled_layer->FindPropertyBool("WRAP_Y", false);
			material_name = tiled_layer->FindPropertyString("MATERIAL", "");

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

		GameLevelInstance * Level::DoCreateLevelInstance(Game * in_game)
		{
			return new LevelInstance;
		}

		// =====================================
		// LayerInstance implementation
		// =====================================

		bool LayerInstance::Initialize(Layer * in_layer)
		{
			layer = in_layer;

			return true;
		}

		// =====================================
		// LevelInstance implementation
		// =====================================

		chaos::TiledMap::Map * LevelInstance::GetTiledMap()
		{
			Level * level = GetTypedLevel();
			if (level == nullptr)
				return nullptr;
			return level->GetTiledMap();
		}

		chaos::TiledMap::Map const * LevelInstance::GetTiledMap() const 
		{
			Level const * level = GetTypedLevel();
			if (level == nullptr)
				return nullptr;
			return level->GetTiledMap();
		}

		Level * LevelInstance::GetTypedLevel()
		{
			return dynamic_cast<Level*>(GetLevel());
		}

		Level const * LevelInstance::GetTypedLevel() const
		{
			return dynamic_cast<Level const *>(GetLevel());
		}

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

		bool LevelInstance::Initialize(death::Game * in_game)
		{
			assert(in_game != nullptr);
			// create a particle manager
			if (!CreateParticleManager(in_game))
				return false;
			// create a the layers
			if (!CreateLayerInstances(in_game))
				return false;

			 



 			return true;
		}

		bool LevelInstance::CreateParticleManager(death::Game * in_game)
		{
			particle_manager = new chaos::ParticleManager;
			if (particle_manager == nullptr)
				return false;
			particle_manager->SetTextureAtlas(in_game->GetTextureAtlas()); // take the atlas
			return true;
		}

		bool LevelInstance::CreateLayerInstances(death::Game * in_game)
		{
			Level * level = GetTypedLevel();

			size_t count = level->layers.size();
			for (size_t i = 0; i < count; ++i)
			{
				Layer * layer = level->layers[i].get();
				if (layer == nullptr)
					continue;

				LayerInstance * layer_instance = new LayerInstance;
				if (layer_instance == nullptr)
					continue;

				if (!layer_instance->Initialize(layer))
					continue;

				layer_instances.push_back(layer_instance);
			}
			return true;
		}


	}; // namespace TiledMap

}; // namespace death

