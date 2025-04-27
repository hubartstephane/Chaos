#include "Ludum44PCH.h"
#include "Ludum44Level.h"
#include "Ludum44LevelInstance.h"
#include "Ludum44Game.h"
#include "Ludum44Player.h"
#include "Ludum44GameInstance.h"

// =============================================================
// LudumLevel implementation
// =============================================================

LudumLevel::LudumLevel()
{
	level_instance_class = LudumLevelInstance::GetStaticClass();
}

chaos::ParticleLayerBase * LudumLevel::DoCreateParticleLayer(chaos::TMLayerInstance * layer_instance)
{
	LudumGame * ludum_game = layer_instance->GetGame();

	std::string const & layer_name = layer_instance->GetTiledLayer()->name;

	bool is_player_and_camera = (chaos::StringTools::Stricmp(layer_name, "PlayerAndCamera") == 0);
	if (is_player_and_camera)
	{
		ParticlePlayerLayerTrait layer_trait;
		layer_trait.game = ludum_game;
		return new chaos::ParticleLayer<ParticlePlayerLayerTrait>(layer_trait);
	}

	bool is_fire = (chaos::StringTools::Stricmp(layer_name, "fire") == 0);
	if (is_fire)
	{
		ParticleFireLayerTrait fire_trait;
		fire_trait.game = ludum_game;
		return new chaos::ParticleLayer<ParticleFireLayerTrait>(fire_trait);
	}

	bool is_explosion = (chaos::StringTools::Stricmp(layer_name, "Explosions") == 0);
	if (is_explosion)
	{
		ParticleExplosionLayerTrait explosion_trait;
		explosion_trait.game = ludum_game;
		return new chaos::ParticleLayer<ParticleExplosionLayerTrait>(explosion_trait);
	}

	bool is_zone = (chaos::StringTools::Stricmp(layer_name, "Zones") == 0);
	if (is_zone)
	{
		return new chaos::ParticleLayer<PowerUpZoneParticleLayerTrait>();
	}

	bool is_enemies = (chaos::StringTools::Stricmp(layer_name, "Enemies") == 0);
	if (is_enemies)
	{
		ParticleEnemyLayerTrait enemy_trait;
		enemy_trait.game = ludum_game;
		return new chaos::ParticleLayer<ParticleEnemyLayerTrait>(enemy_trait);
	}

	return chaos::TMLevel::DoCreateParticleLayer(layer_instance);
}

chaos::TMObjectFactory LudumLevel::DoGetObjectFactory(chaos::TMLayerInstance * in_layer_instance, chaos::TiledMap::TypedObject const * in_typed_object)
{
	if (in_typed_object->IsObjectOfType("PowerUp"))
		return CHAOS_TM_MAKE_OBJECT_FACTORY(return new PowerUpTrigger();); // XXX : the power up, is the only object that has IsParticleCreationEnabled() => true
	if (in_typed_object->IsObjectOfType("SpeedUp"))
		return CHAOS_TM_MAKE_OBJECT_FACTORY(return new SpeedUpTrigger(););
	if (in_typed_object->IsObjectOfType("Spawner"))
		return CHAOS_TM_MAKE_OBJECT_FACTORY(return new SpawnerTrigger(););

	return chaos::TMLevel::DoGetObjectFactory(in_layer_instance, in_typed_object);
}





















// =============================================================
// PowerUpTrigger implementation
// =============================================================

bool PowerUpTrigger::Initialize(chaos::TMLayerInstance * in_layer_instance, chaos::TiledMap::GeometricObject const * in_geometric_object, chaos::TMObjectReferenceSolver& in_reference_solver)
{
	if (!chaos::TMTrigger::Initialize(in_layer_instance, in_geometric_object, in_reference_solver))
		return false;
	decrease_power = in_geometric_object->GetPropertyValueBool("DECREASE_POWER_UP", false);
	return true;
}

bool PowerUpTrigger::OnCollisionEvent(float delta_time, chaos::Object * object, chaos::CollisionType event_type)
{
	chaos::Player* player = auto_cast(object);
	if (player == nullptr)
		return false;

	LudumGameInstance * ludum_game_instance = player->GetGameInstance();
	if (ludum_game_instance == nullptr)
		return false;

	if (event_type == chaos::CollisionType::STARTED)
		ludum_game_instance->OnPowerUpZone(player, true, this);
	else if (event_type == chaos::CollisionType::AGAIN && reset_trigger)
		ludum_game_instance->OnPowerUpZone(player, true, this);
	else if (event_type == chaos::CollisionType::FINISHED)
		ludum_game_instance->OnPowerUpZone(player, false, this);
	else
		return false;

	reset_trigger = false;

	return true; // collisions handled successfully
}


// =============================================================
// SpeedUpTrigger implementation
// =============================================================

bool SpeedUpTrigger::Initialize(chaos::TMLayerInstance* in_layer_instance, chaos::TiledMap::GeometricObject const * in_geometric_object, chaos::TMObjectReferenceSolver& in_reference_solver)
{
	if (!chaos::TMTrigger::Initialize(in_layer_instance, in_geometric_object, in_reference_solver))
		return false;
	scroll_speed = in_geometric_object->GetPropertyValueFloat("SCROLL_SPEED", 1.0f);
	return true;
}

bool SpeedUpTrigger::OnCollisionEvent(float delta_time, chaos::Object * object, chaos::CollisionType event_type)
{
	chaos::Player* player = auto_cast(object);
	if (player == nullptr)
		return false;

	if (event_type != chaos::CollisionType::STARTED) // already handled
		return false;

	LudumLevelInstance * ludum_level_instance = GetLayerInstance()->GetLevelInstance();
	if (ludum_level_instance == nullptr)
		return true;
	ludum_level_instance->SetScrollFactor(scroll_speed);

	return true; // collisions handled successfully
}


// =============================================================
// SpawnerTrigger implementation
// =============================================================


bool SpawnerTrigger::Initialize(chaos::TMLayerInstance* in_layer_instance, chaos::TiledMap::GeometricObject const * in_geometric_object, chaos::TMObjectReferenceSolver& in_reference_solver)
{
	if (!chaos::TMTrigger::Initialize(in_layer_instance, in_geometric_object, in_reference_solver))
		return false;

	scale_factor = in_geometric_object->GetPropertyValueFloat("ENEMY_SCALE_FACTOR", 1.0f);
	life_factor = in_geometric_object->GetPropertyValueFloat("ENEMY_HEALTH_FACTOR", 1.0f);
	count = in_geometric_object->GetPropertyValueInt("ENEMY_COUNT", 10);
	spawn_curve_type = in_geometric_object->GetPropertyValueInt("SPAWN_CURVE_TYPE", 0);
	spawn_enemy_type = in_geometric_object->GetPropertyValueInt("SPAWN_ENEMY_TYPE", 0);
	spawn_move_type = in_geometric_object->GetPropertyValueInt("SPAWN_MOVE_TYPE", 0);

	trigger_once = true;
	return true;
}

bool SpawnerTrigger::OnCollisionEvent(float delta_time, chaos::Object * object, chaos::CollisionType event_type)
{
	chaos::Camera* camera = auto_cast(object);
	if (camera == nullptr)
		return false;

	// only the first time collision is detected
	if (event_type != chaos::CollisionType::STARTED)
		return false;

    // shuludum

#if 0
    chaos::ParticleSpawner spawner = GetLayerInstance()->GetTiledLevelInstance()->GetParticleSpawner("Enemies");
    if (!spawner.IsValid())
        return true;
#endif


	// shuludm : see if this can be refactored

	// search the layer for enemies
	LudumLevelInstance * ludum_level_instance = GetLayerInstance()->GetLevelInstance();

	chaos::TMLayerInstance * enemy_layer_instance = ludum_level_instance->FindLayerInstance("Enemies", true);
	if (enemy_layer_instance == nullptr)
		return true;

	// search the atlas
	chaos::WindowApplication* window_application = chaos::Application::GetInstance();
	if (window_application == nullptr)
		return false;
	chaos::GPUAtlas const * atlas = window_application->GetTextureAtlas();
	if (atlas == nullptr)
		return true;

	// search BitmapLayout for Enemy
	chaos::BitmapAtlas::FolderInfo const * folder_info = atlas->GetFolderInfo("sprites");
	if (folder_info == nullptr)
		return true;

	// create an allocation for all enemies we are about to create
	chaos::ParticleAllocationBase * allocation = enemy_layer_instance->SpawnParticles(0);
	if (allocation == nullptr)
		return true;

	// get the frequencies
	float fire_frequency = 1.0f;
	LudumGame * ludum_game = enemy_layer_instance->GetGame();
	if (ludum_game != nullptr)
		fire_frequency = ludum_game->enemy_fire_rate;

	// extract zone parametes

	chaos::box2 surface_box = GetBoundingBox(true);

	static constexpr int SPAWN_ENEMY_METEORS          = 0;
	static constexpr int SPAWN_ENEMY_ALIEN            = 1;
	static constexpr int SPAWN_ENEMY_FOUR_TURRETS     = 2;
	static constexpr int SPAWN_ENEMY_FOLLOWING_TURRET = 3;
	static constexpr int SPAWN_ENEMY_LAST = 4;
	spawn_enemy_type = (spawn_enemy_type % SPAWN_ENEMY_LAST);

	static constexpr int SPAWN_MOVE_STATIC  = 0;
	static constexpr int SPAWN_MOVE_FORWARD = 1;
	static constexpr int SPAWN_MOVE_RANDOM  = 2;
	static constexpr int SPAWN_MOVE_LAST    = 3;
	spawn_move_type = (spawn_move_type % SPAWN_MOVE_LAST);

	static constexpr int SPAWN_CURVE_RANDOM        = 0;
	static constexpr int SPAWN_CURVE_V             = 1;
	static constexpr int SPAWN_CURVE_INVERTED_V    = 2;
	static constexpr int SPAWN_CURVE_ALIGNED       = 3;
	static constexpr int SPAWN_CURVE_LAST          = 4;
	spawn_curve_type = (spawn_curve_type % SPAWN_CURVE_LAST);

	chaos::BitmapAtlas::BitmapInfo const * enemy_info = nullptr; // so we can reuse among the loop existing random choice

    // shuludum

    chaos::ParticleAccessor<ParticleEnemy> accessor = allocation->AddParticles(count);
    for (ParticleEnemy& particle : accessor)
    {
        size_t i = accessor.GetDataIndex(&particle);

        // fill position & velocity
        if (spawn_curve_type == SPAWN_CURVE_RANDOM)
        {
            particle.bounding_box.position = surface_box.position + (2.0f * chaos::GLMTools::RandVec2() - glm::vec2(1.0f, 1.0f)) * surface_box.half_size;
        }
        else if (spawn_curve_type == SPAWN_CURVE_V || spawn_curve_type == SPAWN_CURVE_INVERTED_V || spawn_curve_type == SPAWN_CURVE_ALIGNED)
        {
            float x = 0.5f; // by default center of the surface
            if (count > 1)
                x = ((float)i) / (float)(count - 1);

            float sx = surface_box.half_size.x;
            float sy = surface_box.half_size.y;

            particle.bounding_box.position.x = surface_box.position.x - sx + 2.0f * x * sx;

            if (spawn_curve_type == SPAWN_CURVE_ALIGNED)
                particle.bounding_box.position.y = surface_box.position.y;
            else
            {
                float renormalized_x = (x * 2.0f) - 1.0f;

                float y = (spawn_curve_type == SPAWN_CURVE_V) ?
                    (renormalized_x * renormalized_x) :
                    (1.0f - renormalized_x * renormalized_x);

                particle.bounding_box.position.y = surface_box.position.y + sy * y;
            }
        }

        // move type
        if (spawn_move_type == SPAWN_MOVE_STATIC)
        {
            particle.velocity = glm::vec2(0.0f, 0.0f);
        }
        else if (spawn_move_type == SPAWN_MOVE_FORWARD)
        {
            particle.velocity = glm::vec2(0.0f, -1000.0f);
        }
        else if (spawn_move_type == SPAWN_MOVE_RANDOM)
        {
            particle.velocity = glm::vec2(chaos::MathTools::RandFloat(-200.0f, +200.0f), chaos::MathTools::RandFloat(0.0f, -1000.0f));
        }
        // fill enemy behavior
        if (spawn_enemy_type == SPAWN_ENEMY_METEORS)
        {
            char const* enemy_names[] = { "meteor1", "meteor2", "meteor3", "meteor4" }; // one RANDOM for each enemy
            int name_count = sizeof(enemy_names) / sizeof(enemy_names[0]);
            int rand_name = rand() % name_count;
            enemy_info = folder_info->GetBitmapInfo(enemy_names[rand_name]);

            particle.rotation_speed = chaos::MathTools::RandFloat(0.0f, -1.0f);


            particle.life = ludum_game->meteor_health;
            particle.damage_for_player = ludum_game->meteor_damage_for_player;
        }
        else if (spawn_enemy_type == SPAWN_ENEMY_ALIEN)
        {
            char const* enemy_names[] = { "Enemy2" }; // , "Enemy3", "Enemy4", "Enemy5"
          // an single RANDOM for the whole set of enemies
            if (enemy_info == nullptr)
            {
                int name_count = sizeof(enemy_names) / sizeof(enemy_names[0]);
                int rand_name = rand() % name_count;

                enemy_info = folder_info->GetBitmapInfo(enemy_names[rand_name]);
            }

            particle.life = ludum_game->alien_health;
            particle.damage_for_player = ludum_game->alien_damage_for_player;
        }
        else if (spawn_enemy_type == SPAWN_ENEMY_FOUR_TURRETS)
        {
            enemy_info = folder_info->GetBitmapInfo("Enemy1");
            particle.fire_frequency = fire_frequency * 2.0f;         // x2 because 4 bullets in a row !!!
            particle.rotation_following_player = false;
            particle.rotation_speed = 1.0f;

            particle.life = ludum_game->turret_health;
            particle.damage_for_player = ludum_game->turret_damage_for_player;

        }
        else if (spawn_enemy_type == SPAWN_ENEMY_FOLLOWING_TURRET)
        {
            enemy_info = folder_info->GetBitmapInfo("Enemy1");
            particle.fire_frequency = fire_frequency;
            particle.rotation_following_player = true;

            particle.life = ludum_game->turret_health;
            particle.damage_for_player = ludum_game->turret_damage_for_player;
        }

        // not found
        if (enemy_info == nullptr)
        {
            particle.life = 0.0f; // destroy it
            continue;
        }
        // common filling
		chaos::ParticleTexcoords texcoords = enemy_info->GetTexcoords();

        particle.bounding_box.half_size = 0.5f * scale_factor * glm::vec2((float)enemy_info->width, (float)enemy_info->height);
        particle.texcoords = texcoords;

        particle.life *= life_factor;
    }

	// auto delete allocation
	allocation->SetDestroyWhenEmpty(true);

	return true; // collisions handled successfully
}
