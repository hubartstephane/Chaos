#include <chaos/Chaos.h>

#include "Ludum49PlayerDisplacementComponent.h"
#include "Ludum49Particles.h"
#include "Ludum49Player.h"
#include "Ludum49LevelInstance.h"










bool LudumPlayerDisplacementComponent::DoTick(float delta_time)
{
	PlayerDisplacementComponent::DoTick(delta_time);

	// early exit
	PlayerPawn* pawn = player->GetPawn();
	if (pawn == nullptr)
		return true;

	LudumPlayer* ludum_player = GetPlayer();

//	pawn_velocity.y += ludum_player->gravity * delta_time;

	glm::vec2 direction = player->GetLeftStickPosition(false);
	direction.x = MathTools::AnalogicToDiscret(direction.x);
	direction.y = MathTools::AnalogicToDiscret(direction.y);
	
	if (direction.x == 0.0f || direction.x * pawn_velocity.x < 0.0f)
	{
		if (pawn_velocity.x > 0.0f)
			pawn_velocity.x = std::max(0.0f, pawn_velocity.x - ludum_player->slow_down * delta_time);
		else
			pawn_velocity.x = std::min(0.0f, pawn_velocity.x + ludum_player->slow_down * delta_time);
		


	}
	pawn_velocity.x += direction.x * ludum_player->acceleration * delta_time;
	



	Key const left_buttons[] = { KeyboardButton::LEFT, Key() };
	bool run_pressed = player->CheckButtonPressed(left_buttons);


	box2 pawn_box = pawn->GetBoundingBox();

	pawn_box.position += pawn_velocity * delta_time;


	LudumLevelInstance* ludum_level = GetLevelInstance();

	float min_distance = std::numeric_limits<float>::max();


	sphere2 pawn_sphere = GetInnerSphere(pawn_box);

	TMObjectCollisionIterator Iterator =ludum_level->GetObjectCollisionIterator(pawn_box, COLLISION_LANDSCAPE, false);
	while (Iterator)
	{
		
		TMObject & object = *Iterator;

		if (Landscape* landscape = auto_cast(&object))
		{
			size_t count = landscape->points.size();
			for (size_t i = 0; i < count; ++i)
			{
				glm::vec2 const& a = landscape->points[i];
				glm::vec2 const& b = landscape->points[(i + 1) % count];

				

			}


			landscape = landscape;
		}



		++Iterator;
	}









	pawn->SetPosition(pawn_box.position);


	return true;
}
