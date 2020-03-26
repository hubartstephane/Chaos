#include "LudumTestPlayer.h"
#include "LudumTestLevel.h"
#include "LudumTestLevelInstance.h"
#include "LudumTestGame.h"
#include "LudumTestGameInstance.h"
#include "LudumTestParticles.h"
#include "LudumTestGameCheckpoint.h"

#include <chaos/MathTools.h>
#include <chaos/ParticleLayer.h>

#include <death/Level.h>
#include <death/SoundContext.h>

LudumPlayer::LudumPlayer(death::GameInstance * in_game_instance) : 
	death::Player(in_game_instance)
{
}

bool LudumPlayer::Initialize(death::GameInstance * in_game_instance)
{
	if (!death::Player::Initialize(in_game_instance))
		return false;



	return true;
}





void LudumPlayer::TickPlayerDisplacement(float delta_time)
{
    Player::TickPlayerDisplacement(delta_time);




}




void LudumPlayer::InternalHandleGamepadInputs(float delta_time, chaos::MyGLFW::GamepadData const * gpd)
{
	death::Player::InternalHandleGamepadInputs(delta_time, gpd);


}

void LudumPlayer::HandleKeyboardInputs(float delta_time)
{
	death::Player::HandleKeyboardInputs(delta_time);


}

void LudumPlayer::OnLevelChanged(death::Level * new_level, death::Level * old_level, death::LevelInstance * new_level_instance)
{
	death::Player::OnLevelChanged(new_level, old_level, new_level_instance);

}

death::PlayerCheckpoint * LudumPlayer::DoCreateCheckpoint() const
{
	return new LudumPlayerCheckpoint();
}

bool LudumPlayer::DoLoadFromCheckpoint(death::PlayerCheckpoint const * checkpoint)
{
	LudumPlayerCheckpoint const * ludum_checkpoint = auto_cast(checkpoint);
	if (ludum_checkpoint == nullptr)
		return false;

	if (!death::Player::DoLoadFromCheckpoint(checkpoint))
		return false;





	return true;
}

bool LudumPlayer::DoSaveIntoCheckpoint(death::PlayerCheckpoint * checkpoint) const
{
	LudumPlayerCheckpoint * ludum_checkpoint = auto_cast(checkpoint);
	if (ludum_checkpoint == nullptr)
		return false;

	if (!death::Player::DoSaveIntoCheckpoint(checkpoint))
		return false;



	return true;
}
