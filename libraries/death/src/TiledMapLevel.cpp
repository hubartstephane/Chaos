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
			chaos::GPURenderMaterial * result = resource_manager->FindRenderMaterial(material_name);
			if (result != nullptr)
				return result;
			// default material else where
			return chaos::ParticleDefault::GenDefaultParticleMaterial();
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

			// empty the bounding box
			bounding_box = chaos::box2();

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
			// early exit for empty tile_layer
			size_t count = tile_layer->tile_indices.size();
			if (count == 0)
				return true;

			// find render material
			chaos::GPURenderMaterial * render_material = FindRenderMaterial(material_name.c_str());

			// create a particle layer
			particle_layer = new chaos::ParticleLayer(new chaos::TypedParticleLayerDesc<chaos::ParticleDefault::ParticleTrait>);
			if (particle_layer == nullptr)
				return false;
			particle_layer->SetRenderMaterial(render_material);

			return true;
		}






#if 0
		glm::vec2 tile_size = chaos::GLMTools::RecastVector<glm::vec2>(tiled_map->tile_size);
		for (size_t j = 0; j < tile_layer->tile_indices.size(); ++j)
		{
			int gid = tile_layer->tile_indices[j];
			if (gid <= 0)
				continue;

			chaos::TiledMap::TileInfo tile_info = tiled_map->FindTileInfo(gid);
			if (tile_info.tiledata == nullptr)
				continue;

			if (tile_info.tiledata != nullptr)
			{
				chaos::BitmapAtlas::BitmapInfo const * info = folder_info->GetBitmapInfo(tile_info.tiledata->atlas_key.c_str());
				if (info == nullptr)
					continue;

				glm::ivec2 tile_coord =
					tile_layer->GetTileCoordinate(j);

				// the coordinate of a tile is the BOTTOMLEFT (greater Y, because Y axis is oriented UP)
				glm::vec2 bottomleft =
					chaos::GLMTools::RecastVector<glm::vec2>(tile_coord) * tile_size +
					glm::vec2(0.0f, tile_size.y);

				glm::vec2 topright = bottomleft;
				topright.x += tile_info.tiledata->image_size.x;
				topright.y -= tile_info.tiledata->image_size.y; //= not clear could be info->size !!! (but with manual atlas, not a good idea)


				ParticleObject new_particle;
				new_particle.bounding_box = chaos::box2(std::make_pair(bottomleft, topright));
				new_particle.texcoords = chaos::ParticleTools::GetParticleTexcoords(*info, texture_atlas->GetAtlasDimension());
				new_particle.color = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);



















				int default_object_type = 0;
				int object_type = 0;

				int const * prop_object_type = tile_info.tiledata->FindPropertyInt("OBJECT_TYPE");
				//		if (prop_object_type == nullptr)
				//			continue;

				if (prop_object_type != nullptr)
					object_type = *prop_object_type;
				else
					object_type = LudumGame::OBJECT_TYPE_PLANET;

				if (object_type == LudumGame::OBJECT_TYPE_PLAYER)
				{
					ParticlePlayer player_particle;
					(ParticleObject&)player_particle = new_particle;

					int const * atlas_size_x = tile_info.tiledata->FindPropertyInt("ATLAS_SIZE_X");
					if (atlas_size_x != nullptr)
						player_particle.atlas_dimension.x = *atlas_size_x;

					int const * atlas_size_y = tile_info.tiledata->FindPropertyInt("ATLAS_SIZE_Y");
					if (atlas_size_y != nullptr)
						player_particle.atlas_dimension.y = *atlas_size_y;

					float const * atlas_frequency = tile_info.tiledata->FindPropertyFloat("ATLAS_FREQUENCY");
					if (atlas_frequency != nullptr)
						player_particle.frequency = *atlas_frequency;

					int const * skip_last = tile_info.tiledata->FindPropertyInt("ATLAS_SKIP_LAST");
					if (skip_last != nullptr)
						player_particle.skip_last = *skip_last;

					//player_particle.delta_image = abs(rand());

					game->SpawnPlayer(player_particle);
					continue;
				}


				// create an allocation
				chaos::ParticleAllocation * allocation = FindOrAllocationForObjectType(object_type);
				if (allocation == nullptr)
					continue;

				if (!allocation->AddParticles(1))
					continue;

				if (!explicit_world_bounds)
					world_bounds = world_bounds | new_particle.bounding_box;

				chaos::ParticleAccessor<ParticleObject> particles = allocation->GetParticleAccessor<ParticleObject>();
				new_particle.bounding_box.position.y = -new_particle.bounding_box.position.y;
				particles[particles.GetCount() - 1] = new_particle;
			}
		}
#endif
















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

