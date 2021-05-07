#include <chaos/Chaos.h>

#include "Ludum44GameInstance.h"
#include "Ludum44Game.h"
#include "Ludum44Level.h"
#include "Ludum44LevelInstance.h"
#include "Ludum44Player.h"

LudumGameInstance::LudumGameInstance()
{
	player_class = LudumPlayer::GetStaticClass();
}

void LudumGameInstance::OnLevelChanged(chaos::Level * new_level, chaos::Level * old_level, chaos::LevelInstance * new_level_instance)
{
	// super method
	chaos::GameInstance::OnLevelChanged(new_level, old_level, new_level_instance);
	// play a sound
	if (new_level != nullptr && old_level != nullptr)
		game->PlaySound("next_level", false, false, 0.0f, chaos::SoundContext::GAME);
	// internals
	current_power_up = nullptr;
	current_powerup_trigger = nullptr;
}

void LudumGameInstance::OnPlayerEntered(chaos::Player * player)
{
	chaos::GameInstance::OnPlayerEntered(player);

	LudumGame * ludum_game = GetGame();
	if (ludum_game == nullptr)
		return;

	LudumPlayer * ludum_player = auto_cast(player);
	if (ludum_player == nullptr)
		return;

	ludum_player->current_speed_index = 0;
	ludum_player->current_damage_index = 0;
	ludum_player->current_charged_damage_index = 0;
	ludum_player->current_fire_rate_index = 0;
}

void LudumGameInstance::OnPowerUpZone(chaos::Player * player, bool enter, PowerUpTrigger* powerup_trigger)
{
	LudumGame * ludum_game = GetGame();
	if (ludum_game == nullptr)
		return;

	LudumPlayer * ludum_player = auto_cast(player);
	if (ludum_player == nullptr)
		return;

	int count = (int)ludum_game->power_ups.size();
	if (count == 0)
		return;

	if (!enter)
	{
		current_power_up = nullptr;
		current_powerup_trigger = nullptr;
	}
	else
	{		
		int value = rand() % count;
		for (int i = 0 ; i < count ; ++i)
		{
			int index = (i + value) % count;
			if (ludum_game->power_ups[index]->CanPowerUp(ludum_game, ludum_player, powerup_trigger->decrease_power))
			{
				current_power_up = ludum_game->power_ups[index].get();	
				current_powerup_trigger = powerup_trigger;
				return;
			}
		}
	}
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
            particle.bounding_box.half_size.x = particle.bounding_box.half_size.y = 1.3f * std::max(ref_box.half_size.x, ref_box.half_size.y);
            particle.color = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);
            particle.explosion_info = explosion_info;
        }
    });
}

chaos::ParticleAccessor<ParticleFire> LudumGameInstance::FireProjectile(char const* bitmap_name, chaos::box2 const & ref_box, float ratio_to_box, int count, char const* sound_name, float offset_rotation, float delta_rotation, float velocity, float damage, bool trample, bool player_ownership)
{
    chaos::ParticleAccessor<ParticleFire> result;

    // early exit
    if (count <= 0)
        return result;
    LudumGame* ludum_game = GetGame();
    if (ludum_game == nullptr)
        return result;

    // create spawner
	LudumLevelInstance* ludum_level_instance = GetLevelInstance();

    chaos::ParticleSpawner spawner = ludum_level_instance->GetParticleSpawner("fire", bitmap_name);
    if (!spawner.IsValid() || !spawner.HasBitmap())
        return result;

    chaos::BitmapAtlas::BitmapLayout layout = *spawner.GetBitmapInfo();

    chaos::box2 particle_box = ref_box;
    particle_box.half_size = ratio_to_box * ref_box.half_size;
    particle_box = chaos::AlterBoxToAspect(particle_box, chaos::MathTools::CastAndDiv<float>(layout.width, layout.height), true);

    // spawn the particles and updates
    spawner.SpawnParticles(count, false).Process([offset_rotation, delta_rotation, particle_box, player_ownership, velocity, damage, trample, &result](chaos::ParticleAccessor<ParticleFire> accessor)
    {
        result = accessor;
        for (ParticleFire& particle : accessor)
        {
            int scroll_direction = 1;

            particle.bounding_box = particle_box;
            particle.color = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);

            float rotation = delta_rotation * ((float)accessor.GetDataIndex(&particle)) - delta_rotation * (float)(accessor.GetDataCount() / 2);

            float particle_orientation = rotation + offset_rotation;
            float particle_velocity_orientation = offset_rotation + rotation + (float)M_PI_2;

            glm::vec2 direction = glm::vec2(std::cos(particle_velocity_orientation), std::sin(particle_velocity_orientation));
            particle.velocity = velocity * direction;
            particle.rotation = particle_orientation;

            particle.bounding_box.position += direction * particle_box.half_size[scroll_direction];

            particle.damage = damage;
            particle.trample = trample;
            particle.player_ownership = player_ownership;
        }
    });

    // play some sound
    if (sound_name != nullptr)
        ludum_game->PlaySound(sound_name, false, false, 0.0f, chaos::SoundContext::LEVEL);

    return result;
}
