#include <chaos/Chaos.h>

#include "Ludum49PlayerDisplacementComponent.h"
#include "Ludum49Particles.h"
#include "Ludum49Player.h"
#include "Ludum49LevelInstance.h"

#if _DEBUG
CHAOS_HELP_TEXT(CMD, "-DebugDisplay");
#endif

//
//       a                    b
//       +---------x----------+
//                 |
//                 |
//                 x p
//
//       a                    b
//       +--------------------+
//      /
//     /
//    x p

glm::vec2 Project(glm::vec2 const& p, glm::vec2 const& a, glm::vec2 const& b)
{
	glm::vec2 ap  = (p - a);
	if (ap == glm::vec2(0.0f, 0.0f))
		return p;

	glm::vec2 ab  = (b - a);
	glm::vec2 dir = glm::normalize(ab);

	return a + dir * glm::dot(ap, dir);
}

glm::vec2 ClosestPoint(glm::vec2 const& p, glm::vec2 const& a, glm::vec2 const& b)
{
	glm::vec2 proj = Project(p, a, b);

	if (dot(proj - a, b - a) < 0.0f)
		return a;
	if (dot(proj - b, a - b) < 0.0f)
		return b;
	return proj;
}

struct CollisionEntry
{
	glm::vec2 a;
	glm::vec2 b;
	glm::vec2 proj;
	float l2;
	Landscape* landscape = nullptr;
};

std::vector<CollisionEntry> ComputeCollisions(box2 const box, LudumLevelInstance* ludum_level)
{
	GPUDrawInterface<VertexDefault> * DI = GetDebugDrawInterface();

	std::vector<CollisionEntry> result;

	sphere2 pawn_sphere = GetInnerSphere(box);

	TMObjectCollisionIterator Iterator = ludum_level->GetObjectCollisionIterator(box, COLLISION_LANDSCAPE, false);
	while (Iterator)
	{
		TMObject & object = *Iterator;

		if (Landscape* landscape = auto_cast(&object))
		{
			glm::vec2 offset = landscape->GetBoundingBox(true).position;

			std::vector<glm::vec2> & v = landscape->smoothed_points;

			size_t count = v.size();
			for (size_t i = 0; i < count; ++i)
			{
				glm::vec2 const& a = v[i] + offset;
				glm::vec2 const& b = v[(i + 1) % count] + offset;

				glm::vec2 proj = ClosestPoint(pawn_sphere.position, a, b);

				float l2 = glm::length2(proj - pawn_sphere.position);
				if (l2 <= pawn_sphere.radius * pawn_sphere.radius)
				{
					result.push_back({ a, b, proj, l2 , landscape});
				}
			}
		}
		++Iterator;
	}

	if (result.size() > 0)
	{
		float min_l2 = std::numeric_limits<float>::max();

		CollisionEntry nearest_collision;
		for (auto const& entry : result)
		{
			min_l2 = std::min(min_l2, entry.l2);
			nearest_collision = entry;
		}

		std::sort(result.begin(), result.end(), [](CollisionEntry const& src1, CollisionEntry const& src2)
		{
			return src1.l2 < src2.l2;
		});
	}

	return result;
}



bool LudumPlayerDisplacementComponent::DoTick(float delta_time)
{
	touching_ground_timer = std::max(0.0f, touching_ground_timer - delta_time);

	GPUDrawInterface<VertexDefault> * DI = GetDebugDrawInterface();

	PlayerDisplacementComponent::DoTick(delta_time);

	// early exit
	PlayerPawn* pawn = player->GetPawn();
	if (pawn == nullptr)
		return true;

	LudumPlayer* ludum_player = GetPlayer();

	box2 pawn_box = pawn->GetBoundingBox();
	box2 pawn_box_before = pawn_box;

	LudumLevelInstance* ludum_level = GetLevelInstance();

	glm::vec2 direction = player->GetLeftStickPosition(false);
	direction.x = MathTools::AnalogicToDiscret(direction.x);
	direction.y = MathTools::AnalogicToDiscret(direction.y);


#if 0 // DEBUG_DISPLACEMENT

	pawn_box.position += 0.3f * delta_time * ludum_player->acceleration * direction;

#else	

	if (direction.x == 0.0f || direction.x * pawn_velocity.x < 0.0f)
	{
		pawn_velocity.x *= std::pow(ludum_player->slow_down_factor, delta_time);
		//if (std::abs(pawn_velocity.x) < ludum_player->stop_velocity && pawn_velocity.x != 0.0f)
		//	pawn_velocity.x = 0.0f;
	}

	if (touching_ground_timer > 0.0f)
	{
		float acceleration_factor = (direction.x * pawn_velocity.x < 1.0f) ? 2.0f : 1.0f;
		pawn_velocity.x += acceleration_factor * direction.x * ludum_player->acceleration * delta_time;
	}

	pawn_velocity.y += ludum_player->gravity * delta_time;

	pawn_box.position += pawn_velocity * delta_time;

#endif

	sphere2 pawn_sphere = GetInnerSphere(pawn_box);

	std::vector<CollisionEntry> collisions = ComputeCollisions(pawn_box, ludum_level);

	if (collisions.size() > 0)
	{
		CollisionEntry const col = collisions[0];
		


		// GOOD




		// TEST
		glm::vec2 m = (col.a + col.b) * 0.5f;

		glm::vec3 n = { col.b - col.a, 0.0f };
		glm::vec3 Z = glm::vec3(0.0f, 0.0f, -1.0f) *col.landscape->polygon_orientation;
		glm::vec3 N = glm::normalize(glm::cross(n, Z));

	//	if (glm::dot(pawn_box.position - m, glm::vec2(N)) < 0.0f)
	//		N = -N;

		float fff = glm::dot(pawn_box.position - m, glm::vec2(N));
		if (glm::dot(pawn_box.position - m, glm::vec2(N)) < 0.0f)
		{
			N = N;
		}
		else
		{
			N = N;
		}
			

		pawn_box.position += glm::normalize(pawn_box.position - col.proj) * (pawn_sphere.radius - std::sqrt(col.l2));

#if _DEBUG
		if (chaos::Application::HasApplicationCommandLineFlag("-DebugDisplay")) // CMDLINE
		{
			LinePrimitive<VertexDefault> line = DI->AddLines(2);
			line[0].position = col.a;
			line[1].position = col.b;
			line[2].position = m;
			line[3].position = m + glm::vec2(N) * 100.0f;
		}
#endif



		touching_ground_timer = ludum_player->touching_ground_lapse_time;
	}

	pawn->SetPosition(pawn_box.position);

	if (delta_time != 0.0f)
		pawn_velocity = (pawn_box.position - pawn_box_before.position) / delta_time;

	float len2 = glm::length2(pawn_velocity);
	if (len2 > ludum_player->max_velocity * ludum_player->max_velocity)
		pawn_velocity = ludum_player->max_velocity * glm::normalize(pawn_velocity);

	return true;
}
