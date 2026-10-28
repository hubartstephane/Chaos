#include "Ludum47PCH.h"
#include "Ludum47PlayerDisplacementComponent.h"
#include "Ludum47Particles.h"
#include "Ludum47Player.h"
#include "Ludum47LevelInstance.h"



ParticlePlayer * LudumPlayerDisplacementComponent::GetParticlePlayer()
{
	// early exit
	chaos::PlayerPawn* pawn = player->GetPawn();
	if (pawn == nullptr)
		return nullptr;

	LudumPlayer* player = GetPlayer();
	if (player == nullptr)
		return nullptr;

	if (pawn->GetAllocation() == nullptr)
		return nullptr;

	chaos::ParticleAccessor<ParticlePlayer> accessor = pawn->GetAllocation()->GetParticleAccessor<ParticlePlayer>();
	if (accessor.GetDataCount() == 0)
		return nullptr;

	return &accessor[0];
}

bool LudumPlayerDisplacementComponent::EnumerateInputActions(chaos::InputActionProcessor& in_action_processor, chaos::EnumerateInputActionContext in_context)
{
	// accelerate
	auto AccelerateInput = chaos::CombinedInput(chaos::Key::Space, chaos::Key::GamepadA);

	bool accelerate_pressed = false;

	auto AccelerateInputCondition = chaos::QueryInput(AccelerateInput, &accelerate_pressed);

	if (in_action_processor.CheckAndProcess(AccelerateInputCondition, "Accelerate", [&]()
	{
		if (ParticlePlayer * player_particle = GetParticlePlayer())
		{
			player_particle->accelerate_pressed = accelerate_pressed;
		}		
	}))
	{
		return true;
	}

	// break
	auto BreakInput = chaos::CombinedInput(chaos::Key::LeftControl, chaos::Key::GamepadB);

	bool break_pressed = false;

	auto BreakInputCondition = chaos::QueryInput(BreakInput, &break_pressed);

	if (in_action_processor.CheckAndProcess(BreakInputCondition, "Break", [&]()
	{
		if (ParticlePlayer * player_particle = GetParticlePlayer())
		{
			player_particle->break_pressed = break_pressed;
		}		
	}))
	{
		return true;
	}

	// move
	auto MoveInput = chaos::CombinedInput(chaos::Input2D::GamepadLeftStick, chaos::Input2D::GamepadLeftStick, chaos::MappedInput2D::keyboard_arrows);

	glm::vec2 stick_position = {0.0f, 0.0f};

	auto MoveInputCondition = chaos::QueryInput(MoveInput, &stick_position);

	if (in_action_processor.CheckAndProcess(MoveInputCondition, "Move", [&]()
	{
		if (ParticlePlayer* player_particle = GetParticlePlayer())
		{
			stick_position.x = chaos::MathTools::AnalogicToDiscret(stick_position.x);
			stick_position.y = chaos::MathTools::AnalogicToDiscret(stick_position.y);
			player_particle->stick_position = stick_position;
		}
	}))
	{
		return true;
	}

	return Super::EnumerateInputActions(in_action_processor, in_context);
}

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
		chaos::GetRotationMatrix(-particle.rotation) *
		glm::translate(glm::vec3(-particle.bounding_box.position, 0.0f));


	chaos::obox2 player_obox;
	player_obox.position = particle.bounding_box.position;
	player_obox.half_size = particle.bounding_box.half_size;
	player_obox.rotation = particle.rotation;


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

							if (!Collide(chaos::box2(a, b, chaos::AnyTwoPoints), extended_box))
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

	LudumPlayer* player = GetPlayer();
	if (player == nullptr)
		return true;

	LudumLevelInstance* level_instance = player->GetLevelInstance();
	if (level_instance == nullptr || level_instance->effective_start_timer > 0.0f)
		return true;

	ParticlePlayer * particle_player = GetParticlePlayer();
	if (particle_player == nullptr)
		return false;

	// no input before race really starts
	if (level_instance->lost_timer >= 0 || player->race_position.IsCompleted())
	{
		particle_player->stick_position = { 0.0f , 0.0f };
		particle_player->accelerate_pressed = particle_player->break_pressed = false;
	}

	CarData const& car_data = player->car_data;

	chaos::box2 & pawn_box = particle_player->bounding_box;

	glm::vec2 & position = pawn_box.position;

	float angular_tweak = player->road->player_angular_tweak;

	float velocity_tweak = player->road->player_velocity_tweak;


	// update position BEFORE velocity so that we are sure that we cannot force ourt way through a collider

	glm::vec2 velocity_vector = particle_player->velocity * glm::vec2(std::cos(particle_player->rotation), std::sin(particle_player->rotation));

	float clamped_rotation = chaos::MathTools::DegreeToRadian(

		6.0f * std::floor(chaos::MathTools::RadianToDegree(particle_player->rotation) / 6.0f)
	);

	if (particle_player->collision_reaction_intensity > 0.0f)
	{
		particle_player->collision_reaction_intensity = std::max(0.0f, particle_player->collision_reaction_intensity - car_data.reaction_decrease * delta_time);
		if (particle_player->collision_reaction_intensity > 0.0f)
		{
			velocity_vector = particle_player->collision_direction * particle_player->collision_reaction_intensity;
			particle_player->bounding_box.position += velocity_vector * delta_time;
		}
	}

	if (particle_player->velocity != 0.0f)
	{
		particle_player->rotation += car_data.angular_velocity * delta_time * -particle_player->stick_position.x * angular_tweak;

		velocity_vector = particle_player->velocity * glm::vec2(std::cos(clamped_rotation), std::sin(clamped_rotation));

		chaos::ApplyWrapMode(particle_player->rotation, -(float)M_PI, (float)M_PI, chaos::WrapMode::Wrap, particle_player->rotation);

		particle_player->bounding_box.position += velocity_vector * delta_time;
	}


	// compute wanted velocity

	if (particle_player->accelerate_pressed)
	{
		particle_player->velocity += delta_time * car_data.acceleration * velocity_tweak;
	}
	else
	{
		if (particle_player->break_pressed)
		{
			if (particle_player->velocity > 0.0f)
				particle_player->velocity -= delta_time * car_data.break_deceleration * velocity_tweak;
			else
				particle_player->velocity -= delta_time * car_data.acceleration * velocity_tweak;
		}
		else
		{
			if (particle_player->velocity > 0.0f)
				particle_player->velocity = std::max(0.0f, particle_player->velocity - delta_time * car_data.normal_deceleration * velocity_tweak);
			else if (particle_player->velocity < 0.0f)
				particle_player->velocity = std::min(0.0f, particle_player->velocity + delta_time * car_data.normal_deceleration * velocity_tweak);
		}
	}

	particle_player->velocity = std::clamp(particle_player->velocity, car_data.min_velocity * velocity_tweak, car_data.max_velocity * velocity_tweak);


	LudumLevelInstance* li = GetLevelInstance();

	if (ComputeBorderCollision(*particle_player, li, car_data))
		player->SoundCollision();

	return true;
}
