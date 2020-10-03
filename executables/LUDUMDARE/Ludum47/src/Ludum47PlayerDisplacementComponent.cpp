#include "Ludum47PlayerDisplacementComponent.h"
#include "Ludum47Particles.h"
#include "Ludum47Player.h"

#include <death/PlayerPawn.h>

bool LudumPlayerDisplacementComponent::DoTick(float delta_time)
{
	death::PlayerDisplacementComponent::DoTick(delta_time);

	// early exit
	death::PlayerPawn* pawn = player->GetPawn();
	if (pawn == nullptr)
		return true;

	LudumPlayer* player = GetPlayer();
	if (player == nullptr)
		return true;

	if (pawn->GetAllocation() == nullptr)
		return true;

	chaos::ParticleAccessor<ParticlePlayer> accessor = pawn->GetAllocation()->GetParticleAccessor<ParticlePlayer>();
	if (accessor.GetDataCount() == 0)
		return true;

	ParticlePlayer& particle = accessor[0];


	// get player inputs of interrests
	glm::vec2 stick_position = player->GetLeftStickPosition();

	bool discrete_stick_mode = true;
	if (discrete_stick_mode)
	{
		stick_position.x = chaos::MathTools::AnalogicToDiscret(stick_position.x);
		stick_position.y = chaos::MathTools::AnalogicToDiscret(stick_position.y);
	}
	stick_position.y = -stick_position.y; // Y stick is inverted

	int const accelerate_keys[] = { GLFW_KEY_SPACE, -1 };
	bool accelerate_pressed = player->CheckButtonPressed(accelerate_keys, chaos::XBoxButton::BUTTON_A);
	int const break_keys[] = { GLFW_KEY_SPACE, -1 };
	bool break_pressed = player->CheckButtonPressed(break_keys, chaos::XBoxButton::BUTTON_B);

#if 0





	int const run_key_buttons[] = { GLFW_KEY_LEFT_SHIFT, GLFW_KEY_RIGHT_SHIFT, -1 };
	bool run_pressed = player->CheckButtonPressed(run_key_buttons, chaos::XBoxButton::BUTTON_RIGHTTRIGGER);

#endif

	chaos::box2 & pawn_box = particle.bounding_box;

	glm::vec2 & position = pawn_box.position;

	//if (glm::length2(particle.velocity) > 0.0f)
	{
		particle.rotation += player->angular_velocity * delta_time * -stick_position.x;


	}

	if (accelerate_pressed)
	{
		particle.velocity += glm::vec2(std::cos(particle.rotation), std::sin(particle.rotation)) * delta_time * player->acceleration;


	}

	float velocity_length = glm::length(particle.velocity);
	if (velocity_length > player->max_velocity)
		particle.velocity = glm::normalize(particle.velocity) * player->max_velocity;

	//pawn->SetBoundingBox(pawn_box);


	return true;
}
