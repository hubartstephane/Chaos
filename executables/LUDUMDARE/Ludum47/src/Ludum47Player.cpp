#include "Ludum47Player.h"
#include "Ludum47Level.h"
#include "Ludum47LevelInstance.h"
#include "Ludum47Game.h"
#include "Ludum47GameInstance.h"
#include "Ludum47Particles.h"

#include <chaos/MathTools.h>
#include <chaos/ParticleLayer.h>

#include <death/Level.h>
#include <death/SoundContext.h>

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

bool LudumPlayer::Initialize(death::GameInstance * in_game_instance)
{
	if (!death::Player::Initialize(in_game_instance))
		return false;



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



#if 0
	// cannot expand chaos::JSONTools::GetAttribute(config, #x, x)
	DEATHGAME_JSON_ATTRIBUTE(car_data.max_velocity);
	DEATHGAME_JSON_ATTRIBUTE(car_data.acceleration);
	DEATHGAME_JSON_ATTRIBUTE(car_data.angular_velocity);
	DEATHGAME_JSON_ATTRIBUTE(car_data.normal_deceleration);
	DEATHGAME_JSON_ATTRIBUTE(car_data.break_deceleration);
#endif
	return true;
}

void LudumPlayer::InternalHandleGamepadInputs(float delta_time, chaos::MyGLFW::GamepadData const * gpd)
{
	death::Player::InternalHandleGamepadInputs(delta_time, gpd);


	// shu47 on aurait pu utiliser IsButtonPressed(...false) pour avoir la previous frame 

	bool honk_pressed = gpd->IsButtonPressed(chaos::XBoxButton::BUTTON_X, false);
	if (honk_pressed && !was_honk_pressed_gamepad)
		Honk();
	was_honk_pressed_gamepad = honk_pressed;

}

void LudumPlayer::HandleKeyboardInputs(float delta_time)
{
	death::Player::HandleKeyboardInputs(delta_time);

	// shu47 CheckKeyPressed on a rien pour connaitre la frame d'avant

	bool honk_pressed = CheckKeyPressed(GLFW_KEY_LEFT_SHIFT);
	if (honk_pressed && !was_honk_pressed_keyboard)
		Honk();
	was_honk_pressed_keyboard = honk_pressed;
}

void LudumPlayer::Honk()
{
	if (honk_sound != nullptr)
		return;
	death::Game* game = GetGame();
	if (game != nullptr)
		honk_sound = game->PlaySound("Honk", false, false, 0.0f, death::SoundContext::GAME);
}




















void LudumPlayer::OnLifeLost()
{
	death::Player::OnLifeLost();
	
}

void LudumPlayer::OnLevelChanged(death::Level * new_level, death::Level * old_level, death::LevelInstance * new_level_instance)
{
	death::Player::OnLevelChanged(new_level, old_level, new_level_instance);

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
	if (!death::Player::DoTick(delta_time))
		return false;



	if (road != nullptr)
	{
		ParticlePlayer const * particle = GetPlayerParticle();
		if (particle != nullptr)
		{
			road->UpdateRacePosition(race_position, particle->bounding_box.position, true);



		}
	}

	// do not start before the beginning
	LudumLevelInstance* li = GetLevelInstance();
	if (li != nullptr && li->effective_start_timer > 0.0f)
		return true;


	if (honk_sound != nullptr)
		if (honk_sound->IsFinished())
			honk_sound = nullptr;



	return true;
}
