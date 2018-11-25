#include "Ludum42OneLevel.h"
#include "Ludum42OneGame.h"

#include <chaos/GLMTools.h>
#include <chaos/ParticleDefault.h>

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
	game->SetPlayerAllocation(nullptr);
}

void LudumLevelInstance::OnLevelStarted()
{
	death::TiledMap::PlayerStartObject * player_start = FindPlayerStart(nullptr);
	if (player_start == nullptr)
		return;

	death::TiledMap::LayerInstance * layer_instance = player_start->GetLayerInstance();
	if (layer_instance == nullptr)
		return;

	chaos::BitmapAtlas::TextureArrayAtlas const * texture_atlas = game->GetTextureAtlas();
	if (texture_atlas == nullptr)
		return;

	chaos::BitmapAtlas::FolderInfo const * folder_info = texture_atlas->GetFolderInfo("sprites");
	if (folder_info == nullptr)
		return;

	std::string const * bitmap_name = player_start->GetGeometricObject()->FindPropertyString("BITMAP_NAME");
	if (bitmap_name == nullptr)
		return;

	chaos::BitmapAtlas::BitmapInfo const * info = folder_info->GetBitmapInfo(bitmap_name->c_str());
	if (info == nullptr)
		return;

	chaos::ParticleAllocation * player_allocation = layer_instance->CreateParticleAllocation();
	if (!player_allocation->AddParticles(1))
		return;

	chaos::ParticleAccessor<chaos::ParticleDefault::Particle> particle_accessor = player_allocation->GetParticleAccessor<chaos::ParticleDefault::Particle>();

	chaos::ParticleDefault::Particle & player_particle = particle_accessor[0];

	chaos::box2 player_bounding_box;

	chaos::TiledMap::GeometricObjectSurface const * object_surface = player_start->GetGeometricObject()->GetObjectSurface();
	if (object_surface != nullptr)
		player_bounding_box = object_surface->GetBoundingBox();
	else
	{
		player_bounding_box.position = player_start->GetGeometricObject()->position;
		player_bounding_box.half_size = glm::vec2(info->width, info->height) * 0.5f;
	}

	player_particle.bounding_box = player_bounding_box;
	player_particle.texcoords = chaos::ParticleTools::GetParticleTexcoords(*info, texture_atlas->GetAtlasDimension());
	player_particle.color = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);


	game->SetPlayerAllocation(player_allocation);
}


