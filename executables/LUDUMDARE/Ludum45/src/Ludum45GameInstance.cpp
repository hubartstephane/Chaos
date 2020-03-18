#include "Ludum45GameInstance.h"
#include "Ludum45Game.h"
#include "Ludum45Level.h"
#include "Ludum45LevelInstance.h"
#include "Ludum45Player.h"

#include <death/SoundContext.h>













LudumGameInstance::LudumGameInstance(death::Game * in_game) : 
	death::GameInstance(in_game)
{
	
}

death::Player * LudumGameInstance::DoCreatePlayer()
{
	return new LudumPlayer(this);
}


#if 0

class GameRule
{

};

bool Player::IsDead()
{
	if (health <= 0.0f)
		return true;


}

class SinglePlayerGameRule : public GameRule
{
public:

	bool Process()
	{

		death::Player* player = game_instance->GetPlayer(0);
		if (player == nullptr)
			return false;

		if 

	}

	death::GameInstance* game_instance = nullptr;

};

class PVPGameRule : public GameRule
{

};

class 







bool LudumGameInstance::RespawnPlayerFromCheckpoint()
{

}

#endif

bool LudumGameInstance::DoCheckGameOverCondition() // shuludum : mututaliser le code
{

	return death::GameInstance::DoCheckGameOverCondition();




	LudumPlayer * player = GetPlayer(0);
	if (player != nullptr)
	{
		if (player->GetHealth() <= 0.0f) // no more energy => go to checkpoint
		{
			// game over mandatory (last life about to be removed)
			int life_count = player->GetLifeCount();
			if (life_count <= 1)
				return true;
			// keep some values to restore later
			int score = player->GetScore();
			// try to go to checkpoint
			if (!RestartFromRespawnCheckpoint())
				return true;
			// update player values after death
			player->SetScore(score, false);
			player->SetLifeCount(life_count - 1, false);
			player->OnLifeLost();
		}
	}
	return false;
}

void LudumGameInstance::FireExplosion(chaos::box2 const & ref_box)
{
	LudumLevelInstance* ludum_level_instance = GetLevelInstance();

    chaos::ParticleSpawner spawner = ludum_level_instance->GetParticleSpawner("Explosions", "explosion");
    if (!spawner.IsValid())
        return;

    chaos::BitmapAtlas::BitmapInfo const* explosion_info = spawner.GetBitmapInfo();

    spawner.SpawnParticles(1, false, [ref_box, explosion_info](chaos::ParticleAccessorBase<ParticleExplosion> accessor) 
    {
        for (ParticleExplosion& particle : accessor)
        {
            particle.bounding_box = ref_box;
            particle.bounding_box.half_size.x = particle.bounding_box.half_size.y = std::max(ref_box.half_size.x, ref_box.half_size.y);
            particle.color = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);
            particle.explosion_info = explosion_info;
        }
    });
}

void LudumGameInstance::OnLevelChanged(death::Level * new_level, death::Level * old_level, death::LevelInstance * new_level_instance)
{
	death::GameInstance::OnLevelChanged(new_level, old_level, new_level_instance);
	// play a sound
	if (new_level != nullptr && old_level != nullptr)
		game->PlaySound("next_level", false, false, 0.0f, death::SoundContext::GAME);
}
