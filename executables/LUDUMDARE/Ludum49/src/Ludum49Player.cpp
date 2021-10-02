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

	CHAOS_JSON_ATTRIBUTE(config, gravity);
	CHAOS_JSON_ATTRIBUTE(config, acceleration);
	CHAOS_JSON_ATTRIBUTE(config, max_velocity);

	

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

#if 0
int LudumPlayer::DoDisplay(GPURenderer* renderer, GPUProgramProviderBase const* uniform_provider, GPURenderParams const& render_params)
{
	int result = 0;

	glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
			glEnable(GL_BLEND);
			glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
			glDisable(GL_DEPTH_TEST);
			glDisable(GL_CULL_FACE);

	if (mesh != nullptr)
		result += mesh->Display(renderer, uniform_provider, render_params);


			glDisable(GL_BLEND);
			glEnable(GL_DEPTH_TEST);
			glEnable(GL_CULL_FACE);
			glPolygonMode(GL_FRONT_AND_BACK,  GL_FILL);
	return result;
}
#endif


