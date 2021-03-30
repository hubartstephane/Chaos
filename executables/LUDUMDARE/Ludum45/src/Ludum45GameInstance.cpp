#include <chaos/Chaos.h>

#include "Ludum45GameInstance.h"
#include "Ludum45Game.h"
#include "Ludum45Level.h"
#include "Ludum45LevelInstance.h"
#include "Ludum45Player.h"

LudumGameInstance::LudumGameInstance()
{
	player_class = LudumPlayer::GetStaticClass();
}
void LudumGameInstance::FireExplosion(chaos::box2 const & ref_box)
{
	LudumLevelInstance* ludum_level_instance = GetLevelInstance();

    chaos::ParticleSpawner spawner = ludum_level_instance->GetParticleSpawner("Explosions", "explosion");
    if (!spawner.IsValid())
        return;

    chaos::BitmapAtlas::BitmapInfo const* explosion_info = spawner.GetBitmapInfo();

    spawner.SpawnParticles(1, false).Process([ref_box, explosion_info](chaos::ParticleAccessor<ParticleExplosion> accessor) 
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

void LudumGameInstance::OnLevelChanged(chaos::Level * new_level, chaos::Level * old_level, chaos::LevelInstance * new_level_instance)
{
	chaos::GameInstance::OnLevelChanged(new_level, old_level, new_level_instance);
	// play a sound
	if (new_level != nullptr && old_level != nullptr)
		game->PlaySound("next_level", false, false, 0.0f, chaos::SoundContext::GAME);
}
