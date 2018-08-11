#include "Ludum42Level.h"


death::GameLevelInstance * LudumGameplayLevel::DoCreateLevelInstance()
{
	return new LudumGameplayLevelInstance(game);
}

death::GameLevelInstance * LudumNarrativeLevel::DoCreateLevelInstance()
{
	return new LudumNarrativeLevelInstance(game);
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

LudumNarrativeLevel::LudumNarrativeLevel(class LudumGame * in_game):
	LudumLevel(in_game)
{
}

LudumNarrativeLevelInstance::LudumNarrativeLevelInstance(class LudumGame * in_game):
	LudumLevelInstance(in_game)
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

LudumNarrativeLevel * LudumNarrativeLevelInstance::GetLudumLevel()
{
	return dynamic_cast<LudumNarrativeLevel*>(GetLevel());
}

LudumNarrativeLevel const * LudumNarrativeLevelInstance::GetLudumLevel() const
{
	return dynamic_cast<LudumNarrativeLevel const *>(GetLevel());
}

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


}

void LudumGameplayLevelInstance::OnLevelStarted()
{
	allocations.clear();

	LudumGameplayLevel const * ludum_level = GetLudumLevel();
	if (ludum_level == nullptr)
		return;

	chaos::ParticleLayer * layer = game->GetParticleManager()->FindLayer(LudumGame::GAMEOBJECT_LAYER_ID);
	if (layer == nullptr)
		return;

	chaos::TiledMap::Map const * tiled_map = ludum_level->tiled_map.get();
	if (tiled_map == nullptr)
		return;

	chaos::BitmapAtlas::TextureArrayAtlas const * atlas = game->GetTextureAtlas();
	if (atlas == nullptr)
		return;

	chaos::BitmapAtlas::BitmapSet const * bitmap_set = atlas->GetBitmapSet("sprites");
	if (bitmap_set == nullptr)
		return;

	for (size_t i = 0 ; tiled_map->tile_layers.size(); ++i)
	{
		chaos::TiledMap::TileLayer * tile_layer = tiled_map->tile_layers[i].get();
		if (tile_layer == nullptr)
			continue;

		size_t tile_count = tile_layer->GetNonEmptyTileCount();
		if (tile_count == 0)
			continue;

		chaos::ParticleAllocation * allocation = layer->SpawnParticles((int)tile_count);
		if (allocation == nullptr)
			continue;


		size_t k = 0;
		chaos::ParticleAccessor<ParticleObject> particles = allocation->GetParticleAccessor<ParticleObject>();


		


		glm::ivec2 size = tile_layer->size;
		for (size_t j = 0 ; j < tile_layer->tile_indices.size() ; ++j)
		{
			int tile_indice = tile_layer->tile_indices[j];
			if (tile_indice <= 0)
				continue;
			
			glm::ivec2 pos = tile_layer->GetTileCoordinate(j);

			chaos::TiledMap::TileInfo tile_info = tiled_map->FindTileInfo(tile_indice);
			if (tile_info.tiledata != nullptr)
			{
				ParticleObject & object = particles[k++];

				//bitmap_set->GetEntry(tile_info.tiledata->)
				


				tile_indice = tile_indice;
			}
		}
	}


	

}

// =============================================================

void LudumNarrativeLevelInstance::OnLevelEnded()
{
}

void LudumNarrativeLevelInstance::OnLevelStarted()
{
}