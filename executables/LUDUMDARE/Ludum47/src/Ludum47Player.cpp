#include <chaos/Chaos.h>

#include "Ludum47Player.h"
#include "Ludum47Level.h"
#include "Ludum47LevelInstance.h"
#include "Ludum47Game.h"
#include "Ludum47GameInstance.h"
#include "Ludum47Particles.h"

ParticlePlayer* LudumPlayer::GetPlayerParticle()
{
	if (pawn == nullptr)
		return nullptr;
	return pawn->GetParticle<ParticlePlayer>(0);
}

ParticlePlayer const * LudumPlayer::GetPlayerParticle() const
{
	if (pawn == nullptr)
		return nullptr;
	return pawn->GetParticle<ParticlePlayer>(0);
}

bool LudumPlayer::Initialize(chaos::GameInstance * in_game_instance)
{
	if (!chaos::Player::Initialize(in_game_instance))
		return false;

	spawner_delay.spawn_per_second = 20.0f;

	


	return true;
}

bool LudumPlayer::InitializeGameValues(nlohmann::json const& config, boost::filesystem::path const& config_path, bool hot_reload)
{
	if (!Player::InitializeGameValues(config, config_path, hot_reload))
		return false;

	

	chaos::JSONTools::GetAttribute(config, "min_velocity", car_data.min_velocity);
	chaos::JSONTools::GetAttribute(config, "max_velocity", car_data.max_velocity);
	chaos::JSONTools::GetAttribute(config, "acceleration", car_data.acceleration);
	chaos::JSONTools::GetAttribute(config, "angular_velocity", car_data.angular_velocity);
	chaos::JSONTools::GetAttribute(config, "normal_deceleration", car_data.normal_deceleration);
	chaos::JSONTools::GetAttribute(config, "break_deceleration", car_data.break_deceleration);

	chaos::JSONTools::GetAttribute(config, "reaction_value", car_data.reaction_value);
	chaos::JSONTools::GetAttribute(config, "reaction_decrease", car_data.reaction_decrease);
	chaos::JSONTools::GetAttribute(config, "reaction_decrease", car_data.reaction_decrease);

	CHAOS_JSON_ATTRIBUTE(config, velocity_collision_factor);
	

	CHAOS_JSON_ATTRIBUTE(config, collision_health_lost);

#if 0
	// cannot expand chaos::JSONTools::GetAttribute(config, #x, x)
	CHAOS_JSON_ATTRIBUTE(config, car_data.max_velocity);
	CHAOS_JSON_ATTRIBUTE(config, car_data.acceleration);
	CHAOS_JSON_ATTRIBUTE(config, car_data.angular_velocity);
	CHAOS_JSON_ATTRIBUTE(config, car_data.normal_deceleration);
	CHAOS_JSON_ATTRIBUTE(config, car_data.break_deceleration);
#endif
	return true;
}

void LudumPlayer::InternalHandleGamepadInputs(float delta_time, chaos::GamepadState const * gpd)
{
	chaos::Player::InternalHandleGamepadInputs(delta_time, gpd);


	// shu47 on aurait pu utiliser IsButtonPressed(...false) pour avoir la previous frame 

	bool honk_pressed = gpd->IsButtonPressed(chaos::GamepadButton::X, false);
	if (honk_pressed && !was_honk_pressed_gamepad)
		Honk();
	was_honk_pressed_gamepad = honk_pressed;

}

void LudumPlayer::HandleKeyboardInputs(float delta_time)
{
	chaos::Player::HandleKeyboardInputs(delta_time);

	// shu47 CheckKeyPressed on a rien pour connaitre la frame d'avant

	bool honk_pressed = CheckKeyPressed(chaos::KeyboardButton::LEFT_SHIFT);
	if (honk_pressed && !was_honk_pressed_keyboard)
		Honk();
	was_honk_pressed_keyboard = honk_pressed;
}

void LudumPlayer::Honk()
{
	// do not start before the beginning
	LudumLevelInstance* li = GetLevelInstance();
	if (li != nullptr && li->effective_start_timer > 0.0f)
		return;


	if (honk_sound != nullptr)
		return;
	chaos::Game* game = GetGame();
	if (game != nullptr)
		honk_sound = game->PlaySound("Honk", false, false, 0.0f, chaos::SoundContext::GAME);
}

void LudumPlayer::SpawnSmokeParticles(size_t count, LudumLevelInstance* li, chaos::obox2 const & ob)
{
	if (li != nullptr && count > 0)
	{
		chaos::ParticleSpawner spawner = li->GetParticleSpawner("Smoke", "Smoke"); // va chercher layer + sprite
		if (spawner.IsValid())
		{
			glm::vec2 vertices[4];
			chaos::GetBoxVertices(ob, vertices);

			glm::vec2 spawn_pos = (vertices[0] + vertices[3]) * 0.5f - 0.5f * (vertices[3] - vertices[2]);





			spawner.SpawnParticles(count, false, [spawn_pos](chaos::ParticleAccessor<ParticleSmoke> accessor)
			{
				glm::vec2 pos = spawn_pos;

				for (ParticleSmoke& p : accessor)
				{
					p.bounding_box.position = pos;
					p.bounding_box.half_size = { 16.0f, 16.0f };

					float angle = chaos::MathTools::RandFloat() * 2.0f * float(M_PI);

					p.velocity = chaos::MathTools::RandFloat(1.0f, 2.0f) * glm::vec2(std::cos(angle), std::sin(angle)) * 20.0f;
					p.lifetime = p.duration = chaos::MathTools::RandFloat(0.75f, 1.5f);
					p.angular_velocity = chaos::MathTools::RandFloat(1.3f, 2.6f);

					float c = 0.0f;
					p.color = glm::vec4(c, c, c, c); // invisible particle for the very first frame
				}


			});
		}
	}
}

void LudumPlayer::SoundCollision()
{
	PlayHealthChangedEffects(true); // play the fx

	if (sound_collision_timer > 0.0f)
		return;
	chaos::Game* game = GetGame();
	if (game != nullptr)
		game->PlaySound("Explosion", false, false, 0.0f, chaos::SoundContext::GAME);

	sound_collision_timer = 0.2f;

}


void LudumPlayer::OnLifeLost()
{
	chaos::Player::OnLifeLost();
	



}

void LudumPlayer::OnLevelChanged(chaos::Level * new_level, chaos::Level * old_level, chaos::LevelInstance * new_level_instance)
{
	chaos::Player::OnLevelChanged(new_level, old_level, new_level_instance);

	if (new_level_instance != nullptr)
	{
		LudumLevelInstance* li = auto_cast(new_level_instance);
		if (li != nullptr)
		{
			road = li->road;
		}
	}
	// reset the race
	race_position = {};
}

bool LudumPlayer::DoTick(float delta_time)
{
	if (!chaos::Player::DoTick(delta_time))
		return false;

	LudumLevelInstance* li = GetLevelInstance();
	if (li != nullptr && li->effective_start_timer == 0.0)
	{
		if (pawn != nullptr)
		{
			size_t smoke_particle = (size_t)spawner_delay.GetSpawnCount(delta_time);
			SpawnSmokeParticles(smoke_particle, GetLevelInstance(), pawn->GetOBox());
		}

	}


	if (road != nullptr)
	{
		ParticlePlayer const * particle = GetPlayerParticle();
		if (particle != nullptr)
			road->UpdateRacePosition(race_position, particle->bounding_box.position, true);
	}


	sound_collision_timer = std::max(0.0f, sound_collision_timer - delta_time);

	if (honk_sound != nullptr)
		if (honk_sound->IsFinished())
			honk_sound = nullptr;

	return true;
}
