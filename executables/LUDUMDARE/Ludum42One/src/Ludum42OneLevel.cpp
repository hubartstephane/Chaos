#include "Ludum42OneLevel.h"
#include "Ludum42OneGame.h"

#include <chaos/GLMTools.h>

// =============================================================
// LudumLevel implementation
// =============================================================

LudumLevel::LudumLevel()
{

}

death::GameLevelInstance * LudumLevel::DoCreateLevelInstance(death::Game * in_game)
{
	return new LudumLevelInstance(dynamic_cast<LudumGame *>(in_game));
}

// =============================================================
// LudumLevelInstance implementation
// =============================================================

LudumLevelInstance::LudumLevelInstance(LudumGame * in_game):
	game(in_game)
{
	assert(in_game != nullptr); 
}



LudumLevel * LudumLevelInstance::GetTypedLevel()
{
	return dynamic_cast<LudumLevel*>(GetLevel());
}

LudumLevel const * LudumLevelInstance::GetTypedLevel() const
{
	return dynamic_cast<LudumLevel const *>(GetLevel());
}

void LudumLevelInstance::OnLevelEnded()
{
	allocations.clear();
	game->UnSpawnPlayer();
}

chaos::ParticleLayer * LudumLevelInstance::ObjectToParticleLayer(int object_type) const
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

chaos::ParticleAllocation * LudumLevelInstance::FindOrAllocationForObjectType(int object_type)
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


void LudumLevelInstance::OnLevelStarted()
{


	allocations.clear();

	LudumLevel const * ludum_level = GetTypedLevel();
	if (ludum_level == nullptr)
		return;

	chaos::TiledMap::Map const * tiled_map = ludum_level->tiled_map.get();
	if (tiled_map == nullptr)
		return;

	auto layer1 = tiled_map->FindLayerByZOrder(2);
	auto layer2 = tiled_map->FindLayerByName("planets");
	auto layer3 = tiled_map->FindLayerByName("planetsXX");

	int cc = tiled_map->GetLayerCount();

	chaos::BitmapAtlas::TextureArrayAtlas const * texture_atlas = game->GetTextureAtlas();
	if (texture_atlas == nullptr)
		return;

	chaos::BitmapAtlas::FolderInfo const * folder_info = texture_atlas->GetFolderInfo("sprites");
	if (folder_info == nullptr)
		return;

	// compute the new world
	chaos::box2 world_bounds; 
	bool explicit_world_bounds = chaos::TiledMapTools::FindExplicitWorldBounds(tiled_map, world_bounds);

	glm::vec2 world_origin = glm::vec2(0.0f, 0.0f);
	bool explicit_world_origin = chaos::TiledMapTools::FindExplicitWorldOrigin(tiled_map, world_origin);
	

	explicit_world_bounds = explicit_world_origin = false;

	if (!explicit_world_bounds)
		world_bounds = chaos::box2();
	if (!explicit_world_origin)
		world_origin = glm::vec2(0.0f, 0.0f);




#if 1



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
		
			chaos::BitmapAtlas::BitmapInfo const * info = folder_info->GetBitmapInfo(tile_info.tiledata->atlas_key.c_str());
			if (info == nullptr)
				continue;

			chaos::box2 bounding_box = object_tile->GetBoundingBox();

			ParticleObject new_particle;
			new_particle.bounding_box = bounding_box;
			new_particle.texcoords = chaos::ParticleTools::GetParticleTexcoords(*info, texture_atlas->GetAtlasDimension());
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

#endif

	//

	for (size_t i = 0 ; i < tiled_map->tile_layers.size(); ++i)
	{
		chaos::TiledMap::TileLayer const * tile_layer = tiled_map->tile_layers[i].get();
		if (tile_layer == nullptr)
			continue;

		for (size_t j = 0 ; j < tile_layer->tile_indices.size() ; ++j)
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

				glm::ivec2 tile_coord = tile_layer->GetTileCoordinate(j);

				ParticleObject new_particle;
				new_particle.bounding_box = tile_layer->GetTileBoundingBox(tile_coord, tile_info.tiledata->image_size);
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
	}

	world_box = world_bounds;

	world_box.position.y = -world_box.position.y;
}


