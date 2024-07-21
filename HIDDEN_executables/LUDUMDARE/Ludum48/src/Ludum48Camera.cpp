#include "Ludum48PCH.h"
#include "Ludum48HUD.h"
#include "Ludum48Game.h"
#include "Ludum48Level.h"
#include "Ludum48Player.h"
#include "Ludum48LevelInstance.h"
#include "Ludum48GameInstance.h"
#include "Ludum48Camera.h"

LudumAutoRecenterToPlayerCameraComponent::LudumAutoRecenterToPlayerCameraComponent(size_t in_player_index):
	AutoRecenterToPlayerCameraComponent(in_player_index)
{
	in_player_index = in_player_index;
}

box2 LudumAutoRecenterToPlayerCameraComponent::GetTargetBox() const
{
	PlayerPawn const * player_pawn = GetPlayerPawn(player_index);
	if (player_pawn != nullptr)
	{
		GameObjectParticle const * particle = player_pawn->GetParticle<GameObjectParticle>(0);
		if (particle != nullptr)
		{
			box2 result = particle->bounding_box;
			result.position += particle->offset * glm::vec2(32.0f, 32.0f); // HACK


			previous_frame_box = result;

			return result;

		}
	}
	if (!IsGeometryEmpty(previous_frame_box))
		return previous_frame_box;
	return AutoRecenterToPlayerCameraComponent::GetTargetBox();
}
