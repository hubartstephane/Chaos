#include "Ludum49PCH.h"
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

bool LudumPlayer::OnReadConfigurableProperties(JSONReadConfiguration config, ReadConfigurablePropertiesContext context)
{
	if (!Player::OnReadConfigurableProperties(config, context))
		return false;

	CHAOS_JSON_ATTRIBUTE(config, gravity);
	CHAOS_JSON_ATTRIBUTE(config, acceleration);
	CHAOS_JSON_ATTRIBUTE(config, max_velocity);
	CHAOS_JSON_ATTRIBUTE(config, slow_down);
	CHAOS_JSON_ATTRIBUTE(config, slow_down_factor);
	CHAOS_JSON_ATTRIBUTE(config, stop_velocity);
	CHAOS_JSON_ATTRIBUTE(config, touching_ground_lapse_time);





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
int LudumPlayer::DoDisplay(GPURenderContext* render_context, GPUProgramProviderInterface const * uniform_provider, GPURenderParams const& render_params)
{
	int result = 0;

	glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
			glEnable(GL_BLEND);
			glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
			glDisable(GL_DEPTH_TEST);
			glDisable(GL_CULL_FACE);

	if (mesh != nullptr)
		result += mesh->Display(render_context, uniform_provider, render_params);


			glDisable(GL_BLEND);
			glEnable(GL_DEPTH_TEST);
			glEnable(GL_CULL_FACE);
			glPolygonMode(GL_FRONT_AND_BACK,  GL_FILL);
	return result;
}
#endif


