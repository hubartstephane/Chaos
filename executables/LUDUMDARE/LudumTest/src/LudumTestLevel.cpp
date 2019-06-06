#include "LudumTestLevel.h"
#include "LudumTestLevelInstance.h"
#include "LudumTestGame.h"
#include "LudumTestPlayer.h"
#include "LudumTestGameInstance.h"

#include <chaos/GLMTools.h>
#include <chaos/ParticleDefault.h>
#include <chaos/GeometryFramework.h>

// =============================================================
// IsTileCreationEnabled
// =============================================================

static bool IsDefaultTileCreationEnabled()
{

	return false;

#if _DEBUG
	return true;
#else 
	return false;
#endif	
}

bool FinishingTriggerSurfaceObject::IsTileCreationEnabled() const
{
	return IsDefaultTileCreationEnabled();
}

bool CheckpointTriggerSurfaceObject::IsTileCreationEnabled() const
{
	return IsDefaultTileCreationEnabled();
}

bool SpeedUpTriggerSurfaceObject::IsTileCreationEnabled() const
{
	return IsDefaultTileCreationEnabled();
}

bool SpawnerTriggerSurfaceObject::IsTileCreationEnabled() const
{
	return IsDefaultTileCreationEnabled();
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

	bool is_player_and_camera = (layer_name == "PlayerAndCamera");
	if (is_player_and_camera)
	{
		ParticlePlayerTrait::LayerTrait layer_trait;
		layer_trait.game = ludum_game;
		return new chaos::ParticleLayer<ParticlePlayerTrait>(layer_trait);
	}

	bool is_fire = (layer_name == "fire");
	if (is_fire)
	{
		ParticleFireTrait::LayerTrait fire_trait;
		fire_trait.game = ludum_game;
		return new chaos::ParticleLayer<ParticleFireTrait>(fire_trait);
	}

	bool is_explosion = (layer_name == "Explosions");
	if (is_explosion)
	{
		ParticleExplosionTrait::LayerTrait explosion_trait;
		explosion_trait.game = ludum_game;
		return new chaos::ParticleLayer<ParticleExplosionTrait>(explosion_trait);
	}

	bool is_zone = (layer_name == "Zones");
	if (is_zone)
	{
		return new chaos::ParticleLayer<TileParticleTraitExt>();
	}

	bool is_enemies = (layer_name == "Enemies");
	if (is_enemies)
	{
		ParticleEnemyTrait::LayerTrait enemy_trait;
		enemy_trait.game = ludum_game;
		return new chaos::ParticleLayer<ParticleEnemyTrait>(enemy_trait);
	}

	return death::TiledMap::Level::CreateParticleLayer(layer_instance);
}


death::TiledMap::TriggerSurfaceObject * LudumLevel::DoCreateTriggerSurface(death::TiledMap::LayerInstance * in_layer_instance, chaos::TiledMap::GeometricObject * in_geometric_object)
{
	if (in_geometric_object->name == "Finish")
		return new FinishingTriggerSurfaceObject(in_layer_instance, in_geometric_object);
	if (in_geometric_object->name == "PowerUp")
		return new PowerUpTriggerSurfaceObject(in_layer_instance, in_geometric_object);
	if (in_geometric_object->name == "Checkpoint")
		return new CheckpointTriggerSurfaceObject(in_layer_instance, in_geometric_object);
	if (in_geometric_object->name == "SpeedUp")
		return new SpeedUpTriggerSurfaceObject(in_layer_instance, in_geometric_object);
	if (in_geometric_object->name == "Spawner")
		return new SpawnerTriggerSurfaceObject(in_layer_instance, in_geometric_object);

	return death::TiledMap::Level::DoCreateTriggerSurface(in_layer_instance, in_geometric_object);
}


bool LudumLevel::OnPlayerTileCollision(double delta_time, class death::Player * player, chaos::ParticleDefault::Particle * player_particle, death::TiledMap::TileParticle * particle)
{

	return true; // continue with other
}

// =============================================================
// FinishingTriggerSurfaceObject implementation
// =============================================================

bool FinishingTriggerSurfaceObject::OnPlayerCollisionEvent(double delta_time, death::Player * player, chaos::ParticleDefault::Particle * player_particle, int event_type)
{
	LudumLevelInstance * ludum_level_instance = auto_cast(GetLayerInstance()->GetTiledLevelInstance());
	if (ludum_level_instance == nullptr)
		return true;
	ludum_level_instance->SetLevelCompleted(true);

	return true; // continue other collisions
}


// =============================================================
// PowerUpTriggerSurfaceObject implementation
// =============================================================

bool PowerUpTriggerSurfaceObject::OnPlayerCollisionEvent(double delta_time, death::Player * player, chaos::ParticleDefault::Particle * player_particle, int event_type)
{
	LudumGameInstance * ludum_game_instance = auto_cast(player->GetGameInstance());
	if (ludum_game_instance != nullptr)
	{
		bool decrease_power = geometric_object->FindPropertyBool("DECREASE_POWER_UP", false);

		if (event_type == TriggerSurfaceObject::COLLISION_STARTED)
			ludum_game_instance->OnPowerUpZone(player, true, this, decrease_power);
		else if (event_type == TriggerSurfaceObject::COLLISION_AGAIN && reset_trigger)
			ludum_game_instance->OnPowerUpZone(player, true, this, decrease_power);
		else if (event_type == TriggerSurfaceObject::COLLISION_FINISHED)
			ludum_game_instance->OnPowerUpZone(player, false, this, decrease_power);

		reset_trigger = false;
	}

	return true; // continue other collisions
}

// =============================================================
// CheckPointTriggerSurfaceObject implementation
// =============================================================

bool CheckpointTriggerSurfaceObject::OnCameraCollisionEvent(double delta_time, chaos::box2 const & camera_box, int event_type)
{
	if (event_type != TriggerSurfaceObject::COLLISION_STARTED)
		return true;

	chaos::TiledMap::GeometricObjectSurface * surface = geometric_object->GetObjectSurface();
	if (surface == nullptr)
		return true;

	SetEnabled(false);

	death::GameInstance * game_instance = GetLayerInstance()->GetGame()->GetGameInstance();
	if (game_instance != nullptr)
		game_instance->SetCheckpointPosition(surface->GetBoundingBox(true).position, GetLayerInstance()->GetTiledLevelInstance());

	return true;
}

// =============================================================
// SpeedUpTriggerSurfaceObject implementation
// =============================================================

bool SpeedUpTriggerSurfaceObject::OnPlayerCollisionEvent(double delta_time, death::Player * player, chaos::ParticleDefault::Particle * player_particle, int event_type)
{
	if (event_type != TriggerSurfaceObject::COLLISION_STARTED) // already handled
		return true;

	LudumLevelInstance * ludum_level_instance = auto_cast(GetLayerInstance()->GetTiledLevelInstance());
	if (ludum_level_instance == nullptr)
		return true;

	float scroll_speed = geometric_object->FindPropertyFloat("SCROLL_SPEED", 1.0f);
	ludum_level_instance->SetScrollFactor(scroll_speed);

	return true; // continue other collisions
}


// =============================================================
// SpawnerTriggerSurfaceObject implementation
// =============================================================


bool SpawnerTriggerSurfaceObject::OnCameraCollisionEvent(double delta_time, chaos::box2 const & camera_box, int event_type)
{
	// only the first time collision is detected
	if (event_type != death::TiledMap::TriggerSurfaceObject::COLLISION_STARTED)
		return true;

	// search the layer for enemies
	death::TiledMap::LayerInstance * enemy_layer_instance = GetLayerInstance()->GetTiledLevelInstance()->FindLayerInstance("Enemies");
	if (enemy_layer_instance == nullptr)
		return true;

	// search the atlas
	chaos::BitmapAtlas::TextureArrayAtlas const * atlas = enemy_layer_instance->GetGame()->GetTextureAtlas();
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
	
	// create the particle layer if necessary
	if (enemy_layer_instance->CreateParticleLayer() == nullptr)
		return true;

	// create an allocation for all enemies we are about to create
	chaos::ParticleAllocationBase * allocation = enemy_layer_instance->CreateParticleAllocation();
	if (allocation == nullptr)
		return true;

	// get the frequencies

	float fire_frequency = 1.0f;
	LudumGame * ludum_game = auto_cast(enemy_layer_instance->GetGame());
	if (ludum_game != nullptr)
		fire_frequency = ludum_game->enemy_fire_rate;









	// extract zone parametes
	
	chaos::box2 surface_box = surface->GetBoundingBox(true);
	float scale_factor     = surface->FindPropertyFloat("ENEMY_SCALE_FACTOR", 1.0f);
	float life_factor     = surface->FindPropertyFloat("ENEMY_LIFE_FACTOR", 1.0f);
	int   count            = surface->FindPropertyInt("ENEMY_COUNT", 10);
	int   spawn_curve_type = surface->FindPropertyInt("SPAWN_CURVE_TYPE", 0);
	int   spawn_enemy_type = surface->FindPropertyInt("SPAWN_ENEMY_TYPE", 0);
	int   spawn_move_type  = surface->FindPropertyInt("SPAWN_MOVE_TYPE", 0);

	static int const SPAWN_ENEMY_METEORS          = 0;
	static int const SPAWN_ENEMY_ALIEN            = 1;
	static int const SPAWN_ENEMY_FOUR_TURRETS     = 2;
	static int const SPAWN_ENEMY_FOLLOWING_TURRET = 3;
	static int const SPAWN_ENEMY_LAST = 4;
	spawn_enemy_type = (spawn_enemy_type % SPAWN_ENEMY_LAST);

	static int const SPAWN_MOVE_STATIC  = 0;
	static int const SPAWN_MOVE_FORWARD = 1;
	static int const SPAWN_MOVE_RANDOM  = 2;	
	static int const SPAWN_MOVE_LAST    = 3;
	spawn_move_type = (spawn_move_type % SPAWN_MOVE_LAST);

	static int const SPAWN_CURVE_RANDOM        = 0;
	static int const SPAWN_CURVE_V             = 1;
	static int const SPAWN_CURVE_INVERTED_V    = 2;
	static int const SPAWN_CURVE_ALIGNED       = 3;
	static int const SPAWN_CURVE_LAST          = 4;
	spawn_curve_type = (spawn_curve_type % SPAWN_CURVE_LAST);


	int index_offset = (int)allocation->GetParticleCount();

	chaos::BitmapAtlas::BitmapInfo const * enemy_info = nullptr; // so we can reuse among the loop existing random choice
	if (allocation->AddParticles(count))
	{
		chaos::ParticleAccessor<ParticleEnemy> particles = allocation->GetParticleAccessor<ParticleEnemy>();
		for (int i = 0 ; i < count ; ++i)
		{
			ParticleEnemy & p = particles[index_offset + i];
		
			// fill position & velocity			
			if (spawn_curve_type == SPAWN_CURVE_RANDOM)
			{
				p.bounding_box.position = surface_box.position + (2.0f * chaos::GLMTools::RandVec2() - glm::vec2(1.0f, 1.0f)) * surface_box.half_size;			
			}
			else if (spawn_curve_type == SPAWN_CURVE_V || spawn_curve_type == SPAWN_CURVE_INVERTED_V || spawn_curve_type == SPAWN_CURVE_ALIGNED)
			{
				float x = 0.5f; // by default center of the surface
				if (count > 1)
					x = ((float)i) / (float)(count - 1);

				float sx = surface_box.half_size.x;
				float sy = surface_box.half_size.y;

				p.bounding_box.position.x = surface_box.position.x - sx + 2.0f * x * sx;				

				if (spawn_curve_type == SPAWN_CURVE_ALIGNED)
					p.bounding_box.position.y = surface_box.position.y;
				else
				{
					float renormalized_x = (x * 2.0f) - 1.0f;

					float y = (spawn_curve_type == SPAWN_CURVE_V)? 
						(renormalized_x * renormalized_x) : 
						(1.0f - renormalized_x * renormalized_x);

					p.bounding_box.position.y = surface_box.position.y + sy * y;
				}
			}

			// move type
			if (spawn_move_type == SPAWN_MOVE_STATIC)
			{
				p.velocity = glm::vec2(0.0f, 0.0f);
			}
			else if (spawn_move_type == SPAWN_MOVE_FORWARD)
			{
				p.velocity = glm::vec2(0.0f, -1000.0f);
			}
			else if (spawn_move_type == SPAWN_MOVE_RANDOM)
			{
				p.velocity = glm::vec2(chaos::MathTools::RandFloat(-200.0f, +200.0f), chaos::MathTools::RandFloat(0.0f, -1000.0f));
			}


			// fill enemy behavior
			if (spawn_enemy_type == SPAWN_ENEMY_METEORS)
			{
				char const * enemy_names[] = {"meteor1", "meteor2", "meteor3", "meteor4"}; // one RANDOM for each enemy
				int name_count = sizeof(enemy_names) / sizeof(enemy_names[0]);				
				int rand_name = rand() % name_count;
				enemy_info = bitmap_set->GetBitmapInfo(enemy_names[rand_name]);

				p.rotation_speed = chaos::MathTools::RandFloat(0.0f, -1.0f);


				p.life = ludum_game->meteor_life;
				p.damage_for_player = ludum_game->meteor_damage_for_player;
			}
			else if (spawn_enemy_type == SPAWN_ENEMY_ALIEN)
			{
				char const * enemy_names[] = { "Enemy2" }; // , "Enemy3", "Enemy4", "Enemy5"
			  // an single RANDOM for the whole set of enemies
				if (enemy_info == nullptr)
				{
					int name_count = sizeof(enemy_names) / sizeof(enemy_names[0]);				
					int rand_name = rand() % name_count;

					enemy_info = bitmap_set->GetBitmapInfo(enemy_names[rand_name]);
				}			

				p.life = ludum_game->alien_life;
				p.damage_for_player = ludum_game->alien_damage_for_player;
			}
			else if (spawn_enemy_type == SPAWN_ENEMY_FOUR_TURRETS)
			{
				enemy_info = bitmap_set->GetBitmapInfo("Enemy1");
				p.fire_frequency = fire_frequency * 2.0f;         // x2 because 4 bullets in a row !!!
				p.rotation_following_player = false;
				p.rotation_speed = 1.0f;

				p.life = ludum_game->turret_life;
				p.damage_for_player = ludum_game->turret_damage_for_player;

			}
			else if (spawn_enemy_type == SPAWN_ENEMY_FOLLOWING_TURRET)
			{
				enemy_info = bitmap_set->GetBitmapInfo("Enemy1");
				p.fire_frequency = fire_frequency;
				p.rotation_following_player = true;

				p.life = ludum_game->turret_life;
				p.damage_for_player = ludum_game->turret_damage_for_player;
			}

			// not found
			if (enemy_info == nullptr)
			{
				p.life = 0.0f; // destroy it
				continue;
			}


			// common filling
			chaos::ParticleTexcoords texcoords = chaos::ParticleTools::GetParticleTexcoords(*enemy_info, atlas->GetAtlasDimension());

			p.bounding_box.half_size = 0.5f * scale_factor * glm::vec2((float)enemy_info->width, (float)enemy_info->height);	
			p.texcoords = texcoords;

			p.life *= life_factor;
		}
	}

	// auto delete allocation
	allocation->SetDestroyWhenEmpty(true);

	// disable the surface
	SetEnabled(false);

	return true; // continue other collisions
}