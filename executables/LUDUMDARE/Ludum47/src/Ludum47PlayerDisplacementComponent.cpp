#include "Ludum47PCH.h"
#include "Ludum47PlayerDisplacementComponent.h"
#include "Ludum47Particles.h"
#include "Ludum47Player.h"
#include "Ludum47LevelInstance.h"

bool LudumPlayerDisplacementComponent::ComputeBorderCollision(ParticleBase & particle, LudumLevelInstance* li, CarData const& car_data)
{
	bool result = false;

	// update velocity to indicates our intention to collision code
	glm::vec2 velocity_vector = particle.velocity * glm::vec2(std::cos(particle.rotation), std::sin(particle.rotation));




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
	player_obox.position = particle.bounding_box.position;
	player_obox.half_size = particle.bounding_box.half_size;
	player_obox.rotator = particle.rotation;


	glm::vec2 player_box_vertices[4];
	GetBoxVertices(player_obox, player_box_vertices, true);


	if (li != nullptr)
	{
		chaos::TMLayerInstance* layer = li->FindLayerInstance("Collisions", true);
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

							if (chaos::GLMTools::Get2DCrossProductZ(velocity_vector, b - a) < 0.0f) // CLOCKWISE
								continue;

							if (chaos::HasSeparatingPlane(transformed_box, &a, 2, false, transform))
								continue;
							if (chaos::IsSeparatingPlane(a, b, player_box_vertices, 4))
								continue;

							particle.velocity = 0;

							float c = std::cos((float)M_PI * 0.5f);
							float s = std::sin((float)M_PI * 0.5f);

							particle.collision_direction = glm::normalize(chaos::GLMTools::Rotate(b - a, c, s));


							// the intensity of the collision depends on the VELOCITY, but in case the VELOCITY is too small, use a minimal value (0.5 x MAX)

							float intensity = std::max(glm::abs(particle.velocity), car_data.max_velocity * 0.5f);

							particle.collision_reaction_intensity = intensity * car_data.reaction_value * std::max(0.5f, glm::dot(particle.collision_direction, -glm::normalize(velocity_vector)));

							result = true;
						}
					}
				}
			}
		}
	}
	return result;
}















bool LudumPlayerDisplacementComponent::DoTick(float delta_time)
{
	chaos::PlayerDisplacementComponent::DoTick(delta_time);

	// early exit
	chaos::PlayerPawn* pawn = player->GetPawn();
	if (pawn == nullptr)
		return true;

	LudumPlayer* player = GetPlayer();
	if (player == nullptr)
		return true;

	if (pawn->GetAllocation() == nullptr)
		return true;


	LudumLevelInstance* level_instance = player->GetLevelInstance();
	if (level_instance == nullptr || level_instance->effective_start_timer > 0.0f)
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

	chaos::Key const accelerate_keys[] = { chaos::Key::SPACE, chaos::Key::GAMEPAD_A, chaos::Key() };
	particle.accelerate_pressed = player->CheckKeyDown(accelerate_keys);
	chaos::Key const break_keys[] = { chaos::Key::LEFT_CONTROL, chaos::Key::GAMEPAD_B, chaos::Key() };
	particle.break_pressed = player->CheckKeyDown(break_keys);

	// no input before race really starts
	if (level_instance->lost_timer >= 0 || player->race_position.IsCompleted())
	{
		stick_position = { 0.0f , 0.0f };
		particle.accelerate_pressed = particle.break_pressed = false;
	}

	CarData const& car_data = player->car_data;

	chaos::box2 & pawn_box = particle.bounding_box;

	glm::vec2 & position = pawn_box.position;

	float angular_tweak = player->road->player_angular_tweak;

	float velocity_tweak = player->road->player_velocity_tweak;


	// update position BEFORE velocity so that we are sure that we cannot force ourt way through a collider

	glm::vec2 velocity_vector = particle.velocity * glm::vec2(std::cos(particle.rotation), std::sin(particle.rotation));

	float clamped_rotation = chaos::MathTools::DegreeToRadian(

		6.0f * std::floor(chaos::MathTools::RadianToDegree(particle.rotation) / 6.0f)
	);

	if (particle.collision_reaction_intensity > 0.0f)
	{
		particle.collision_reaction_intensity = std::max(0.0f, particle.collision_reaction_intensity - car_data.reaction_decrease * delta_time);
		if (particle.collision_reaction_intensity > 0.0f)
		{
			velocity_vector = particle.collision_direction * particle.collision_reaction_intensity;
			particle.bounding_box.position += velocity_vector * delta_time;
		}
	}

	if (particle.velocity != 0.0f)
	{
		particle.rotation += car_data.angular_velocity * delta_time * -stick_position.x * angular_tweak;

		velocity_vector = particle.velocity * glm::vec2(std::cos(clamped_rotation), std::sin(clamped_rotation));

		chaos::ApplyWrapMode(particle.rotation, -(float)M_PI, (float)M_PI, chaos::WrapMode::WRAP, particle.rotation);

		particle.bounding_box.position += velocity_vector * delta_time;
	}


	// compute wanted velocity

	if (particle.accelerate_pressed)
	{
		particle.velocity += delta_time * car_data.acceleration * velocity_tweak;
	}
	else
	{
		if (particle.break_pressed)
		{
			if (particle.velocity > 0.0f)
				particle.velocity -= delta_time * car_data.break_deceleration * velocity_tweak;
			else
				particle.velocity -= delta_time * car_data.acceleration * velocity_tweak;
		}
		else
		{
			if (particle.velocity > 0.0f)
				particle.velocity = std::max(0.0f, particle.velocity - delta_time * car_data.normal_deceleration * velocity_tweak);
			else if (particle.velocity < 0.0f)
				particle.velocity = std::min(0.0f, particle.velocity + delta_time * car_data.normal_deceleration * velocity_tweak);
		}
	}

	particle.velocity = std::clamp(particle.velocity, car_data.min_velocity * velocity_tweak, car_data.max_velocity * velocity_tweak);


	LudumLevelInstance* li = GetLevelInstance();

	if (ComputeBorderCollision(particle, li, car_data))
		player->SoundCollision();

	return true;
}
