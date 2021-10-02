#include <chaos/Chaos.h>

#include "Ludum49PlayerDisplacementComponent.h"
#include "Ludum49Particles.h"
#include "Ludum49Player.h"
#include "Ludum49LevelInstance.h"










bool LudumPlayerDisplacementComponent::DoTick(float delta_time)
{
	chaos::PlayerDisplacementComponent::DoTick(delta_time);

	// early exit
	chaos::PlayerPawn* pawn = player->GetPawn();
	if (pawn == nullptr)
		return true;

	LudumPlayer* ludum_player = GetPlayer();

//	pawn_velocity.y += ludum_player->gravity * delta_time;

	glm::vec2 direction = player->GetLeftStickPosition(false);
	pawn_velocity.x += direction.x * ludum_player->acceleration * delta_time;
	



	chaos::Key const left_buttons[] = { chaos::KeyboardButton::LEFT, chaos::Key() };
	bool run_pressed = player->CheckButtonPressed(left_buttons);


	chaos::box2 pawn_box = pawn->GetBoundingBox();

	pawn_box.position += pawn_velocity * delta_time;




	//TMObjectCollisionIterator Iterator = GetLevelInstance()->GetObjectCollisionIterator(pawn_box, );









	pawn->SetPosition(pawn_box.position);


	return true;
}
