#include "Ludum47Level.h"
#include "Ludum47LevelInstance.h"
#include "Ludum47Game.h"
#include "Ludum47Player.h"
#include "Ludum47GameInstance.h"

#include <chaos/GLMTools.h>
#include <chaos/ParticleDefault.h>
#include <chaos/GeometryFramework.h>
#include <chaos/CollisionFramework.h>
#include <chaos/ParticleSpawner.h>
#include <chaos/StringTools.h>
#include <chaos/WrapMode.h>


#include <death/TM.h>



// =============================================================
// LudumOpponent implementation
// =============================================================

bool LudumOpponent::Initialize(death::TMLayerInstance* in_layer_instance, chaos::TiledMap::GeometricObject const* in_geometric_object, death::TMObjectReferenceSolver& reference_solver)
{
	if (!death::TMObject::Initialize(in_layer_instance, in_geometric_object, reference_solver))
		return false;

	car_data.max_velocity = in_geometric_object->GetPropertyValueFloat("MIN_VELOCITY", car_data.max_velocity);
	car_data.max_velocity = in_geometric_object->GetPropertyValueFloat("MAX_VELOCITY", car_data.max_velocity);
	car_data.acceleration = in_geometric_object->GetPropertyValueFloat("ACCELERATION", car_data.acceleration);
	car_data.angular_velocity = in_geometric_object->GetPropertyValueFloat("ANGULAR_VELOCITY", car_data.angular_velocity);
	car_data.normal_deceleration = in_geometric_object->GetPropertyValueFloat("NORMAL_DECELERATION", car_data.normal_deceleration);
	car_data.break_deceleration = in_geometric_object->GetPropertyValueFloat("BREAK_DECELERATION", car_data.break_deceleration);
	


	car_data.reaction_value = in_geometric_object->GetPropertyValueFloat("REACTION_VALUE", car_data.reaction_value);
	car_data.reaction_decrease = in_geometric_object->GetPropertyValueFloat("REACTION_DECREASE", car_data.reaction_decrease);
	
	max_health = in_geometric_object->GetPropertyValueFloat("MAX_HEALTH", max_health);



	
	health = max_health;

	return true;
}

void LudumOpponent::OnLevelStarted()
{
	death::TMObject::OnLevelStarted();


	LudumLevelInstance* li = layer_instance->GetLevelInstance();
	if (li != nullptr)
	{
		road = li->road;
	}
}

bool LudumOpponent::DoTick(float delta_time)
{
	if (!TMObject::DoTick(delta_time))
		return false;

	size_t road_point_count = road->points.size();
	if (road == nullptr || road->points.size() == 0)
		return true;

	ParticleOpponent* particle = GetParticle<ParticleOpponent>(0);
	if (particle != nullptr)
	{

		if (particle->collision_reaction_intensity > 0.0f)
		{
			particle->collision_reaction_intensity = std::max(0.0f, particle->collision_reaction_intensity - car_data.reaction_decrease * delta_time);
			if (particle->collision_reaction_intensity > 0.0f)
			{
				glm::vec2 velocity_vector = particle->collision_direction * particle->collision_reaction_intensity;
				
				
				//particle->bounding_box.position += velocity_vector * delta_time;

				bounding_box.position += velocity_vector * delta_time;
				
				
				//goto skip_opponent_input; // :)

			}
		}







		//

		RoadPoint const& target = road->points[(race_position.current_road_point + 1) % road_point_count];

		float wr = std::atan2(target.position.y - bounding_box.position.y, target.position.x - bounding_box.position.x);
		float cr = rotation;

		float dir = 0.0f;

		float a = wr - cr;
		if (std::abs(a) <= M_PI)
		{
			dir = (wr < cr) ? -1.0f : +1.0f;
		}
		else
		{
			dir = (wr < cr) ? +1.0f : -1.0f;
		}

		float angular_tweak = road->opponent_angular_tweak;

		rotation += dir * delta_time * car_data.angular_velocity * angular_tweak;
		chaos::ApplyWrapMode(rotation, -(float)M_PI, (float)M_PI, chaos::WrapMode::WRAP, rotation);


		float velocity_tweak = road->opponent_velocity_tweak;

		float sf = road->GetSpeedFactor(bounding_box.position);

		float target_velocity = car_data.max_velocity * sf * velocity_tweak;


		if (target_velocity > particle->velocity)
			particle->velocity = std::min(target_velocity, particle->velocity + car_data.acceleration * delta_time * velocity_tweak);
		else
			particle->velocity = target_velocity; // immediate break

		bounding_box.position += glm::vec2(std::cos(rotation), std::sin(rotation)) * delta_time * particle->velocity;

skip_opponent_input:

		if (road->UpdateRacePosition(race_position, bounding_box.position, false) == RoadUpdateValue::END_OF_RACE)
			allocations = nullptr;


		SynchronizeData(false);
	}
	return true;
}


// =============================================================
// LudumSpeedIndication implementation
// =============================================================

bool LudumSpeedIndication::Initialize(death::TMLayerInstance* in_layer_instance, chaos::TiledMap::GeometricObject const* in_geometric_object, death::TMObjectReferenceSolver& reference_solver)
{
	if (!death::TMObject::Initialize(in_layer_instance, in_geometric_object, reference_solver))
		return false;

	speed_factor = in_geometric_object->GetPropertyValueFloat("SPEED_FACTOR", speed_factor);

	return true;
}

// =============================================================
// LudumRoad implementation
// =============================================================


bool LudumRoad::DoTick(float delta_time)
{
	if (!death::TMObject::DoTick(delta_time))
		return false;

	// seach all opponents
	LudumLevelInstance * level_instance = layer_instance->GetLevelInstance();
	if (level_instance == nullptr)
		return true;


	death::TMLayerInstance * li = level_instance->FindLayerInstance("Opponents");
	if (li == nullptr)
		return true;

	std::vector<LudumOpponent*> opponents;
	
	size_t object_count = li->GetObjectCount();
	for (size_t i = 0; i < object_count; ++i)
	{
		LudumOpponent* opp = li->GetObject(i);
		if (opp != nullptr)
		{
			opponents.push_back(opp);
		}
	}

	if (opponents.size() == 0)
		return true;

	// search player
	LudumPlayer* player = level_instance->GetPlayer(0);

	ParticlePlayer* player_particle = (player != nullptr) ? player->GetPlayerParticle() : nullptr;




	

	// for each pair of opponents
	size_t opponent_count = opponents.size();
	for (size_t i = 0; i < opponent_count - 1; ++i)
	{
		for (size_t j = i + 1 ; j < opponent_count ; ++j)
		{
			LudumOpponent* opp1 = opponents[i];
			LudumOpponent* opp2 = opponents[j];

			ParticleOpponent * p1 = opp1->GetParticle<ParticleOpponent>(0);
			ParticleOpponent * p2 = opp2->GetParticle<ParticleOpponent>(0);
			if (p1 == nullptr || p2 == nullptr)
				continue;

			glm::vec2 vel_dir1 = p1->velocity * glm::vec2(std::cos(p1->rotation), std::sin(p1->rotation));
			glm::vec2 vel_dir2 = p2->velocity * glm::vec2(std::cos(p2->rotation), std::sin(p2->rotation));
			if (glm::dot(vel_dir1, vel_dir2) < 0.0f) // already getting away from one another
				continue;


			auto b1 = opp1->GetBoundingBox(true);
			auto b2 = opp2->GetBoundingBox(true);

			chaos::obox2 ob1;
			ob1.position = b1.position;
			ob1.half_size = b1.half_size;
			ob1.rotator = opp1->GetRotation();

			chaos::obox2 ob2;
			ob2.position = b2.position;
			ob2.half_size = b2.half_size;
			ob2.rotator = opp2->GetRotation();

			// raw evaluation 
			if (!Collide(GetBoundingSphere(ob1), GetBoundingSphere(ob2)))   // shu47 : peut etre faire une fonction dediée pour eviter les [2 x sqrtf] pour les creations de bounding sphere
				continue;		
			if (!Collide(ob1, ob2))
				continue;


			glm::vec2 dp = p2->bounding_box.position - p1->bounding_box.position;



			p1->collision_direction = glm::normalize(-dp);
			p2->collision_direction = glm::normalize(dp);

			p1->collision_reaction_intensity = opp1->car_data.reaction_value;
			p2->collision_reaction_intensity = opp2->car_data.reaction_value;

		//	p1->velocity = p2->velocity = 0.0f;






		

			opp1->SynchronizeData(false);
			opp2->SynchronizeData(false);
		}
	}





	return true;
}













bool LudumRoad::Initialize(death::TMLayerInstance* in_layer_instance, chaos::TiledMap::GeometricObject const* in_geometric_object, death::TMObjectReferenceSolver& reference_solver)
{
	if (!death::TMObject::Initialize(in_layer_instance, in_geometric_object, reference_solver))
		return false;


	lap_count = in_geometric_object->GetPropertyValueInt("LAP_COUNT", lap_count);
	lap_count = std::max(lap_count, 1);

	checkpoint_short_distance = in_geometric_object->GetPropertyValueFloat("CHECKPOINT_SHORT_DISTANCE", checkpoint_short_distance);
	checkpoint_short_distance = std::max(checkpoint_short_distance, 50.0f);

	checkpoint_long_distance = in_geometric_object->GetPropertyValueFloat("CHECKPOINT_LONG_DISTANCE", checkpoint_long_distance);
	checkpoint_long_distance = std::max(checkpoint_long_distance, 50.0f);

	point_speed_factor = in_geometric_object->GetPropertyValueFloat("POINT_SPEED_FACTOR", point_speed_factor);

	player_angular_tweak = in_geometric_object->GetPropertyValueFloat("PLAYER_ANGULAR_TWEAK", player_angular_tweak);
	opponent_angular_tweak = in_geometric_object->GetPropertyValueFloat("OPPONENT_ANGULAR_TWEAK", opponent_angular_tweak);

	player_velocity_tweak = in_geometric_object->GetPropertyValueFloat("PLAYER_VELOCITY_TWEAK", player_velocity_tweak);
	opponent_velocity_tweak = in_geometric_object->GetPropertyValueFloat("OPPONENT_VELOCITY_TWEAK", opponent_velocity_tweak);

	std::vector<glm::vec2> const* src_points = nullptr;

	if (chaos::TiledMap::GeometricObjectPolygon const* pn = in_geometric_object->GetObjectPolygon())
		src_points = &pn->points;
	else if (chaos::TiledMap::GeometricObjectPolyline const * pl = in_geometric_object->GetObjectPolyline())
		src_points = &pl->points;

	if (src_points == nullptr)
		return false;

	glm::vec2 offset = layer_instance->GetLayerOffset();

	for (glm::vec2 const& p : *src_points)
	{
		RoadPoint rp;
		rp.position = p + offset + GetPosition(); // expressed in world system !
		rp.speed_factor = point_speed_factor;
		points.push_back(rp);
	}
	return true;
}

void LudumRoad::OnLevelStarted()
{
	death::TMObject::OnLevelStarted();

	if (layer_instance != nullptr && layer_instance->GetLevelInstance() != nullptr)
	{
		LudumLevelInstance* level_instance = layer_instance->GetLevelInstance();
		if (level_instance != nullptr)
		{
			death::TMLayerInstance* li = level_instance->FindLayerInstance("SpeedIndications");
			if (li != nullptr)
			{
				size_t count = li->GetObjectCount();
				for (size_t i = 0 ; i < count ; ++i)					
				{
					LudumSpeedIndication* indication = auto_cast(li->GetObject(i));
					if (indication != nullptr)
					{
						chaos::box2 indication_box = indication->GetBoundingBox(true);

						for (RoadPoint& p : points)
						{
							if (chaos::Collide(p.position, indication_box))
							{
								p.speed_factor = indication->speed_factor;
							}
						}
					}
				}
			}
		}
	}
}

RoadUpdateValue LudumRoad::UpdateRacePosition(RacePosition& race_position, glm::vec2 const & p, bool player) const
{
	size_t count = points.size();
	for (size_t i = 0 ; i < count ; ++i)
	{
		if (i == race_position.current_road_point) // ignore current point
			continue;

		RoadPoint const& road_pt = points[i];

		float d = (player)? checkpoint_long_distance : checkpoint_short_distance;

		if (glm::length2(p - road_pt.position) < d * d)
		{
			// good point, good direction
			if (i == (size_t)((race_position.current_road_point + 1) % count))
			{
				race_position.current_road_point = ((race_position.current_road_point + 1) % count);

				if (race_position.current_road_point == 0)
				{
					++race_position.current_lap;
					if (race_position.current_lap >= lap_count)
						return RoadUpdateValue::END_OF_RACE;
					return RoadUpdateValue::NEW_LAP;
				}
				return RoadUpdateValue::NEW_CHECKPOINT;
			}
			// wrong direction
			else
			{
				return RoadUpdateValue::WRONG_WAY;
			}
		}
	}
	return RoadUpdateValue::NOP;
}

float LudumRoad::GetSpeedFactor(glm::vec2 const& p) const
{
	size_t count = points.size();
	for (size_t i = 0; i < count; ++i)
	{
		RoadPoint const& road_pt = points[i];

		float d = checkpoint_long_distance;

		if (glm::length2(p - road_pt.position) < d * d)
		{
			return road_pt.speed_factor;
		}
	}

	return 1.0f;
}

// =================================================
// LudumCollision
//=================================================

bool LudumCollision::Initialize(death::TMLayerInstance* in_layer_instance, chaos::TiledMap::GeometricObject const* in_geometric_object, death::TMObjectReferenceSolver& reference_solver)
{
	if (!death::TMObject::Initialize(in_layer_instance, in_geometric_object, reference_solver))
		return false;

	// capture the points
	if (chaos::TiledMap::GeometricObjectPolygon const* pn = in_geometric_object->GetObjectPolygon())
	{
		points = pn->points;
		if (points.size() > 0)
			points.push_back(points[0]);
	}
	else if (chaos::TiledMap::GeometricObjectPolyline const* pl = in_geometric_object->GetObjectPolyline())
	{
		points = pl->points;
	}

	// express points in world coordinates
	for (auto & p : points)
		p = p + in_layer_instance->GetLayerOffset() + GetPosition();


	// compute the bounding box
	if (points.size() > 0)
	{
		internal_bounding_box.position = points[0];
		internal_bounding_box.half_size = { 0, 0 };
		for (auto const& p : points)
			chaos::ExtendBox(internal_bounding_box, p);
	}


	return true;
}

void LudumCollision::OnLevelStarted()
{
	death::TMObject::OnLevelStarted();


}



// =============================================================
// LudumLevel implementation
// =============================================================

LudumLevel::LudumLevel()
{
	level_instance_class = LudumLevelInstance::GetStaticClass();
}

chaos::ParticleLayerBase * LudumLevel::DoCreateParticleLayer(death::TMLayerInstance * layer_instance)
{
	LudumGame * ludum_game = auto_cast(layer_instance->GetGame());

	std::string const & layer_name = layer_instance->GetTiledLayer()->name;

	if (chaos::StringTools::Stricmp(layer_name, "PlayerAndCamera") == 0)
	{
		ParticlePlayerLayerTrait layer_trait;
		layer_trait.game = ludum_game;
		return new chaos::ParticleLayer<ParticlePlayerLayerTrait>(layer_trait);
	}

	if (chaos::StringTools::Stricmp(layer_name, "Opponents") == 0)
	{
		ParticleOpponentLayerTrait layer_trait;
		layer_trait.game = ludum_game;
		return new chaos::ParticleLayer<ParticleOpponentLayerTrait>(layer_trait);
	}




	return death::TMLevel::DoCreateParticleLayer(layer_instance);
}


death::TMObjectFactory LudumLevel::DoGetObjectFactory(death::TMLayerInstance * in_layer_instance, chaos::TiledMap::TypedObject const * in_typed_object)
{
#if 0
	if (in_typed_object->IsObjectOfType("Road"))
		return DEATH_MAKE_OBJECT_FACTORY(return new LudumRoad(););

	if (in_typed_object->IsObjectOfType("SpeedIndication"))
		return DEATH_MAKE_OBJECT_FACTORY(return new LudumSpeedIndication(););
#endif

	return death::TMLevel::DoGetObjectFactory(in_layer_instance, in_typed_object);
}


bool LudumLevel::Initialize(chaos::TiledMap::Map* in_tiled_map)
{
	if (!death::TMLevel::Initialize(in_tiled_map))
		return false;



	return true;
}
