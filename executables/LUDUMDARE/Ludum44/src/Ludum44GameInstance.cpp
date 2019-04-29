#include "Ludum44GameInstance.h"
#include "Ludum44Game.h"
#include "Ludum44Level.h"
#include "Ludum44LevelInstance.h"
#include "Ludum44Player.h"

DEATH_GAMEFRAMEWORK_IMPLEMENT_GAMEINSTANCE(Ludum);

LudumGameInstance::LudumGameInstance(death::Game * in_game) : 
	death::GameInstance(in_game)
{
	
}

death::Player * LudumGameInstance::DoCreatePlayer()
{
	return new LudumPlayer(this);
}

bool LudumGameInstance::DoCheckGameOverCondition()
{
	LudumPlayer * ludum_player = GetLudumPlayer(0);
	if (ludum_player != nullptr)
	{
		if (ludum_player->GetCurrentLife() <= 0.0f) // no more energy => go to checkpoint
		{
			// remove one life
			ludum_player->SetLifeCount(-1, true);
			// game over mandatory
			if (ludum_player->GetLifeCount() <= 0) 
				return true;
			// try to go to checkpoint
			if (!IsCheckpointValid() || !RestartFromCheckpoint(ludum_player))
				return true; // cannot respawn !
		}
	}
	return false;
}

bool LudumGameInstance::OnRestartedFromCheckpoint(death::Player * player)
{
	LudumPlayer * ludum_player = dynamic_cast<LudumPlayer*>(player);
	if (ludum_player != nullptr)
	{
		ludum_player->current_life = ludum_player->current_max_life;
	}

	// enable all triggers
	death::TiledMap::LayerInstance * trigger_layer_instance = GetLudumLevelInstance()->FindLayerInstance("Zones");
	if (trigger_layer_instance != nullptr)
	{
		size_t count = trigger_layer_instance->GetTriggerSurfaceCount();
		for (size_t i = 0 ; i < count ; ++i)
		{
			death::TiledMap::TriggerSurfaceObject * trigger = trigger_layer_instance->GetTriggerSurface(i);
			if (trigger != nullptr)
				trigger->SetEnabled(true);		
		}	
	}
	// destroy all bullets
	death::TiledMap::LayerInstance * fire_layer_instance = GetLudumLevelInstance()->FindLayerInstance("fire");
	if (fire_layer_instance != nullptr)
	{
		chaos::ParticleLayerBase * particle_layer = fire_layer_instance->GetParticleLayer();
		if (particle_layer != nullptr)
			particle_layer->ClearAllAllocations();
	}
	// destroy all enemies
	death::TiledMap::LayerInstance * enemies_layer_instance = GetLudumLevelInstance()->FindLayerInstance("Enemies");
	if (enemies_layer_instance != nullptr)
	{
		chaos::ParticleLayerBase * particle_layer = enemies_layer_instance->GetParticleLayer();
		if (particle_layer != nullptr)
			particle_layer->ClearAllAllocations();
	}



	return death::GameInstance::OnRestartedFromCheckpoint(player);
}

bool LudumGameInstance::DoTick(double delta_time)
{
	death::GameInstance::DoTick(delta_time);


	return true;
}


void LudumGameInstance::OnLevelChanged(death::GameLevel * new_level, death::GameLevel * old_level, death::GameLevelInstance * new_level_instance)
{
	// super method
	death::GameInstance::OnLevelChanged(new_level, old_level, new_level_instance);

	current_power_up = nullptr;
	current_power_up_surface = nullptr;
}

void LudumGameInstance::OnPlayerEntered(death::Player * player)
{
	death::GameInstance::OnPlayerEntered(player);

	LudumGame * ludum_game = GetLudumGame();
	if (ludum_game == nullptr)
		return;

	LudumPlayer * ludum_player = dynamic_cast<LudumPlayer *>(player);
	if (ludum_player == nullptr)
		return;

	ludum_player->current_life = ludum_game->initial_player_life; 
	ludum_player->current_max_life = ludum_game->initial_player_life;

	ludum_player->current_speed_index = 0;
	ludum_player->current_damage_index = 0;
	ludum_player->current_charged_damage_index = 0;
	ludum_player->current_fire_rate_index = 0;

	// find bitmap set
	chaos::BitmapAtlas::FolderInfo const * bitmap_set = ludum_game->GetTextureAtlas()->GetFolderInfo("sprites");
	if (bitmap_set != nullptr)
	{
		chaos::BitmapAtlas::BitmapInfo const * fire_info = bitmap_set->GetBitmapInfo("fire");
		if (fire_info == nullptr)
			return;
		chaos::BitmapAtlas::BitmapInfo const * charged_fire_info = bitmap_set->GetBitmapInfo("charged_fire");
		if (charged_fire_info == nullptr)
			return;

		ludum_player->fire_bitmap_layout = *fire_info;
		ludum_player->charged_fire_bitmap_layout = *charged_fire_info;
	
	}
}

void LudumGameInstance::OnPowerUpZone(death::Player * player, bool enter, death::TiledMap::TriggerSurfaceObject * surface)
{
	LudumGame * ludum_game = GetLudumGame();
	if (ludum_game == nullptr)
		return;

	LudumPlayer * ludum_player = dynamic_cast<LudumPlayer*>(player);
	if (ludum_player == nullptr)
		return;

	int count = ludum_game->power_ups.size();
	if (count == 0)
		return;

	if (!enter)
	{
		current_power_up = nullptr;
		current_power_up_surface = nullptr;
	}
	else
	{		
		int value = rand() % count;
		for (int i = 0 ; i < count ; ++i)
		{
			int index = (i + value) % count;
			if (ludum_game->power_ups[index]->CanPowerUp(ludum_game, ludum_player))
			{
				current_power_up = ludum_game->power_ups[index].get();	
				current_power_up_surface = surface;
				return;
			}
		}
	}
}


ParticleExplosion * LudumGameInstance::FireExplosion(chaos::box2 const & ref_box)
{

	// search BitmapLayout for Enemy
	chaos::BitmapAtlas::FolderInfo const * bitmap_set = game->GetTextureAtlas()->GetFolderInfo("sprites");
	if (bitmap_set == nullptr)
		return nullptr;

	chaos::BitmapAtlas::BitmapInfo const * explosion_info = bitmap_set->GetBitmapInfo("explosion_4x4");
	if (explosion_info == nullptr)
		return nullptr;

	int count = explosion_info->GetAnimationImageCount();
	if (count == 0)
		return nullptr;

	if (explosion_info->animation_info == nullptr)
		return nullptr;


	death::TiledMap::LayerInstance * layer_instance = GetLudumLevelInstance()->FindLayerInstance("Explosions");
	if (layer_instance == nullptr)
		return nullptr;

	chaos::ParticleLayerBase * particle_layer = layer_instance->CreateParticleLayer();
	if (particle_layer == nullptr)
		return nullptr;

	chaos::ParticleAllocationBase * allocation = nullptr;
	if (particle_layer->GetAllocationCount() > 0)
		allocation = particle_layer->GetAllocation(0);
	else
	{
		allocation = particle_layer->SpawnParticles(0);
		if (allocation == nullptr)
			return nullptr;		
	}

	if (!allocation->AddParticles(1))
		return nullptr;

	chaos::ParticleAccessor<ParticleExplosion> particle = allocation->GetParticleAccessor<ParticleExplosion>();
	if (particle.GetCount() == 0)
		return nullptr;

	ParticleExplosion * p = &particle[particle.GetCount() - 1];

	p->bounding_box = ref_box; 
	p->bounding_box.half_size.x = p->bounding_box.half_size.y = 1.3f * chaos::MathTools::Maximum(ref_box.half_size.x, ref_box.half_size.y);

	p->color = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f); 
	p->explosion_info = explosion_info;

	return p;
}

ParticleFire * LudumGameInstance::FireProjectile(chaos::ParticleAllocationBase * allocation, chaos::box2 const & ref_box, chaos::BitmapAtlas::BitmapLayout const & layout, float ratio_to_box, int count, char const * sound_name, float delta_rotation, bool player_ownership, float velocity, float offset_rotation)
{
	if (count <= 0)
		return nullptr;
	if (allocation == nullptr)
		return nullptr;

	LudumGame * ludum_game = GetLudumGame();
	if (ludum_game == nullptr)
		return nullptr;

	if (!allocation->AddParticles(count))
		return nullptr;
	chaos::ParticleAccessor<ParticleFire> particles = allocation->GetParticleAccessor<ParticleFire>();
	if (particles.GetCount() == 0)
		return nullptr;

	ParticleFire * result = &particles[particles.GetCount() - count];

	// compute the bounding box for all particles
	chaos::box2 particle_box = ref_box;

	particle_box.half_size = ratio_to_box * ref_box.half_size;
	particle_box = chaos::AlterBoxToAspect(particle_box, chaos::MathTools::CastAndDiv<float>(layout.width, layout.height), true);	
	particle_box.position = ref_box.position;

	// compute texcoords for all particles
	chaos::ParticleTexcoords texcoords = chaos::ParticleTools::GetParticleTexcoords(layout, ludum_game->GetTextureAtlas()->GetAtlasDimension());


	float rotation = 0.0f - delta_rotation * (float)(count / 2);

	for (int i = 0 ; i < count ; ++i)
	{
		result[i].bounding_box = particle_box;	
		result[i].texcoords = texcoords;
		result[i].color = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);

		int scroll_direction = 1;
		float particle_orientation = rotation + offset_rotation;
		float particle_velocity_orientation = offset_rotation + rotation + (float)M_PI_2;

		glm::vec2 direction = glm::vec2(cosf(particle_velocity_orientation), sinf(particle_velocity_orientation));
		result[i].velocity  = velocity * direction;
		result[i].rotation  = particle_orientation;

		result[i].bounding_box.position += direction * ref_box.half_size[scroll_direction];

		result[i].player_ownership = player_ownership;

		rotation += delta_rotation;
	}

	if (sound_name != nullptr)	
		ludum_game->PlaySound(sound_name, false, false);

	return result;
}