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

	glm::vec2 direction = glm::vec2(std::cos(particle.rotation), std::sin(particle.rotation));

	if (particle.velocity > 0.0f)
	{
		particle.rotation += car_data.angular_velocity * delta_time * -stick_position.x * angular_tweak;
		chaos::ApplyWrapMode(particle.rotation, -(float)M_PI, (float)M_PI, chaos::WrapMode::WRAP, particle.rotation);

		particle.bounding_box.position += particle.velocity * direction * delta_time;
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

	// shu47 : conversion automatic     obox2 <=> box2 ???



	// while there may be a rotation, we have to extend the ALIGNED box for raw test
	chaos::box2 extended_box = GetBoundingBox(GetBoundingSphere(particle.bounding_box));


	// points in LudumCollision will be transformed into a referencial where the PLAYER BOX is at center
	chaos::box2 transformed_box;
	transformed_box.position = { 0.0f, 0.0f };
	transformed_box.half_size = particle.bounding_box.half_size;

	glm::mat4x4 transform = 
		chaos::GetRotatorMatrix(-particle.rotation) * 
		glm::translate(glm::vec3(-particle.bounding_box.position, 0.0f));


	chaos::obox2 player_obox;
	player_obox.position  = particle.bounding_box.position;
	player_obox.half_size = particle.bounding_box.half_size;
	player_obox.rotator   = particle.rotation;


	glm::vec2 player_box_vertices[4];
	GetBoxVertices(player_obox, player_box_vertices, true);

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
					if (chaos::Collide(col->internal_bounding_box, extended_box)) // raw collision check
					{
						size_t pcount = col->points.size();
						for (size_t k = 0; k < pcount - 1; ++k)
						{
							glm::vec2 const& a = col->points[k];
							glm::vec2 const& b = col->points[k + 1];
							if (a == b)
								continue;

							if (!Collide(chaos::box2(std::make_pair(a, b)), extended_box))
								continue;

							if (chaos::GLMTools::Get2DCrossProductZ(direction, b - a) > 0.0f)
								continue;


							if (chaos::HasSeparatingPlane(transformed_box, &a, 2, false, transform))
								continue;
							if (chaos::IsSeparatingPlane(a, b, player_box_vertices, 4))
								continue;

							particle.velocity = 0.0f;

							k = k;


						}





#if 0

						if (chaos::HasSeparatingPlane(transformed_box, &col->points[0], col->points.size(), false, transform))
							continue;

						size_t pcount = col->points.size();
						size_t k = 0;
						for (; k < pcount - 1; ++k)
						{
							glm::vec2 const& a = col->points[k];
							glm::vec2 const& b = col->points[k + 1];
							if (a == b)
								continue;

							if (chaos::IsSeparatingPlane(a, b, player_box_vertices, 4))
								break;
						}

						// collision happened
						if (k == pcount - 1)
						{
							k = k;
						}

						col = col;

#endif


					}
				}
			}
		}
	}



	return true;
}
