#include <death/TiledMapLevel.h>
#include <death/Game.h>
#include <chaos/TiledMapTools.h>
#include <chaos/ParticleDefault.h>

namespace death
{
	namespace TiledMap
	{

		// =====================================
		// LevelInstanc implementation
		// =====================================



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

#define DEATH_EMPTY_TOKEN
#define DEATH_DOCREATE_OBJECT(result_type, func_name, declared_parameters, calling_parameters)\
		result_type * Level::Do##func_name(declared_parameters)\
		{\
			return new result_type(calling_parameters);\
		}
#define DEATH_CREATE_OBJECT(result_type, func_name, declared_parameters, constructor_parameters, initialization_parameters)\
		result_type * Level::func_name(declared_parameters)\
		{\
			result_type * result = Do##func_name(constructor_parameters);\
			if (result == nullptr)\
				return nullptr;\
			if (!result->Initialize(initialization_parameters))\
			{\
				delete result;\
				return nullptr;\
			}\
			return result;\
		}

		DEATH_DOCREATE_OBJECT(PlayerStartObject, CreatePlayerStart, DEATH_EMPTY_TOKEN, DEATH_EMPTY_TOKEN);		
		DEATH_CREATE_OBJECT(PlayerStartObject, CreatePlayerStart, chaos::TiledMap::GeometricObject * in_geometric_object, DEATH_EMPTY_TOKEN, in_geometric_object);

		DEATH_DOCREATE_OBJECT(LayerInstance, CreateLayerInstance, LevelInstance * in_level_instance, in_level_instance);
		DEATH_CREATE_OBJECT(LayerInstance, CreateLayerInstance, LevelInstance * in_level_instance BOOST_PP_COMMA() chaos::TiledMap::LayerBase * in_layer, in_level_instance, in_layer);

#undef DEATH_CREATE_OBJECT
#undef DEATH_DOCREATE_OBJECT
#undef DEATH_EMPTY_TOKEN

		// =====================================
		// PlayerStartObject implementation
		// =====================================

		bool PlayerStartObject::Initialize(chaos::TiledMap::GeometricObject * in_geometric_object)
		{
			geometric_object = in_geometric_object;


			return true;
		}

		// =====================================
		// LayerInstance implementation
		// =====================================

		LayerInstance::LayerInstance(LevelInstance * in_level_instance):
			level_instance(in_level_instance)
		{
			assert(in_level_instance != nullptr);
		}

		Level * LayerInstance::GetTypedLevel()
		{
			if (level_instance == nullptr)
				return nullptr;
			return level_instance->GetTypedLevel();
		}

		Level const * LayerInstance::GetTypedLevel() const
		{
			if (level_instance == nullptr)
				return nullptr;
			return level_instance->GetTypedLevel();
		}

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
				return InitializeLayer(image_layer);

			chaos::TiledMap::ObjectLayer * object_layer = in_layer->GetObjectLayer();
			if (object_layer != nullptr)
				return InitializeLayer(object_layer);

			chaos::TiledMap::TileLayer * tile_layer = in_layer->GetTileLayer();
			if (tile_layer != nullptr)
				return InitializeLayer(tile_layer);

			return false;
		}

		bool LayerInstance::InitializeLayer(chaos::TiledMap::ImageLayer * image_layer)
		{

			return true;
		}

		bool LayerInstance::InitializeLayer(chaos::TiledMap::ObjectLayer * object_layer)
		{
			Level * level = GetTypedLevel();

			size_t count = object_layer->geometric_objects.size();
			for (size_t i = 0; i < count; ++i)
			{
				chaos::TiledMap::GeometricObject * geometric_object = object_layer->geometric_objects[i].get();
				if (geometric_object == nullptr)
					continue;
				
				// player start ?
				if (chaos::TiledMapTools::IsPlayerStart(geometric_object))
				{
					PlayerStartObject * player_start = level->CreatePlayerStart(geometric_object);
					if (player_start != nullptr)
						player_starts.push_back(player_start);
				}
#if 0
				// zones
				chaos::TiledMap::GeometricObjectRectangle * rectangle = geometric_object->GetObjectRectangle();
				chaos::TiledMap::GeometricObjectEllipse   * ellipse   = geometric_object->GetObjectEllipse();
				if (rectangle != nullptr || ellipse != nullptr)
				{
					if (rectangle != nullptr)
					{
						chaos::box2 bounding_box = rectangle->GetBoundingBox();

						auto corners = bounding_box.GetCorners();
						rectangle = rectangle;
					}

					if (ellipse != nullptr)
					{
						chaos::box2 bounding_box = ellipse->GetBoundingBox();

						auto corners = bounding_box.GetCorners();
						ellipse = ellipse;

					}

				}
	
				// zones
				chaos::TiledMap::GeometricObjectTile * tile = geometric_object->GetObjectTile();
				if (tile != nullptr)
				{

				}
					
#endif

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
			if (particle_layer != nullptr)
				particle_layer->Display(uniform_provider, render_params);

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
			// create a the layers instances
			if (!CreateLayerInstances(in_game))
				return false;
			// create a particle manager
			if (!CreateParticleManager(in_game))
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

			chaos::TiledMap::Map * tiled_map = GetTiledMap();

			// handle layers ordered by Z-Order
			size_t count = tiled_map->GetLayerCount();
			for (size_t i = 0; i < count; ++i)
			{
				// get the chaos::LayerBase object per Z-order
				chaos::TiledMap::LayerBase * layer = tiled_map->FindLayerByZOrder(i);
				if (layer == nullptr)
					continue;
				// create and store the layer_instance
				LayerInstance * layer_instance = level->CreateLayerInstance(this, layer);
				if (layer_instance != nullptr)
					layer_instances.push_back(layer_instance);	
			}
			return true;
		}

	}; // namespace TiledMap

}; // namespace death

