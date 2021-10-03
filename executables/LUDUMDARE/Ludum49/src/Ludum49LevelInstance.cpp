#include <chaos/Chaos.h>

#include "Ludum49Level.h"
#include "Ludum49LevelInstance.h"
#include "Ludum49Game.h"
#include "Ludum49Player.h"
#include "Ludum49GameInstance.h"
#include "Ludum49PlayerDisplacementComponent.h"
#include "Ludum49Particles.h"


bool LandscapeMorph::Tick(Landscape* landscape, float delta_time, std::vector<glm::vec2> & mutable_points)
{
	internal_time += delta_time;
	return DoTick(landscape, delta_time, mutable_points);
}

float LandscapeMorph::GetInternalTime() const
{ 
	return internal_time; 
}

bool LandscapeMorph::DoTick(Landscape* landscape,float delta_time, std::vector<glm::vec2> & mutable_points)
{

	return false;
}

bool LandscapeMorph::Initialize(TMLayerInstance* in_layer_instance, TiledMap::GeometricObject const* in_geometric_object, TMObjectReferenceSolver& reference_solver)
{


	return true;
}


// =================================================================================

bool LandscapeMorphCircle::DoTick(Landscape* landscape,float delta_time, std::vector<glm::vec2> & mutable_points)
{
	size_t count = mutable_points.size();

	for (size_t i = 0; i < count; ++i)
	{
		glm::vec2& v = mutable_points[i];

		float alpha = (float)i * 2.0f * (float)M_PI / (float)count;

		v.x = morph_radius * std::cos(alpha);
		v.y = morph_radius * std::sin(alpha);
	}
	return true;
}

bool LandscapeMorphCircle::Initialize(TMLayerInstance* in_layer_instance, TiledMap::GeometricObject const* in_geometric_object, TMObjectReferenceSolver& reference_solver)
{
	LandscapeMorph::Initialize(in_layer_instance, in_geometric_object, reference_solver);
	morph_radius = in_geometric_object->GetPropertyValueFloat("MORPH_RADIUS", morph_radius);
	return true;
}

// =================================================================================


bool LandscapeMorphScale::DoTick(Landscape* landscape,float delta_time, std::vector<glm::vec2> & mutable_points)
{
	size_t count = mutable_points.size();
#if 0
	for (size_t i = 0; i < count; ++i)
	{
		glm::vec2& v = mutable_points[i];

		float alpha = (float)i * 2.0f * (float)M_PI / (float)count;

		float factor = std::cos(GetInternalTime() * 0.5f);

		v.y = v.y * 1.2f * (1.2f - std::cos(factor));
	}
#endif


	for (size_t i = 0; i < count; ++i)
	{
		glm::vec2& v = mutable_points[i];

		float alpha = (float)i * 2.0f * (float)M_PI / (float)count;

		float scale_y = (std::sin(alpha) >= 0.0f) ? 1.0f : -1.0f;

		v.x = morph_radius * std::cos(alpha);
		v.y = 0.5f * morph_radius * scale_y;
	}









	return true;
}

bool LandscapeMorphScale::Initialize(TMLayerInstance* in_layer_instance, TiledMap::GeometricObject const* in_geometric_object, TMObjectReferenceSolver& reference_solver)
{
	LandscapeMorph::Initialize(in_layer_instance, in_geometric_object, reference_solver);
	morph_radius = in_geometric_object->GetPropertyValueFloat("MORPH_RADIUS", morph_radius);
	return true;
}


// =================================================================================


bool LandscapeMorphMorph::DoTick(Landscape* landscape,float delta_time, std::vector<glm::vec2> & mutable_points)
{
	size_t count = mutable_points.size();

	for (size_t i = 0; i < count; ++i)
	{
		glm::vec2& v = mutable_points[i];

		float factor = std::cos(GetInternalTime() * morph_speed);
		v = (1.0f - factor) * v + landscape->points[i] * factor;
	}
	return true;
}

bool LandscapeMorphMorph::Initialize(TMLayerInstance* in_layer_instance, TiledMap::GeometricObject const* in_geometric_object, TMObjectReferenceSolver& reference_solver)
{
	LandscapeMorph::Initialize(in_layer_instance, in_geometric_object, reference_solver);
	morph_speed = in_geometric_object->GetPropertyValueFloat("MORPH_SPEED", morph_speed);
	return true;
}




// =======================================================

box2 BoxFromPoints(std::vector<glm::vec2> const& v)
{
	glm::vec2 min_position = v[0];
	glm::vec2 max_position = v[0];
	for (glm::vec2 const& p : v)
	{
		min_position = glm::min(min_position, p);
		max_position = glm::max(max_position, p);
	}		
	return std::make_pair(min_position, max_position);
}

Landscape::Landscape()
{

}

bool Landscape::DoTick(float delta_time)
{
	TMObject::DoTick(delta_time);

	if (morphs.size() > 0)
	{
		bool changed = false;

		std::vector<glm::vec2> mutable_points = points;
		for (auto& morph : morphs)
			changed |= morph->Tick(this, delta_time, mutable_points);
		if (changed)
		{
			point_bounding_box = BoxFromPoints(mutable_points);
			BuildMesh(mutable_points);
		}
	}
	return true;
}

int Landscape::DoDisplay(GPURenderer* renderer, GPUProgramProviderBase const* uniform_provider, GPURenderParams const& render_params)
{
	int result = 0;

	chaos::GPUProgramProviderChain main_provider(uniform_provider);

	glm::mat4 local_to_world = glm::translate(glm::vec3(GetBoundingBox(true).position, 0.0f));
	main_provider.AddVariable("local_to_world", local_to_world);

	
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glDisable(GL_DEPTH_TEST);
	glDisable(GL_CULL_FACE);
	glPointSize(5.0f);

	if (mesh != nullptr)
		result += mesh->Display(renderer, &main_provider, render_params);

	static bool DEBUG_DISPLAY = true;
	if (DEBUG_DISPLAY)
	{
		// shu49. ca pourrait etre partique d avoir une fonction d affichage de bounding box

		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
		GPUDrawInterface<VertexDefault> DI(DefaultParticleProgram::GetMaterial(), 4);
		QuadPrimitive<VertexDefault> quad = DI.AddQuads(1);

		glm::vec2 box_v[4];
		GetBoxVertices(point_bounding_box, box_v, true); // shu49 false -> want the box in local (shader applies the transforms) ... but in that case, layer transform (that here is null) is ignored
		quad[0].position = box_v[0];
		quad[1].position = box_v[1];
		quad[2].position = box_v[3];
		quad[3].position = box_v[2];
		
		PointPrimitive<VertexDefault> smooth_prim = DI.AddPoints(smoothed_points.size());
		for (auto& p : smoothed_points)
		{
			smooth_prim[0].position = p;
			smooth_prim[0].color = { 0.0f,1.0f,0.0f,1.0f };
			++smooth_prim;
		}

		PointPrimitive<VertexDefault> prim_p = DI.AddPoints(points.size());
		for (auto& p : points)
		{
			prim_p[0].position = p;
			prim_p[0].color = { 0.0f,0.0f,1.0f,1.0f };
			++prim_p;
		}

		// shu49 attention. il ne faudrait pas faire un shared_ptr<> p = GetDynamicMesh();
		//
		// template<typename T> class ForbidSmartReference : ...
		//
		DI.Flush();

		debug_mesh = DI.ExtractMesh();
		debug_mesh->Display(renderer, &main_provider, render_params);
		//DI.GetDynamicMesh().Display(renderer, &main_provider, render_params); // shu49 problematique de detruire l interface a la fin de la fonction
	}

	glPolygonMode(GL_FRONT_AND_BACK,  GL_FILL);
	glDisable(GL_BLEND);
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);

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


#if 0
	ori_bounding_box = bounding_box;
	bounding_box = std::make_pair(min_position, max_position);


	for (glm::vec2& p : points)
		p -= (bounding_box.position - ori_bounding_box.position);
	for (glm::vec2& p : smoothed_points)
		p -= (bounding_box.position - ori_bounding_box.position);

	ori_bounding_box = bounding_box;

#endif

void Landscape::BuildMesh(std::vector<glm::vec2> const & src)
{
	smoothed_points = SmoothBoundary(src, smooth_count, smooth_factor);

	std::vector<glm::vec2> v = smoothed_points;

	// orientation of the polygon
	float accum = 0.0f;

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


	std::vector<triangle2> triangles;
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
				triangles.push_back({ a, b, c });
				v.erase(v.begin() + index);
			}
		}

		if (!new_triangle)
		{
			assert(0);
		}
	}

	GPURenderMaterial * RM = GetLayerInstance()->GetRenderMaterial();

	GPUDrawInterface<VertexDefault> DI(RM, 3 * triangles.size());

	for (auto const& t : triangles)
	{
		auto tri = DI.AddTriangles(1);

		for (int i = 0; i < 3; ++i)
		{
			tri[i].position = t[i];
			tri[i].color = color;
		}
	}
	mesh = DI.ExtractMesh();
}

box2 Landscape::GetBoundingBox(bool world_system) const
{
	box2 result = point_bounding_box;


	result.position += bounding_box.position;

	if (world_system)
		result.position += layer_instance->GetLayerOffset();
	return result;

}

bool Landscape::Initialize(TMLayerInstance* in_layer_instance, TiledMap::GeometricObject const* in_geometric_object, TMObjectReferenceSolver& reference_solver)
{
	if (!TMObject::Initialize(in_layer_instance, in_geometric_object, reference_solver))
		return false;

	smooth_count = in_geometric_object->GetPropertyValueInt("SMOOTH_COUNT", smooth_count);
	smooth_count = std::clamp(smooth_count, 0, 5);

	smooth_factor = in_geometric_object->GetPropertyValueFloat("SMOOTH_FACTOR", smooth_factor);
	smooth_factor = std::clamp(smooth_factor, 0.0f, 1.0f);

	if (std::string const* names = in_geometric_object->FindPropertyString("MORPH_CLASSES"))
	{
		std::vector<SubClassOf<LandscapeMorph>> morph_classes = GetSubClassesFromString<LandscapeMorph>(names->c_str(), ',');
		for (auto cls : morph_classes)
		{
			if (LandscapeMorph* morph = cls.CreateInstance())
			{
				morph->Initialize(in_layer_instance, in_geometric_object, reference_solver);
				morphs.push_back(morph);
			}
		}
	}


	color = in_geometric_object->GetPropertyValueColor("COLOR", color);





	ori_bounding_box = bounding_box;

	// capture the points
	if (TiledMap::GeometricObjectPolygon const* pn = auto_cast(in_geometric_object))
	{
		points = pn->points;
		point_bounding_box = BoxFromPoints(points);
		BuildMesh(points);
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
