#include "Ludum45Level.h"
#include "Ludum45LevelInstance.h"
#include "Ludum45Game.h"
#include "Ludum45Player.h"
#include "Ludum45GameInstance.h"

#include <chaos/GLMTools.h>
#include <chaos/ParticleDefault.h>
#include <chaos/GeometryFramework.h>


// =============================================================
// LudumLevel implementation
// =============================================================


bool BonusSpawnerTriggerObject::IsAdditionalParticlesCreationEnabled() const
{


	return true;
}

bool BonusSpawnerTriggerObject::Initialize()
{
	if (!death::TiledMap::TriggerObject::Initialize())
		return false;
	trigger_once = true;

	bonus_type = geometric_object->FindPropertyString("BONUS_TYPE", "");
	return true;
}

bool BonusSpawnerTriggerObject::OnCameraCollisionEvent(double delta_time, chaos::box2 const & camera_box, int event_type)
{
	if (event_type != TriggerObject::COLLISION_STARTED)
		return false;



	// search the layer for bonus
	death::TiledMap::LayerInstance * bonus_layer_instance = GetLayerInstance()->GetTiledLevelInstance()->FindLayerInstance("Bonus");
	if (bonus_layer_instance == nullptr)
		return true;

	// search the atlas
	chaos::BitmapAtlas::TextureArrayAtlas const * atlas = bonus_layer_instance->GetGame()->GetTextureAtlas();
	if (atlas == nullptr)
		return true;

	// cast in a surface
	chaos::TiledMap::GeometricObjectSurface const * surface = geometric_object->GetObjectSurface();
	if (surface == nullptr)
		return true;

	// search BitmapLayout for Enemy
	chaos::BitmapAtlas::FolderInfo const * bitmap_set = atlas->GetFolderInfo("sprites");
	if (bitmap_set == nullptr)
		return true;

	// create an allocation for all bonus we are about to create
	chaos::ParticleAllocationBase * allocation = bonus_layer_instance->CreateParticleAllocation();
	if (allocation == nullptr)
		return true;

















	return true;
}



// =============================================================
// LudumLevel implementation
// =============================================================

LudumLevel::LudumLevel()
{

}

death::GameLevelInstance * LudumLevel::DoCreateLevelInstance(death::Game * in_game)
{
	return new LudumLevelInstance(auto_cast(in_game));
}

bool LudumLevel::FinalizeLayerParticles(death::TiledMap::LayerInstance * layer_instance, chaos::ParticleAllocationBase * allocation)
{
	if (!death::TiledMap::Level::FinalizeLayerParticles(layer_instance, allocation))
		return false;



	return true;
}


chaos::ParticleLayerBase * LudumLevel::CreateParticleLayer(death::TiledMap::LayerInstance * layer_instance)
{
	LudumGame * ludum_game = auto_cast(layer_instance->GetGame());

	std::string const & layer_name = layer_instance->GetTiledLayer()->name;

	bool is_player_and_camera = (chaos::StringTools::Stricmp(layer_name, "PlayerAndCamera") == 0);
	if (is_player_and_camera)
	{
		ParticlePlayerTrait::LayerTrait layer_trait;
		layer_trait.game = ludum_game;
		return new chaos::ParticleLayer<ParticlePlayerTrait>(layer_trait);
	}

	bool is_fire = (chaos::StringTools::Stricmp(layer_name, "fire") == 0);
	if (is_fire)
	{
		ParticleFireTrait::LayerTrait fire_trait;
		fire_trait.game = ludum_game;
		return new chaos::ParticleLayer<ParticleFireTrait>(fire_trait);
	}

	bool is_bonus = (chaos::StringTools::Stricmp(layer_name, "bonus") == 0);
	if (is_bonus)
	{
		ParticleBonusTrait::LayerTrait bonus_trait;
		bonus_trait.game = ludum_game;
		return new chaos::ParticleLayer<ParticleBonusTrait>(bonus_trait);
	}

#if 0
	bool is_enemies = (layer_name == "Enemies");
	if (is_enemies)
	{
		ParticleEnemyTrait::LayerTrait enemy_trait;
		enemy_trait.game = ludum_game;
		return new chaos::ParticleLayer<ParticleEnemyTrait>(enemy_trait);
	}
#endif















	return death::TiledMap::Level::CreateParticleLayer(layer_instance);
}


death::TiledMap::GeometricObject * LudumLevel::DoCreateGeometricObject(death::TiledMap::LayerInstance * in_layer_instance, chaos::TiledMap::GeometricObject * in_geometric_object)
{
	chaos::TiledMap::GeometricObjectSurface * surface_object = in_geometric_object->GetObjectSurface();

	if (surface_object != nullptr)
	{
		if (chaos::TiledMapTools::HasFlag(surface_object, nullptr, nullptr, "BONUS_SPAWNER")) // shuludum : it is better to rely on a type of an object that to rely on its name
			return new BonusSpawnerTriggerObject(in_layer_instance, surface_object);          //            here we test for the property 'BONUS_SPAWNER' . Maybe the cleaner. Beware HasFlag method does not recurse like FindPropery(...)





	}
	return death::TiledMap::Level::DoCreateGeometricObject(in_layer_instance, in_geometric_object);
}

bool LudumLevel::OnPlayerTileCollision(double delta_time, class death::Player * player, chaos::ParticleDefault::Particle * player_particle, death::TiledMap::TileParticle * particle)
{

	return true; // continue with other
}
