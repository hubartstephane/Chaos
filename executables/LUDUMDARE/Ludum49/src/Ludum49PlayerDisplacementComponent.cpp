#include <chaos/Chaos.h>

#include "Ludum49PlayerDisplacementComponent.h"
#include "Ludum49Particles.h"
#include "Ludum49Player.h"
#include "Ludum49LevelInstance.h"

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



#if 0
		if (pawn_velocity.x > 0.0f)
			pawn_velocity.x = std::max(0.0f, pawn_velocity.x - ludum_player->slow_down * delta_time);
		else
			pawn_velocity.x = std::min(0.0f, pawn_velocity.x + ludum_player->slow_down * delta_time);
#endif
		




bool LudumPlayerDisplacementComponent::DoTick(float delta_time)
{
	PlayerDisplacementComponent::DoTick(delta_time);

	// early exit
	PlayerPawn* pawn = player->GetPawn();
	if (pawn == nullptr)
		return true;

	LudumPlayer* ludum_player = GetPlayer();

	box2 pawn_box = pawn->GetBoundingBox();

	GPUDrawInterface<VertexDefault> * DI = GetDebugDrawInterface();
	
#if 0
		std::pair<glm::vec2, glm::vec2> corners = GetBoxCorners(pawn_box);

		auto quad = DI->AddQuads();
		for (auto& v : quad)
		{			
			v.color = { 1.0f, 0.0f, 0.0f, 1.0f };
			v.texcoord = { -1.0f, -1.0f, -1.0f };
			v.flags = 0;
		}

		quad[0].position = { corners.first.x, corners.first.y };
		quad[1].position = { corners.second.x, corners.first.y };
		quad[2].position = { corners.second.x, corners.second.y };
		quad[3].position = { corners.first.x, corners.second.y };
		
#endif







//	pawn_velocity.y += ludum_player->gravity * delta_time;

	glm::vec2 direction = player->GetLeftStickPosition(false);
	direction.x = MathTools::AnalogicToDiscret(direction.x);
	direction.y = MathTools::AnalogicToDiscret(direction.y);
	
#if 0
	if (direction.x == 0.0f || direction.x * pawn_velocity.x < 0.0f)
	{
		pawn_velocity.x *= std::pow(ludum_player->slow_down_factor, delta_time);
		if (std::abs(pawn_velocity.x) < ludum_player->stop_velocity && pawn_velocity.x != 0.0f)
			pawn_velocity.x = 0.0f;
	}

	float acceleration_factor = (direction.x * pawn_velocity.x < 1.0f) ? 2.0f : 1.0f;
	pawn_velocity.x += acceleration_factor * direction.x * ludum_player->acceleration * delta_time;
	

	if (direction.y == 0.0f || direction.y * pawn_velocity.y < 0.0f)
	{
		pawn_velocity.y *= std::pow(ludum_player->slow_down_factor, delta_time);
		if (std::abs(pawn_velocity.y) < ludum_player->stop_velocity && pawn_velocity.y != 0.0f)
			pawn_velocity.y = 0.0f;
	}

	float acceleration_factory = (direction.y * pawn_velocity.y < 1.0f) ? 2.0f : 1.0f;
	pawn_velocity.y += acceleration_factor * direction.y * ludum_player->acceleration * delta_time;

#endif

	pawn_box.position += 0.3f * delta_time * ludum_player->acceleration * direction;








	Key const left_buttons[] = { KeyboardButton::LEFT, Key() };
	bool run_pressed = player->CheckButtonPressed(left_buttons);


	

	//pawn_box.position += pawn_velocity * delta_time;


	LudumLevelInstance* ludum_level = GetLevelInstance();

	float  min_distance2 = std::numeric_limits<float>::max();
	size_t min_distance_index = 0;

	sphere2 pawn_sphere = GetInnerSphere(pawn_box);

	TMObjectCollisionIterator Iterator =ludum_level->GetObjectCollisionIterator(pawn_box, COLLISION_LANDSCAPE, false);
	while (Iterator)
	{
		
		TMObject & object = *Iterator;

		if (Landscape* landscape = auto_cast(&object))
		{
			glm::vec2 offset = landscape->GetBoundingBox(true).position;

			size_t count = landscape->points.size();
			for (size_t i = 0; i < count; ++i)
			{
				glm::vec2 const& a = landscape->points[i] + offset;
				glm::vec2 const& b = landscape->points[(i + 1) % count] + offset;

				
				glm::vec2 proj = ClosestPoint(pawn_sphere.position, a, b);

					PointPrimitive<VertexDefault> P = DI->AddPoints(1);
					P[0].position = proj;
					P[0].color = { 1.0f, 1.0f, 1.0f, 1.0f };
					P[0].flags = 0;
					P[0].texcoord = { -1.0f, -1.0f, -1.0f };

				float l2 = glm::length2(proj - pawn_sphere.position);
				if (min_distance2 > l2)
				{




					min_distance2 = l2;
					min_distance_index = i;
				}
			}

			if (min_distance2 <= pawn_sphere.radius * pawn_sphere.radius)



				landscape = landscape;
		}



		++Iterator;
	}









	pawn->SetPosition(pawn_box.position);


	return true;
}
