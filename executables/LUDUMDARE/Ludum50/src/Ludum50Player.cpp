#include "Ludum50PCH.h"
#include "Ludum50Player.h"
#include "Ludum50Level.h"
#include "Ludum50LevelInstance.h"
#include "Ludum50Game.h"
#include "Ludum50GameInstance.h"
#include "Ludum50Particles.h"

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

bool LudumPlayer::Initialize(GameInstance * in_game_instance)
{
	if (!Player::Initialize(in_game_instance))
		return false;





	return true;
}

bool LudumPlayer::InitializeGameValues(nlohmann::json const& config, bool hot_reload)
{
	if (!Player::InitializeGameValues(config, hot_reload))
		return false;







	return true;
}

void LudumPlayer::HandleInputs(float delta_time, GamepadState const* gpd)
{
	Player::HandleInputs(delta_time, gpd);


}



void LudumPlayer::OnLifeLost()
{
	Player::OnLifeLost();




}

void LudumPlayer::OnLevelChanged(Level * new_level, Level * old_level, LevelInstance * new_level_instance)
{
	Player::OnLevelChanged(new_level, old_level, new_level_instance);

}

bool LudumPlayer::DoTick(float delta_time)
{
	if (!Player::DoTick(delta_time))
		return false;

	return true;
}
