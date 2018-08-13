#include "Ludum42Level.h"

#include <chaos/GLMTools.h>

death::GameLevelInstance * LudumGameplayLevel::DoCreateLevelInstance()
{
	return new LudumGameplayLevelInstance(game);
}


// =============================================================

LudumLevel::LudumLevel(class LudumGame * in_game):
	game(in_game)
{
}

LudumLevelInstance::LudumLevelInstance(class LudumGame * in_game):
	game(in_game)
{
}

LudumGameplayLevel::LudumGameplayLevel(class LudumGame * in_game):
	LudumLevel(in_game)
{
}

LudumGameplayLevelInstance::LudumGameplayLevelInstance(class LudumGame * in_game):
	LudumLevelInstance(in_game)
{
}

// =============================================================

LudumGameplayLevel * LudumGameplayLevelInstance::GetLudumLevel()
{
	return dynamic_cast<LudumGameplayLevel*>(GetLevel());
}

LudumGameplayLevel const * LudumGameplayLevelInstance::GetLudumLevel() const
{
	return dynamic_cast<LudumGameplayLevel const *>(GetLevel());
}

// =============================================================

void LudumGameplayLevelInstance::OnLevelEnded()
{
	allocations.clear();
	game->UnSpawnPlayer();
}

chaos::ParticleLayer * LudumGameplayLevelInstance::ObjectToParticleLayer(int object_type) const
{
	int layer_id = -1;
	if (object_type == LudumGame::OBJECT_TYPE_PLAYER)
		layer_id = LudumGame::PLAYER_LAYER_ID;
	else if (object_type == LudumGame::OBJECT_TYPE_PLANET)
		layer_id = LudumGame::PLANETS_LAYER_ID;

	if (layer_id > 0)
		return game->GetParticleManager()->FindLayer(layer_id);

	return nullptr;
}

chaos::ParticleAllocation * LudumGameplayLevelInstance::FindOrAllocationForObjectType(int object_type)
{
	// find the allocation
	auto it = allocations.find(object_type);
	if (it != allocations.end())
		return it->second.get();

	// create the allocation
	chaos::ParticleLayer * particle_layer = ObjectToParticleLayer(object_type);
	if (particle_layer == nullptr)
		return nullptr;

	chaos::ParticleAllocation * allocation = particle_layer->SpawnParticles(0);
	if (allocation == nullptr)
		return nullptr;
	allocations[object_type] = allocation;

	return allocation;
}










// returns false, if the object does not have the flag
bool HasExplicitFlag(chaos::TiledMap::GeometricObject const * object_geometric, char const * name, char const * type, char const * property_name)
{
	// name is an indicator
	if (name != nullptr && object_geometric->name == name)
		return true;
	// type is an indicator
	if (type != nullptr && object_geometric->type == type)
		return true;
	// search in properties
	if (property_name != nullptr)
	{
		chaos::TiledMap::Property const * property = object_geometric->FindProperty(property_name);
		if (property != nullptr)
		{
			bool const * property_bool = property->GetBoolProperty();
			if (property_bool != nullptr)
				return *property_bool;

			int const * property_int = property->GetIntProperty();
			if (property_int != nullptr)
				return (*property_int > 0);				
		}
	}
	return false;
}






bool HasExplicitWorldOrigin(chaos::TiledMap::GeometricObject const * object_geometric)
{
	return HasExplicitFlag(object_geometric, "world_origin", "world_origin", "WORLD_ORIGIN");
}

bool GetExplicitWorldOrigin(chaos::TiledMap::GeometricObject const * object_geometric, glm::vec2 & result) // expressed in layer coordinates
{
	if (object_geometric == nullptr)
		return false;
	if (HasExplicitWorldOrigin(object_geometric))
	{
		result = object_geometric->position;
		return true;
	}
	return false;
}

bool FindExplicitWorldOrigin(chaos::TiledMap::ObjectLayer const * object_layer, glm::vec2 & result)
{
	if (object_layer == nullptr)
		return false;
	for (size_t i = 0 ; i < object_layer->geometric_objects.size(); ++i)
	{
		chaos::TiledMap::GeometricObject const * object = object_layer->geometric_objects[i].get();
		if (GetExplicitWorldOrigin(object, result))
		{
			result += object_layer->offset;
			return true;
		}
	}
	return false;
}

bool FindExplicitWorldOrigin(chaos::TiledMap::Map const * tiled_map, glm::vec2 & result) // expressed in map coordinates
{
	if (tiled_map == nullptr)
		return false;
	for (size_t i = 0 ; i < tiled_map->object_layers.size(); ++i)
	{
		chaos::TiledMap::ObjectLayer const * object_layer = tiled_map->object_layers[i].get();
		if (FindExplicitWorldOrigin(object_layer, result))
			return true;
	}
	return false;
}










bool HasExplicitWorldBounds(chaos::TiledMap::GeometricObject const * object_geometric)
{
	return HasExplicitFlag(object_geometric, "world_bounds", "world_bounds", "WORLD_BOUNDS");
}

bool GetExplicitWorldBounds(chaos::TiledMap::GeometricObject const * object_geometric, chaos::box2 & result)  // expressed in layer coordinates
{
	if (object_geometric == nullptr)
		return false;
	chaos::TiledMap::GeometricObjectSurface const * object_surface = object_geometric->GetObjectSurface();
	if (object_surface == nullptr)
		return false;
	if (HasExplicitWorldBounds(object_surface))
	{
		result = object_surface->GetBoundingBox();
		return true;
	}
	return false;
}

bool FindExplicitWorldBounds(chaos::TiledMap::ObjectLayer const * object_layer, chaos::box2 & result)
{
	if (object_layer == nullptr)
		return false;
	for (size_t i = 0 ; i < object_layer->geometric_objects.size(); ++i)
	{
		chaos::TiledMap::GeometricObject const * object = object_layer->geometric_objects[i].get();
		if (GetExplicitWorldBounds(object, result))
		{
			result.position += object_layer->offset;
			return true;
		}
	}
	return false;
}

bool FindExplicitWorldBounds(chaos::TiledMap::Map const * tiled_map, chaos::box2 & result) // expressed in map coordinates
{
	if (tiled_map == nullptr)
		return false;
	for (size_t i = 0 ; i < tiled_map->object_layers.size(); ++i)
	{
		chaos::TiledMap::ObjectLayer const * object_layer = tiled_map->object_layers[i].get();
		if (FindExplicitWorldBounds(object_layer, result))
			return true;
	}
	return false;
}






























#if 0

class ParticleLayerOwner : public ReferencedObject
{
	void ParticleLayer * AddParticleLayer(float render_order)
	{

		return nullptr;
	}
};

class ParticleLayer : public ParticleLayerOwner
{
	AddParticleGroup(ParticleGroup * group)

};


class ParticleGroup : public ReferencedObject
{
	ParticleAllocation * SpawnParticles(size_t count);

	
};



class ParticleManager : public ParticleLayerOwner
{


};



void InitializeParticleManager()
{
	ParticleManager * manager;




};


class Loader
{
public:

	chaos::TiledMap::Map const * tiled_map = nullptr;


	void OnTiled(chaos::TiledMap::GeometricObjectTile const * object_tile, chaos::TiledMap::TileInfo tile_info);
	
	entry

};
#endif








void LudumGameplayLevelInstance::OnLevelStarted()
{
	allocations.clear();

	LudumGameplayLevel const * ludum_level = GetLudumLevel();
	if (ludum_level == nullptr)
		return;

	chaos::TiledMap::Map const * tiled_map = ludum_level->tiled_map.get();
	if (tiled_map == nullptr)
		return;


	chaos::BitmapAtlas::TextureArrayAtlas const * texture_atlas = game->GetTextureAtlas();
	if (texture_atlas == nullptr)
		return;

	chaos::BitmapAtlas::BitmapSet const * bitmap_set = texture_atlas->GetBitmapSet("sprites");
	if (bitmap_set == nullptr)
		return;

	// compute the new world
	chaos::box2 world_bounds; 
	bool explicit_world_bounds = FindExplicitWorldBounds(tiled_map, world_bounds);

	glm::vec2 world_origin; 
	bool explicit_world_origin = FindExplicitWorldOrigin(tiled_map, world_origin);
	












	for (size_t i = 0 ; i < tiled_map->object_layers.size(); ++i)
	{
		chaos::TiledMap::ObjectLayer const * object_layer = tiled_map->object_layers[i].get();
		if (object_layer == nullptr)
			continue;

		for (size_t j = 0 ; j < object_layer->geometric_objects.size(); ++j)
		{
			chaos::TiledMap::GeometricObject const * object = object_layer->geometric_objects[j].get();
			if (object == nullptr)
				continue;

			chaos::TiledMap::GeometricObjectTile const * object_tile = object->GetObjectTile();
			if (object_tile == nullptr)
				continue;

			chaos::TiledMap::TileInfo tile_info = tiled_map->FindTileInfo(object_tile->gid);
			if (tile_info.tiledata == nullptr)
				continue;
		
			chaos::BitmapAtlas::BitmapEntry const * entry = bitmap_set->GetEntry(tile_info.tiledata->atlas_key.c_str());
			if (entry == nullptr)
				continue;

			chaos::box2 bounding_box = object_tile->GetBoundingBox();

			ParticleObject new_particle;
			new_particle.bounding_box = bounding_box;
			new_particle.texcoords = chaos::ParticleTools::GetParticleTexcoords(*entry, texture_atlas->GetAtlasDimension());
			new_particle.color = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);

			int object_type = 0;

			int const * prop_object_type = tile_info.tiledata->FindPropertyInt("OBJECT_TYPE");
			if (prop_object_type == nullptr)
				continue;

			object_type = *prop_object_type;


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


	//

	for (size_t i = 0 ; i < tiled_map->tile_layers.size(); ++i)
	{
		chaos::TiledMap::TileLayer const * tile_layer = tiled_map->tile_layers[i].get();
		if (tile_layer == nullptr)
			continue;

		glm::vec2 tile_size = chaos::GLMTools::RecastVector<glm::vec2>(tiled_map->tile_size);
		for (size_t j = 0 ; j < tile_layer->tile_indices.size() ; ++j)
		{
			int gid = tile_layer->tile_indices[j];
			if (gid <= 0)
				continue;

			chaos::TiledMap::TileInfo tile_info = tiled_map->FindTileInfo(gid);
			if (tile_info.tiledata != nullptr)
			{	
				chaos::BitmapAtlas::BitmapEntry const * entry = bitmap_set->GetEntry(tile_info.tiledata->atlas_key.c_str());
				if (entry == nullptr)
					continue;

				glm::ivec2 tile_coord = 
					tile_layer->GetTileCoordinate(j);

				// the coordinate of a tile is the BOTTOMLEFT (greater Y, because Y axis is oriented UP)
				glm::vec2 bottomleft = 
					chaos::GLMTools::RecastVector<glm::vec2>(tile_coord) * tile_size +
					glm::vec2(0.0f, tile_size.y);

				glm::vec2 topright = bottomleft;
				topright.x += tile_info.tiledata->image_size.x;
				topright.y -= tile_info.tiledata->image_size.y; // SHUXXX = not clear could be entry->size !!! (but with manual atlas, not a good idea)


				ParticleObject new_particle;
				new_particle.bounding_box = chaos::box2(std::make_pair(bottomleft, topright));
				new_particle.texcoords = chaos::ParticleTools::GetParticleTexcoords(*entry, texture_atlas->GetAtlasDimension());
				new_particle.color = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);




				














				int default_object_type = 0;
				int object_type = 0;

				int const * prop_object_type = tile_info.tiledata->FindPropertyInt("OBJECT_TYPE");
				if (prop_object_type == nullptr)
					continue;

				object_type = *prop_object_type;

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
	}

	world_box = world_bounds;

	world_box.position.y = -world_box.position.y;
}


