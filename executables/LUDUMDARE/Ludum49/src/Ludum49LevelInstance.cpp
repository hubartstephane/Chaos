#include <chaos/Chaos.h>

#include "Ludum49Level.h"
#include "Ludum49LevelInstance.h"
#include "Ludum49Game.h"
#include "Ludum49Player.h"
#include "Ludum49GameInstance.h"
#include "Ludum49PlayerDisplacementComponent.h"
#include "Ludum49Particles.h"


Landscape::Landscape()
{

}

bool Landscape::DoTick(float delta_time)
{
	TMObject::DoTick(delta_time);

	if (0 && id == 2233)
	{
		internal_t += delta_time * 0.5f;

		float c = std::cos(internal_t);
		float s = std::sin(internal_t);

		bounding_box.position = ori_bounding_box.position + glm::vec2(0.0f, 1.0f) * GLMTools::Rotate(glm::vec2(100.0f), c, s);


	}

	//bounding_box.position


	return true;
}

int Landscape::DoDisplay(GPURenderer* renderer, GPUProgramProviderBase const* uniform_provider, GPURenderParams const& render_params)
{
	int result = 0;

	chaos::GPUProgramProviderChain main_provider(uniform_provider);

	glm::mat4 local_to_world = glm::translate(glm::vec3(GetBoundingBox(true).position, 0.0f));
	main_provider.AddVariable("local_to_world", local_to_world);

	//glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
			glEnable(GL_BLEND);
			glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
			glDisable(GL_DEPTH_TEST);
			glDisable(GL_CULL_FACE);

	//uniform_provider.



	if (mesh != nullptr)
		result += mesh->Display(renderer, &main_provider, render_params);


			glDisable(GL_BLEND);
			glEnable(GL_DEPTH_TEST);
			glEnable(GL_CULL_FACE);
		//	glPolygonMode(GL_FRONT_AND_BACK,  GL_FILL);








	return result;
}



// shu49

float sign (glm::vec2 const &p1, glm::vec2 const &p2, glm::vec2 const &p3)
{
    return (p1.x - p3.x) * (p2.y - p3.y) - (p2.x - p3.x) * (p1.y - p3.y);
}

bool PointInTriangle (glm::vec2 const & pt, glm::vec2 const & v1, glm::vec2 const & v2, glm::vec2 const & v3)
{
    float d1, d2, d3;
    bool has_neg, has_pos;

    d1 = sign(pt, v1, v2);
    d2 = sign(pt, v2, v3);
    d3 = sign(pt, v3, v1);

    has_neg = (d1 < 0) || (d2 < 0) || (d3 < 0);
    has_pos = (d1 > 0) || (d2 > 0) || (d3 > 0);

    return !(has_neg && has_pos);
}


float GetAngle(glm::vec2 const & a, glm::vec2 const & b, glm::vec2 const & c)
{
	glm::vec2 ab = b - a;
	glm::vec2 cb = b - c;

	float dot = (ab.x * cb.x + ab.y * cb.y); // dot product
	float cross = (ab.x * cb.y - ab.y * cb.x); // cross product

	return std::atan2(cross, dot);
}

std::vector<glm::vec2> SmoothBoundary(std::vector<glm::vec2> const & src, size_t loop_count, float smooth_factor)
{
	if (loop_count == 0 || src.size() == 0)
		return src;

	std::vector<glm::vec2> result;
	result.reserve(src.size() * 2);

	size_t count = src.size();
	for (size_t i = 0; i < src.size() ; ++i)
	{		
		size_t prev_index = (i - 1 + count) % count;
		size_t next_index = (i + 1) % count;

		glm::vec2 const & a = src[i];
		glm::vec2 const & prev = src[prev_index];
		glm::vec2 const & next = src[next_index];

		assert(a != prev && prev != next && next != a);

		glm::vec2 extra_next = a + (next - a) * smooth_factor;
		glm::vec2 extra_prev = a + (prev - a) * smooth_factor;

		result.push_back(extra_prev);
		result.push_back(extra_next);

	}
	return SmoothBoundary(result, loop_count - 1, smooth_factor);
}

bool Landscape::Initialize(TMLayerInstance* in_layer_instance, TiledMap::GeometricObject const* in_geometric_object, TMObjectReferenceSolver& reference_solver)
{
	if (!TMObject::Initialize(in_layer_instance, in_geometric_object, reference_solver))
		return false;


	smooth_count = in_geometric_object->GetPropertyValueInt("SMOOTH_COUNT", smooth_count);
	smooth_count = std::clamp(smooth_count, 0, 5);

	smooth_factor = in_geometric_object->GetPropertyValueFloat("SMOOTH_FACTOR", smooth_factor);
	smooth_factor = std::clamp(smooth_factor, 0.0f, 1.0f);
	
	ori_bounding_box = bounding_box;








	// capture the points
	if (TiledMap::GeometricObjectPolygon const* pn = auto_cast(in_geometric_object))
	{
		points = SmoothBoundary(pn->points, smooth_count, smooth_factor);

		std::vector<triangle2> triangles;

		std::vector<glm::vec2> v = points;


		float accum = 0.0f;


		// orientation of the polygon


		size_t count = v.size();
		for (size_t i = 0; i < count; ++i)
		{
			glm::vec2 const& a = v[i];
			glm::vec2 const& b = v[(i + 1) % count];
			glm::vec2 const& c = v[(i + 2) % count];

			if (a == b || b == c || c == a)
				continue;

			float abc = GetAngle(a, b, c);

			// the angle that interest us is not the abc, but the signed angle with the along direction

			//         | angle
			//  -PI    |       C   +PI
			//           --  /
			//         |   /
			//           /
			//       B + 
			//         |
			//         |
			//         |
			//         A
			//

			float angle = (abc > 0.0f) ? (float)M_PI - abc : -(float)M_PI - abc;
			accum += angle;
		}

		box2 bbox = GetBoundingBox(true);

		//glm::vec2 offset = bbox.position;
		glm::vec2 offset = bbox.position;


		offset = offset * 0.0f;
		
		while (v.size() > 2)
		{
			bool new_triangle = false;
			for (size_t i = v.size(); i > 0; --i) // to remove at the end of vector more frequently
			{
				size_t count = v.size();
				size_t index = i - 1;
				size_t prev = (index - 1 + count) % count;
				size_t next = (index + 1 + count) % count;

				glm::vec2 const& a = v[index];
				glm::vec2 const& b = v[prev];
				glm::vec2 const& c = v[next];

				if (a == b || b == c || c == a)
					continue;

				if (GetAngle(b, a, c) * accum < 0.0f) // angle in opposite direction of the polygon orientation
					continue;

				size_t j = 0;
				for (; j < count - 3; ++j)
				{
					size_t test_index = (next + 1 + j) % count;

					if (PointInTriangle(v[test_index], a, b, c))
						break;
				}

				// no other point inside the triangle found (can remove the 'index')
				if (j == count - 3)
				{
					new_triangle = true;
					triangles.push_back({ a + offset, b + offset, c + offset });
					v.erase(v.begin() + index);
				}
			}

			if (!new_triangle)
			{
				assert(0);
			}
		}

		GPURenderMaterial * RM = in_layer_instance->GetRenderMaterial();

		GPUDrawInterface<VertexDefault> DI(RM, 3 * triangles.size());

		for (auto const& t : triangles)
		{
			auto tri = DI.AddTriangles(1);

			for (int i = 0; i < 3; ++i)
			{
				tri[i].position = t[i] - offset;
				tri[i].color = { 1.0f, 0.0f, 0.0f, 0.8f };
			}
		}
#if 0
		std::pair<glm::vec2, glm::vec2> corners = GetBoxCorners(bbox);

		auto quad = DI.AddQuads();
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


		mesh = DI.ExtractMesh();
	}


	return true;
}


// =============================================================
// LudumLevelInstance implementation
// =============================================================

LudumLevelInstance::LudumLevelInstance()
{
	player_displacement_component_class = LudumPlayerDisplacementComponent::GetStaticClass();
}

void LudumLevelInstance::CreateCameraComponents(Camera* camera, TMCameraTemplate* camera_template)
{
	TMLevelInstance::CreateCameraComponents(camera, camera_template);

	camera->SetSafeZone(glm::vec2(0.0f, 0.0f));
	camera->AddComponent(new FollowPlayerCameraComponent(0));
	camera->AddComponent(new ShakeCameraComponent(0.15f, 0.05f, 0.15f, true, true));
	camera->AddComponent(new SoundListenerCameraComponent());
}

bool LudumLevelInstance::DoTick(float delta_time)
{
	TMLevelInstance::DoTick(delta_time);


	

	return true;
}

bool LudumLevelInstance::Initialize(Game * in_game, Level * in_level)
{
	if (!TMLevelInstance::Initialize(in_game, in_level))
		return false;



	return true;
}


bool LudumLevelInstance::IsPlayerDead(Player* player)
{
	if (TMLevelInstance::IsPlayerDead(player))
		return true;

	return false;
}


bool LudumLevelInstance::CheckLevelCompletion() const
{
	LudumLevel const * ludum_level = GetLevel();

	LudumPlayer const * ludum_player = GetPlayer(0);
	if (ludum_player != nullptr && ludum_level != nullptr)
	{
	
	}
	return false;
}

bool LudumLevelInstance::CanCompleteLevel() const
{
	if (completion_timer == 0.0f)
		return true;
	return false;
}

int LudumLevelInstance::DoDisplay(GPURenderer* renderer, GPUProgramProviderBase const* uniform_provider, GPURenderParams const& render_params)
{



	return TMLevelInstance::DoDisplay(renderer, uniform_provider, render_params);
}

uint64_t LudumLevelInstance::GetCollisionFlagByName(char const* name) const
{
	assert(name != nullptr);

	if (StringTools::Stricmp(name, "Landscape") == 0)
		return COLLISION_LANDSCAPE;

	return TMLevelInstance::GetCollisionFlagByName(name);
}
