#include "Ludum47PlayerDisplacementComponent.h"
#include "Ludum47Particles.h"
#include "Ludum47Player.h"
#include "Ludum47LevelInstance.h"


#include <chaos/WrapMode.h>
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


	CarData const& car_data = player->car_data;

	chaos::box2 & pawn_box = particle.bounding_box;

	glm::vec2 & position = pawn_box.position;

	float angular_tweak = player->road->player_angular_tweak;

	if (particle.velocity > 0.0f)
	{
		particle.rotation += car_data.angular_velocity * delta_time * -stick_position.x * angular_tweak;
		chaos::ApplyWrapMode(particle.rotation, -(float)M_PI, (float)M_PI, chaos::WrapMode::WRAP, particle.rotation);
	}

	float velocity_tweak = player->road->player_velocity_tweak;
	

	if (accelerate_pressed)
	{
		particle.velocity += delta_time * car_data.acceleration * velocity_tweak;


	}
	else
	{
		if (break_pressed)
			particle.velocity -= delta_time * car_data.break_deceleration * velocity_tweak;
		else
			particle.velocity -= delta_time * car_data.normal_deceleration * velocity_tweak;



		particle.velocity = std::max(particle.velocity, 0.0f);
	}



	particle.velocity = std::min(particle.velocity, car_data.max_velocity * velocity_tweak);


	// search collision

	// shu47 : conversion automatic

	chaos::obox2 player_obox;
	player_obox.position = particle.bounding_box.position;
	player_obox.half_size = particle.bounding_box.half_size;
	player_obox.rotator = particle.rotation;


	chaos::box2 transformed_box;
	transformed_box.position = { 0.0f, 0.0f };
	transformed_box.half_size = particle.bounding_box.half_size;

	glm::mat4x4 transform = 
		chaos::GetRotatorMatrix(-particle.rotation) * 
		glm::translate(glm::vec3(-particle.bounding_box.position, 0.0f));

	LudumLevelInstance* li = GetLevelInstance();
	if (li != nullptr)
	{
		death::TMLayerInstance* layer = li->FindLayerInstance("Collisions");
		if (layer != nullptr)
		{
			size_t count = layer->GetObjectCount();
			for (size_t i = 0; i < count; ++i)
			{
				LudumCollision* col = layer->GetObject(i);
				if (col != nullptr && col->points.size() >= 2)
				{





//					if (chaos::Collide(col->internal_bounding_box, particle.bounding_box))
					{

						size_t pcount = col->points.size();
						for (size_t i = 0; i < pcount - 1; ++i)
						{
							glm::vec2 const& a = col->points[i];
							glm::vec2 const& b = col->points[i + 1];
							if (a == b)
								continue;

							if (!chaos::HasSeparatingPlane(transformed_box, &col->points[0], col->points.size(), transform))
							{



								i = i;
							}












						}






















						col = col;
					}
				}
			}
		}
	}



	return true;
}
