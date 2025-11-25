#include "Ludum49PCH.h"
#include "Ludum49PlayerDisplacementComponent.h"
#include "Ludum49Particles.h"
#include "Ludum49Player.h"
#include "Ludum49LevelInstance.h"

namespace GlobalVariables
{
#if _DEBUG
	CHAOS_GLOBAL_VARIABLE(bool, DebugDisplay);
#endif
};

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



std::vector<CollisionEntry> ComputeCollisions(box2 b, LudumLevelInstance* ludum_level)
{
#if _DEBUG
	GPUDrawInterface<VertexDefault> * DI = GetDebugDrawInterface();
#endif

	std::vector<CollisionEntry> result;

	sphere2 pawn_sphere = GetInnerSphere(b);


	// The CollisionIterator does not correctly detect
	//   pawn / landscape collision
	//   it does it to late
	//   at this point, the sphere already interpenetrate the landscape
	//   but by too much.
	//
	//   -> one of the 2 box are wrongs
	//   -> increase pawn bbox to increase early detection
	//

	b.half_size *= 4.0f;

	TMObjectCollisionIterator Iterator = ludum_level->GetObjectCollisionIterator(b, COLLISION_LANDSCAPE, false);
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
		std::sort(result.begin(), result.end(), [](CollisionEntry const& src1, CollisionEntry const& src2)
		{
			return src1.l2 < src2.l2;
		});
	}

	return result;
}





CollisionEntry LudumPlayerDisplacementComponent::ProcessCollision(box2 & pawn_box)
{
	LudumPlayer* ludum_player = GetPlayer();

#if _DEBUG
	GPUDrawInterface<VertexDefault> * DI = GetDebugDrawInterface();
#endif

	LudumLevelInstance* ludum_level = GetLevelInstance();

	sphere2 pawn_sphere = GetInnerSphere(pawn_box);

	std::vector<CollisionEntry> collisions = ComputeCollisions(pawn_box, ludum_level);

	if (collisions.size() > 0)
	{
		CollisionEntry const& col = collisions[0];

		glm::vec2 m = (col.a + col.b) * 0.5f;
		glm::vec3 n = { col.b - col.a, 0.0f };

		//glm::vec3 Z = glm::vec3(0.0f, 0.0f, -1.0f) *col.landscape->polygon_orientation;
		// polygon_orientation is not a good idea due to scale -1

		glm::vec3 Z = glm::vec3(0.0f, 0.0f, -1.0f);
		glm::vec3 N = glm::normalize(glm::cross(n, Z));

		if (glm::dot(pawn_box.position - m, glm::vec2(N)) < 0.0f)
			N = -N;

		glm::vec2 direction = glm::normalize(pawn_box.position - col.proj);
		pawn_box.position = col.proj + direction * pawn_sphere.radius;

#if _DEBUG
		if (GlobalVariables::DebugDisplay.Get())
		{
			LinePrimitive<VertexDefault> line = DI->AddLines(2);
			line[0].position = col.a;
			line[1].position = col.b;
			line[2].position = col.proj;
			line[3].position = col.proj + direction * 100.0f;
		}
#endif

		touching_ground_timer = ludum_player->touching_ground_lapse_time;

		return collisions[0];
	}
	return {};
}

bool LudumPlayerDisplacementComponent::DoTick(float delta_time)
{
	touching_ground_timer = std::max(0.0f, touching_ground_timer - delta_time);

#if _DEBUG
	GPUDrawInterface<VertexDefault> * DI = GetDebugDrawInterface();
#endif

	PlayerDisplacementComponent::DoTick(delta_time);

	// early exit
	PlayerPawn* pawn = player->GetPawn();
	if (pawn == nullptr)
		return true;

	LudumPlayer* ludum_player = GetPlayer();


	box2 pawn_box = pawn->GetBoundingBox();


	box2 pawn_box_before = pawn_box;

	// - - - - - - - - - -


	glm::vec2 direction = player->GetLeftStickPosition(false);
	direction.x = MathTools::AnalogicToDiscret(direction.x);
	direction.y = MathTools::AnalogicToDiscret(direction.y);


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

	// - - - - - - - - - - - -

	CollisionEntry col = ProcessCollision(pawn_box);

	// - - - - - - - - - - - -


	pawn->SetPosition(pawn_box.position);

	if (delta_time != 0.0f)
		pawn_velocity = (pawn_box.position - pawn_box_before.position) / delta_time;

	float len2 = glm::length2(pawn_velocity);
	if (len2 > ludum_player->max_velocity * ludum_player->max_velocity)
		pawn_velocity = ludum_player->max_velocity * glm::normalize(pawn_velocity);

	return true;
}
