#include "Ludum46Player.h"
#include "Ludum46PlayerDisplacementComponent.h"
#include "Ludum46Level.h"
#include "Ludum46LevelInstance.h"
#include "Ludum46Game.h"
#include "Ludum46GameInstance.h"
#include "Ludum46Particles.h"


#include "chaos/TiledMap.h"
#include "chaos/MathTools.h"
#include "chaos/Direction.h"
#include "chaos/Edge.h"

#include "death/TM.h"
#include "death/CollisionMask.h"


bool LudumPlayerDisplacementComponent::DoTick(float delta_time)
{


	// early exit
	death::PlayerPawn* pawn = player->GetPawn();
	if (pawn == nullptr)
		return true;

	// get player inputs of interrests
	glm::vec2 stick_position = player->GetLeftStickPosition();

	stick_position.x = chaos::MathTools::AnalogicToDiscret(stick_position.x);
	stick_position.y = chaos::MathTools::AnalogicToDiscret(stick_position.y);

#if 0
	if (displacement_info.discrete_stick_mode)
	{
		stick_position.x = chaos::MathTools::AnalogicToDiscret(stick_position.x);
		stick_position.y = chaos::MathTools::AnalogicToDiscret(stick_position.y);
	}
#endif
	stick_position.y = -stick_position.y; // Y stick is inverted

	int const jump_key_buttons[] = { GLFW_KEY_SPACE, -1 };
	bool jump_pressed = player->CheckButtonPressed(jump_key_buttons, chaos::XBoxButton::BUTTON_A);

	int const run_key_buttons[] = { GLFW_KEY_LEFT_SHIFT, GLFW_KEY_RIGHT_SHIFT, -1 };
	bool run_pressed = player->CheckButtonPressed(run_key_buttons, chaos::XBoxButton::BUTTON_RIGHTTRIGGER);


	int const attack_key_buttons[] = { GLFW_KEY_LEFT_CONTROL, GLFW_KEY_RIGHT_CONTROL, -1 };

	// get player position
	chaos::box2 pawn_box = pawn->GetBoundingBox();

	chaos::box2 next_pawn_box = pawn_box;
	next_pawn_box.position += 200.0f * stick_position * delta_time;




	//next_pawn_box = ComputeCollisions(pawn_box, next_pawn_box, death::CollisionMask::PLAYER, pawn->GetAllocation(), "CollisionPlatformer", [](chaos::box2 const & b, death::TMParticle & p, chaos::Edge edge) 

	//next_pawn_box = death::ComputeTileCollisionAndReaction(GetLevelInstance(), pawn_box, next_pawn_box, death::CollisionMask::PLAYER, pawn->GetAllocation(), nullptr, [](chaos::box2 const& b, death::TMParticle& p, chaos::Edge edge)
	next_pawn_box = death::ComputeTileCollisionAndReaction(GetLevelInstance(), pawn_box, next_pawn_box, death::CollisionMask::PLAYER, pawn->GetAllocation(), "CollisionPlatformer", [](chaos::box2 const& b, death::TMParticle& p, chaos::Edge edge)
	{
	
		p.color.x = chaos::MathTools::RandFloat();
		p.color.y = chaos::MathTools::RandFloat();
		p.color.z = chaos::MathTools::RandFloat();
	
	});



	pawn->SetBoundingBox(next_pawn_box);

	return true;
}
