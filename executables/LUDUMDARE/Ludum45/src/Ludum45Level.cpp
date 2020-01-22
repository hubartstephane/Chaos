#include "Ludum45Level.h"
#include "Ludum45LevelInstance.h"
#include "Ludum45Game.h"
#include "Ludum45Player.h"
#include "Ludum45GameInstance.h"

#include <chaos/GLMTools.h>
#include <chaos/ParticleDefault.h>
#include <chaos/GeometryFramework.h>
#include <chaos/ParticleSpawner.h>

#include <death/TiledMapLevel.h>

#if 1


void f()
{
    chaos::ParticleSpawner spawner = chaos::ParticleSpawner(nullptr);

    spawner.SpawnParticles(5, true, [](chaos::ParticleAccessor<chaos::ParticleDefault::Particle> accessor)
    {
        for (chaos::ParticleDefault::Particle& p : accessor)
            p.color = glm::vec4(1.0f, 0.0f, 0.0f, 1.0f);
    });
}
#endif



// =============================================================
// LayerParticleSpawnerBase
// =============================================================


// SHULUDUM : study ParticleLayerPopulator & LayerParticleSpawnerBase
//   => maybe some code to make in common except that 
//      then popular spawn only ONE ALLOC


class LayerParticleSpawnerBase // shuludum : to include natively inside death & chaos
{

public:

	bool Initialize(death::TiledMap::LayerInstance * layer_instance, char const * layer_name, size_t count);

public:


	chaos::TiledMap::GeometricObjectSurface const * surface = nullptr;

	death::TiledMap::LayerInstance * layer_instance = nullptr;

	chaos::BitmapAtlas::TextureArrayAtlas const * atlas = nullptr;

	chaos::BitmapAtlas::FolderInfo const * bitmap_set = nullptr;

	chaos::ParticleAllocationBase * allocation = nullptr;
};

bool LayerParticleSpawnerBase::Initialize(death::TiledMap::LayerInstance * in_layer_instance, char const * layer_name, size_t count)
{
	// get the final layer (by default, the same than the one in argument)
	if (in_layer_instance != nullptr && layer_name != nullptr)
		in_layer_instance = in_layer_instance->GetTiledLevelInstance()->FindLayerInstance(layer_name);
	layer_instance = in_layer_instance;
	if (layer_instance == nullptr)
		return false;
	// search the atlas
	atlas = layer_instance->GetGame()->GetTextureAtlas();
	if (atlas == nullptr)
		return false;
	// search the bitmap set
	bitmap_set = atlas->GetFolderInfo("sprites");
	if (bitmap_set == nullptr)
		return false;
	// create the allocation
	allocation = layer_instance->SpawnParticles(count);
	if (allocation == nullptr)
		return false;


	return true;
}


// -------------------------------------------------------------------



template<typename PARTICLE_TYPE>
class LayerParticleSpawner : public LayerParticleSpawnerBase
{

public:

	/** constructor */
	using LayerParticleSpawnerBase::LayerParticleSpawnerBase;

	/** getter for the accessor */
	chaos::ParticleAccessor<PARTICLE_TYPE> GetParticleAccessor()
	{
		if (allocation != nullptr)
			return allocation->GetParticleAccessor<PARTICLE_TYPE>();
		return chaos::ParticleAccessor<PARTICLE_TYPE>();
	}
};

// -------------------------------------------------------------------
















// =============================================================
// BonusSpawnerTriggerObject implementation
// =============================================================


#define BONUS_NORMAL 1

bool BonusSpawnerTriggerObject::IsAdditionalParticlesCreationEnabled() const
{

#if BONUS_NORMAL
	return false; //false;
#else
	return true;
#endif
}

bool BonusSpawnerTriggerObject::Initialize()
{
	if (!death::TiledMap::TriggerObject::Initialize())
		return false;
	trigger_once = true;

	bonus_name = geometric_object->FindPropertyString("BONUS_TYPE", "");
	bonus_type = chaos::MakeStaticTagType(bonus_name.c_str());

	return true;
}


// -------------------------------------------------------------------



bool BonusSpawnerTriggerObject::OnCameraCollisionEvent(double delta_time, chaos::box2 const & camera_box, int event_type)
{
	if (event_type != TriggerObject::COLLISION_STARTED)
		return false;

#if BONUS_NORMAL

	LudumGame * ludum_game = auto_cast(GetLayerInstance()->GetGame());
	if (ludum_game != nullptr)
	{
		LudumPlayer * ludum_player = auto_cast(ludum_game->GetPlayer(0));
		if (ludum_player != nullptr)
			ludum_player->OnPlayerUpgrade(bonus_type);	
	}

#else





	// prepare the spawner
	LayerParticleSpawner<ParticleBonus> spawner;
	if (!spawner.Initialize(GetLayerInstance(), "Bonus", 1))
		return true;
	// cast in a surface
	chaos::TiledMap::GeometricObjectSurface const * surface = geometric_object->GetObjectSurface();
	if (surface == nullptr)
		return true;
	chaos::BitmapAtlas::BitmapInfo const * bitmap_info = spawner.bitmap_set->GetBitmapInfo(bonus_name.c_str());
	if (bitmap_info == nullptr)
		return true;

	// prepare the particles
	chaos::ParticleAccessor<ParticleBonus> particles = spawner.GetParticleAccessor();

	chaos::ParticleTexcoords texcoords = chaos::ParticleTools::GetParticleTexcoords(*bitmap_info);

	particles[0].bounding_box = surface->GetBoundingBox(false);
	particles[0].texcoords = texcoords;
	particles[0].color = glm::vec4(1.0f, 1.0f, 0.0f, 1.0f);
	particles[0].bonus_type = bonus_type; 


#endif

	return true;
}









// =============================================================
// EnemySpawnerTriggerObject implementation
// =============================================================


bool EnemySpawnerTriggerObject::IsAdditionalParticlesCreationEnabled() const
{


	return false;
}

bool EnemySpawnerTriggerObject::Initialize()
{
	if (!death::TiledMap::TriggerObject::Initialize())
		return false;
	trigger_once = true;

	enemy_type    = geometric_object->FindPropertyString("ENEMY_TYPE", "");
	enemy_pattern = geometric_object->FindPropertyString("ENEMY_PATTERN", "");


	// we possess the pattern
	pattern = new EnemyPattern;
	if (pattern != nullptr)
		pattern->Initialize(geometric_object.get());

	LudumGame * ludum_game = auto_cast(GetLayerInstance()->GetGame());
	if (ludum_game != nullptr)
		type = ludum_game->FindEnemyType(enemy_type.c_str());

	return true;
}


// -------------------------------------------------------------------



bool EnemySpawnerTriggerObject::OnCameraCollisionEvent(double delta_time, chaos::box2 const & camera_box, int event_type)
{
	if (event_type != TriggerObject::COLLISION_STARTED)
		return false;

	if (type == nullptr || pattern == nullptr)
		return true;

	size_t enemy_count = pattern->enemy_count;
	if (enemy_count == 0)
		return true;


  //  chaos::ParticleSpawner spawner2(GetLayerInstance()->GetParticleLayer(), "Enemies");

    //GetLayerInstance()->CreateParticleSpawner();


	// prepare the spawner
	LayerParticleSpawner<ParticleEnemy> spawner;
	if (!spawner.Initialize(GetLayerInstance(), "Enemies", enemy_count))
		return true;
	// cast in a surface
	chaos::TiledMap::GeometricObjectSurface const * surface = geometric_object->GetObjectSurface();
	if (surface == nullptr)
		return true;
	chaos::BitmapAtlas::BitmapInfo const * bitmap_info = spawner.bitmap_set->GetBitmapInfo(type->bitmap_name.c_str());
	if (bitmap_info == nullptr)
		return true;
#if 0
	// initialize the allocation
	chaos::ParticleAllocation<ParticleEnemyTrait> * typed_allocation = auto_cast(spawner.allocation);
	if (typed_allocation != nullptr)
	{
		typed_allocation = nullptr;
	
	
	
	}
#endif


	chaos::box2 player_box = GetLayerInstance()->GetGame()->GetPlayer(0)->GetPlayerBox();



	// generate






	// prepare the particles
	chaos::ParticleAccessor<ParticleEnemy> particles = spawner.GetParticleAccessor();
	for (size_t i = 0 ; i < particles.GetCount() ; ++i)
	{
		chaos::ParticleTexcoords texcoords = chaos::ParticleTools::GetParticleTexcoords(*bitmap_info);

		particles[i].bounding_box.half_size = 0.5f * glm::vec2(bitmap_info->width, bitmap_info->height) * type->scale;



		particles[i].texcoords = texcoords;
		particles[i].color = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);	
	
		particles[i].enemy_damage = type->enemy_damage;
		particles[i].enemy_life = type->enemy_life;

		particles[i].enemy_index = (int)i;	
		particles[i].enemy_particle_count = (int)particles.GetCount();
		particles[i].pattern = pattern;
		particles[i].spawner_surface = surface->GetBoundingBox(true);
		particles[i].time = 0.0f;
		pattern->UpdateParticle(0.0f, &particles[i], player_box, camera_box);	
	}

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


chaos::ParticleLayerBase * LudumLevel::DoCreateParticleLayer(death::TiledMap::LayerInstance * layer_instance)
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

	bool is_fire = (chaos::StringTools::Stricmp(layer_name, "PlayerFire") == 0) || (chaos::StringTools::Stricmp(layer_name, "EnemyFire") == 0);
	if (is_fire)
	{
		ParticleFireTrait::LayerTrait fire_trait;
		fire_trait.game = ludum_game;
		return new chaos::ParticleLayer<ParticleFireTrait>(fire_trait);
	}

	bool is_explosion = (chaos::StringTools::Stricmp(layer_name, "Explosions") == 0);
	if (is_explosion)
	{
		ParticleExplosionTrait::LayerTrait explosion_trait;
		explosion_trait.game = ludum_game;
		return new chaos::ParticleLayer<ParticleExplosionTrait>(explosion_trait);
	}


	bool is_bonus = (chaos::StringTools::Stricmp(layer_name, "bonus") == 0);
	if (is_bonus)
	{
		ParticleBonusTrait::LayerTrait bonus_trait;
		bonus_trait.game = ludum_game;
		return new chaos::ParticleLayer<ParticleBonusTrait>(bonus_trait);
	}

	bool is_enemies = (layer_name == "Enemies");
	if (is_enemies)
	{
		ParticleEnemyTrait::LayerTrait enemy_trait;
		enemy_trait.game = ludum_game;
		return new chaos::ParticleLayer<ParticleEnemyTrait>(enemy_trait);
	}















	return death::TiledMap::Level::DoCreateParticleLayer(layer_instance);
}


death::TiledMap::GeometricObject * LudumLevel::DoCreateGeometricObject(death::TiledMap::LayerInstance * in_layer_instance, chaos::TiledMap::GeometricObject * in_geometric_object)
{
	chaos::TiledMap::GeometricObjectSurface * surface_object = in_geometric_object->GetObjectSurface();

	if (surface_object != nullptr)
	{
		if (chaos::TiledMapTools::HasFlag(surface_object, nullptr, nullptr, "BONUS_SPAWNER")) // shuludum : it is better to rely on a type of an object that to rely on its name
			return new BonusSpawnerTriggerObject(in_layer_instance, surface_object);          //            here we test for the property 'BONUS_SPAWNER' . Maybe the cleaner. Beware HasFlag method does not recurse like FindPropery(...)

		if (chaos::TiledMapTools::HasFlag(surface_object, nullptr, nullptr, "ENEMY_SPAWNER"))
			return new EnemySpawnerTriggerObject(in_layer_instance, surface_object);





	}
	return death::TiledMap::Level::DoCreateGeometricObject(in_layer_instance, in_geometric_object);
}

bool LudumLevel::OnPlayerTileCollision(double delta_time, class death::Player * player, chaos::ParticleDefault::Particle * player_particle, death::TiledMap::TileParticle * particle)
{

	return true; // continue with other
}
