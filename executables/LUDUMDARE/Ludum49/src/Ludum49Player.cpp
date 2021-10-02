#include <chaos/Chaos.h>

#include "Ludum49Player.h"
#include "Ludum49Level.h"
#include "Ludum49LevelInstance.h"
#include "Ludum49Game.h"
#include "Ludum49GameInstance.h"
#include "Ludum49Particles.h"

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


	


	return true;
}

bool LudumPlayer::InitializeGameValues(nlohmann::json const& config, bool hot_reload)
{
	if (!Player::InitializeGameValues(config, hot_reload))
		return false;

	

	return true;
}

void LudumPlayer::HandleInputs(float delta_time, chaos::GamepadState const* gpd)
{
	chaos::Player::HandleInputs(delta_time, gpd);


}



void LudumPlayer::OnLifeLost()
{
	chaos::Player::OnLifeLost();
	



}

void LudumPlayer::OnLevelChanged(chaos::Level * new_level, chaos::Level * old_level, chaos::LevelInstance * new_level_instance)
{
	chaos::Player::OnLevelChanged(new_level, old_level, new_level_instance);

}

bool LudumPlayer::DoTick(float delta_time)
{
	if (!chaos::Player::DoTick(delta_time))
		return false;

	return true;
}
