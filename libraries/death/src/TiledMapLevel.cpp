#include <death/TiledMapLevel.h>
#include <death/Game.h>
#include <chaos/TiledMapTools.h>
#include <chaos/ParticleDefault.h>

namespace death
{
	namespace TiledMap
	{

		// =====================================
		// Level implementation
		// =====================================

		Level::Level()
		{

		}

		bool Level::Initialize(chaos::TiledMap::Map * in_tiled_map)
		{
			assert(in_tiled_map != nullptr);
			assert(tiled_map == nullptr);
			tiled_map = in_tiled_map;
			return true;
		}

		GameLevelInstance * Level::DoCreateLevelInstance(Game * in_game)
		{
			return new LevelInstance;
		}

		// =====================================
		// PlayerStartInstance implementation
		// =====================================

		bool PlayerStartInstance::Initialize(chaos::TiledMap::GeometricObject * in_geometric_object)
		{
			geometric_object = in_geometric_object;


			return true;
		}

		// =====================================
		// LayerInstance implementation
		// =====================================

		chaos::GPURenderMaterial * LayerInstance::FindRenderMaterial(char const * material_name)
		{
			// early exit
			if (material_name == nullptr || material_name[0] == 0)
				return nullptr;
			// get the resource manager
			chaos::GPUResourceManager * resource_manager = chaos::MyGLFW::SingleWindowApplication::GetGPUResourceManagerInstance();
			if (resource_manager == nullptr)
				return nullptr;
			// get the material
			return resource_manager->FindRenderMaterial(material_name);
		}

		bool LayerInstance::Initialize(chaos::TiledMap::LayerBase * in_layer)
		{
			// store the layer
			layer = in_layer;

			// get the properties of interrest
			displacement_ratio = in_layer->FindPropertyFloat("DISPLACEMENT_FACTOR", 1.0f);
			wrap_x = in_layer->FindPropertyBool("WRAP_X", false);
			wrap_y = in_layer->FindPropertyBool("WRAP_Y", false);
			material_name = in_layer->FindPropertyString("MATERIAL", "");

			// special initialization

			chaos::TiledMap::ImageLayer * image_layer = in_layer->GetImageLayer();
			if (image_layer != nullptr)
				return Initialize(image_layer);

			chaos::TiledMap::ObjectLayer * object_layer = in_layer->GetObjectLayer();
			if (object_layer != nullptr)
				return Initialize(object_layer);

			chaos::TiledMap::TileLayer * tile_layer = in_layer->GetTileLayer();
			if (tile_layer != nullptr)
				return Initialize(tile_layer);

			return false;
		}

		bool LayerInstance::InitializeLayer(chaos::TiledMap::ImageLayer * image_layer)
		{

			return true;
		}

		bool LayerInstance::InitializeLayer(chaos::TiledMap::ObjectLayer * object_layer)
		{
			size_t count = object_layer->geometric_objects.size();
			for (size_t i = 0; i < count; ++i)
			{
				chaos::TiledMap::GeometricObject * geometric_object = object_layer->geometric_objects[i].get();
				if (geometric_object == nullptr)
					continue;
				
				// player start ?
				bool player_start = chaos::TiledMapTools::IsPlayerStart(geometric_object);
				if (player_start)
				{
					PlayerStartInstance * player_start_instance = new PlayerStartInstance;
					if (player_start_instance != nullptr)
					{
						if (player_start_instance->Initialize(geometric_object))
							player_starts.push_back(player_start_instance);
						else
							delete player_start_instance;
					}
				}

				// zones
				chaos::TiledMap::GeometricObjectRectangle * rectangle = geometric_object->GetObjectRectangle();
				chaos::TiledMap::GeometricObjectEllipse   * ellipse   = geometric_object->GetObjectEllipse();
				if (rectangle != nullptr || ellipse != nullptr)
				{

				}
	
				// zones
				chaos::TiledMap::GeometricObjectTile * tile = geometric_object->GetObjectTile();
				if (tile != nullptr)
				{

				}
					


			}

			chaos::box2 world_bounds;
			chaos::TiledMapTools::FindExplicitWorldBounds(object_layer, world_bounds);

			return true;
		}

		bool LayerInstance::InitializeLayer(chaos::TiledMap::TileLayer * tile_layer)
		{
			// find render material
			chaos::GPURenderMaterial * render_material = FindRenderMaterial(material_name.c_str());

			// create a particle layer
			particle_layer = new chaos::ParticleLayer(new chaos::TypedParticleLayerDesc<chaos::ParticleDefault::ParticleTrait>);
			if (particle_layer == nullptr)
				return false;
			particle_layer->SetRenderMaterial(render_material);

			return true;
		}

		bool LayerInstance::DoTick(double delta_time)
		{

			return true;
		}
		
		int LayerInstance::DoDisplay(chaos::GPUProgramProviderBase const * uniform_provider, chaos::RenderParams const & render_params) const
		{

			return 0;
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
			// tick the particle manager
			if (particle_manager == nullptr)
				return 0;
			particle_manager->Tick(delta_time);
			// tick all layer instances
			size_t count = layer_instances.size();
			for (size_t i = 0; i < count; ++i)
				layer_instances[i]->Tick(delta_time);

			return true;
		}

		int LevelInstance::DoDisplay(chaos::GPUProgramProviderBase const * uniform_provider, chaos::RenderParams const & render_params) const
		{
			int result = 0;
			// display particle manager
			if (particle_manager != nullptr)
				result += particle_manager->Display(uniform_provider, render_params);
			// display all layer instances
			size_t count = layer_instances.size();
			for (size_t i = 0; i < count; ++i)
				result += layer_instances[i]->Display(uniform_provider, render_params);
			return result;
		}

		bool LevelInstance::Initialize(death::Game * in_game)
		{
			assert(in_game != nullptr);
			// create a particle manager
			if (!CreateParticleManager(in_game))
				return false;
			// create a the layers instances
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
			chaos::TiledMap::Map * tiled_map = GetTiledMap();

			// handle layers ordered by Z-Order
			size_t count = tiled_map->GetLayerCount();
			for (size_t i = 0; i < count; ++i)
			{
				// create a layer_instance per layer
				chaos::TiledMap::LayerBase * layer = tiled_map->FindLayerByZOrder(i);
				if (layer == nullptr)
					continue;
				LayerInstance * layer_instance = new LayerInstance;
				if (layer_instance == nullptr)
					continue;
				// initialize the level_instance
				if (!layer_instance->Initialize(layer))
				{
					delete(layer_instance);
					continue;
				}
				// insert the layer_instance
				layer_instances.push_back(layer_instance);
			}
			return true;
		}

	}; // namespace TiledMap

}; // namespace death

