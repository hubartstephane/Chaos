#include <chaos/Chaos.h>

#include "Ludum48HUD.h"
#include "Ludum48Game.h"
#include "Ludum48Level.h"
#include "Ludum48Player.h"
#include "Ludum48LevelInstance.h"
#include "Ludum48GameInstance.h"
#include "Ludum48Camera.h"

MyAutoRecenterCameraComponent::MyAutoRecenterCameraComponent(size_t in_player_index):
	chaos::AutoRecenterToPlayerCameraComponent(in_player_index)
{
	in_player_index = in_player_index;
}

chaos::box2 MyAutoRecenterCameraComponent::GetTargetBox() const
{
	chaos::PlayerPawn const * player_pawn = GetPlayerPawn(player_index);
	if (player_pawn != nullptr)
	{
		GameObjectParticle const * particle = player_pawn->GetParticle<GameObjectParticle>(0);
		if (particle != nullptr)
		{
			chaos::box2 result = particle->bounding_box;
			result.position += particle->offset * glm::vec2(32.0f, 32.0f);
			return result;

		}
	}
	return chaos::AutoRecenterToPlayerCameraComponent::GetTargetBox();
}
