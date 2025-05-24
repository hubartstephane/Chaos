#include "Ludum49PCH.h"
#include "Ludum49Level.h"
#include "Ludum49LevelInstance.h"
#include "Ludum49Game.h"
#include "Ludum49Player.h"
#include "Ludum49GameInstance.h"
#include "Ludum49PlayerDisplacementComponent.h"
#include "Ludum49Particles.h"

#define MORPH_PARAM(prop_name) StringTools::Printf("%s:%s", name, prop_name).c_str()


float GetAngle(glm::vec2 const & a, glm::vec2 const & b, glm::vec2 const & c)
{
	glm::vec2 ab = b - a;
	glm::vec2 cb = b - c;

	float dot = (ab.x * cb.x + ab.y * cb.y); // dot product
	float cross = (ab.x * cb.y - ab.y * cb.x); // cross product

	return std::atan2(cross, dot);
}

float GetPolygonOrientation(std::vector<glm::vec2> const& v)
{
	float result = 0.0f;

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
		result += angle;
	}
	return result;
}

void ReadDataMap(float& value, MORPH_DATA_MAP data_map, char const* name)
{
	for (auto const& entry : data_map)
	{
		if (StringTools::Stricmp(entry.first, name) == 0)
		{
			value = entry.second;
			return;
		}
	}
}

void ReadDataMap(int& value, MORPH_DATA_MAP data_map, char const* name)
{
	float tmp = float(value);
	ReadDataMap(tmp, data_map, name);
	value = (int)tmp;
}

//         +---- 1
//        /|
//       / |
// 0 ---+  |
//      |  |
//      a  b

float linearstep(float t, float a, float b)
{
	if (a == b)
		return (t < a) ? 0.0f : 1.0f;
	return std::clamp((t - a) / (b - a), 0.0f, 1.0f);
}


bool LPMorph::Tick(Landscape* landscape, float delta_time)
{
	internal_time += delta_time;
	return DoTick(landscape, delta_time);
}

bool LPMorph::GetPoints(Landscape* landscape, std::vector<glm::vec2>& mutable_points)
{
	return true;
}

float LPMorph::GetStrength(Landscape* landscape)
{
	return 1.0f;
}


float LPMorph::GetInternalTime() const
{
	return internal_time;
}

bool LPMorph::DoTick(Landscape* landscape,float delta_time)
{
	return false;
}

bool LPMorph::Initialize(MORPH_DATA_MAP const & data_map, TMObjectReferenceSolver& reference_solver)
{
	return true;
}

bool LPMorph::ComposeTree(MORPH_TREE_IT& it, MORPH_TREE_IT end)
{
	return true;
}

// =================================================================================

bool LPMorph_Unary::ComposeTree(MORPH_TREE_IT& it, MORPH_TREE_IT end)
{
	if (!LPMorph::ComposeTree(it, end))
		return false;
	if (it == end)
		return false;
	arg1 = *it;
	if (!arg1->ComposeTree(++it, end))
		return false;
	return true;
}

bool LPMorph_Binary::ComposeTree(MORPH_TREE_IT& it, MORPH_TREE_IT end)
{
	if (!LPMorph_Unary::ComposeTree(it, end))
		return false;
	if (it == end)
		return false;
	arg2 = *it;
	if (!arg2->ComposeTree(++it, end))
		return false;
	return true;
}

bool LPMorph_Ternary::ComposeTree(MORPH_TREE_IT& it, MORPH_TREE_IT end)
{
	if (!LPMorph_Binary::ComposeTree(it, end))
		return false;
	if (it == end)
		return false;
	arg3 = *it;
	if (!arg3->ComposeTree(++it, end))
		return false;
	return true;
}



bool LPMorph_Unary::DoTick(Landscape* landscape, float delta_time)
{
	arg1->Tick(landscape, delta_time);
	return true;
}

bool LPMorph_Binary::DoTick(Landscape* landscape, float delta_time)
{
	arg1->Tick(landscape, delta_time);
	arg2->Tick(landscape, delta_time);
	return true;
}

bool LPMorph_Ternary::DoTick(Landscape* landscape, float delta_time)
{
	arg1->Tick(landscape, delta_time);
	arg1->Tick(landscape, delta_time);
	arg3->Tick(landscape, delta_time);
	return true;
}


// =================================================================================

float LPMorph_Const::GetStrength(Landscape* landscape)
{
	return value;
}

bool LPMorph_Const::Initialize(MORPH_DATA_MAP const& data_map, TMObjectReferenceSolver& reference_solver)
{
	ReadDataMap(value, data_map, "VALUE");
	return true;
}

// =================================================================================

float LPMorph_Add::GetStrength(Landscape* landscape)
{
	float value1 = arg1->GetStrength(landscape);
	float value2 = arg2->GetStrength(landscape);
	return value1 + value2;
}

float LPMorph_Sub::GetStrength(Landscape* landscape)
{
	float value1 = arg1->GetStrength(landscape);
	float value2 = arg2->GetStrength(landscape);
	return value1 - value2;
}


float LPMorph_Mul::GetStrength(Landscape* landscape)
{
	float value1 = arg1->GetStrength(landscape);
	float value2 = arg2->GetStrength(landscape);
	return value1 * value2;
}


float LPMorph_Div::GetStrength(Landscape* landscape)
{
	float value1 = arg1->GetStrength(landscape);
	float value2 = arg2->GetStrength(landscape);
	return value1 / value2;
}

float LPMorph_Time::GetStrength(Landscape * landscape)
{
	return internal_time;
}

// =================================================================================

bool LPMorph_Rotate::GetPoints(Landscape* landscape, std::vector<glm::vec2>& mutable_points)
{
	float angle = arg1->GetStrength(landscape);
	float c = std::cos(angle);
	float s = std::sin(angle);

	arg2->GetPoints(landscape, mutable_points);

	for (auto& p : mutable_points)
		p = GLMTools::Rotate(p, c, s);

	return true;
}

float LPMorph_Neg::GetStrength(Landscape* landscape)
{
	return -arg1->GetStrength(landscape);
}

// =================================================================================

bool LPMorph_Translate::GetPoints(Landscape* landscape, std::vector<glm::vec2>& mutable_points)
{
	glm::vec2 delta = {
		arg1->GetStrength(landscape),
		arg2->GetStrength(landscape)
	};

	arg3->GetPoints(landscape, mutable_points);

	for (auto& p : mutable_points)
		p += delta;

	return true;
}

// =================================================================================

bool LPMorph_Circle::GetPoints(Landscape* landscape, std::vector<glm::vec2> & mutable_points)
{
	if (vertice_count > 0)
		mutable_points.resize(size_t(vertice_count));

	size_t count = mutable_points.size();

	for (size_t i = 0; i < count; ++i)
	{
		glm::vec2& v = mutable_points[i];

		float alpha = (float)i * 2.0f * (float)M_PI / (float)count;

		v.x = radius * std::cos(alpha);
		v.y = radius * std::sin(alpha);
	}

	return true;
}


bool LPMorph_Circle::Initialize(MORPH_DATA_MAP const & data_map, TMObjectReferenceSolver& reference_solver)
{
	LPMorph::Initialize(data_map, reference_solver);
	ReadDataMap(radius, data_map, "RADIUS");
	ReadDataMap(vertice_count, data_map, "VERTICE_COUNT");
	return true;
}

// =================================================================================

bool LPMorph_Gear::GetPoints(Landscape* landscape, std::vector<glm::vec2> & mutable_points)
{
	auto MyRot = [](glm::vec2 const& src, float alpha)
	{
		return GLMTools::Rotate(src, std::cos(alpha), std::sin(alpha));
	};

	if (gear_count < 3)
		gear_count = 3;

	mutable_points.reserve(4 * size_t(gear_count));
	mutable_points.clear();

	float gear_angle = 2.0f * float(M_PI) / float(gear_count);

	float half_angle = 0.5f * gear_angle;

	glm::vec2 T = {0.0f, r1};

	glm::vec2 A = MyRot(T, -0.5f * half_angle);
	glm::vec2 D = MyRot(T,  0.5f * half_angle);


	float c = std::cos(gear_angle);
	float s = std::sin(gear_angle);

	for (size_t i = 0; i < gear_count; ++i)
	{
		glm::vec2 N = glm::vec2(
			glm::cross(
				glm::vec3(D, 0.0f) - glm::vec3(A, 0.0f),
				glm::vec3(0.0f, 0.0f, 1.0f)
			));
		N = glm::normalize(N);

		glm::vec2 B = A + N * r2;
		glm::vec2 C = D + N * r2;

		mutable_points.push_back(A);
		mutable_points.push_back(B);
		mutable_points.push_back(C);
		mutable_points.push_back(D);

		A = GLMTools::Rotate(A, c, s);
		D = GLMTools::Rotate(D, c, s);
	}

	return true;
}

bool LPMorph_Gear::Initialize(MORPH_DATA_MAP const & data_map, TMObjectReferenceSolver& reference_solver)
{
	LPMorph::Initialize(data_map, reference_solver);
	ReadDataMap(r1, data_map, "r1");
	ReadDataMap(r2, data_map, "r2");
	ReadDataMap(gear_count, data_map, "gear_count");
	return true;
}

// =================================================================================

bool LPMorph_Rectangle::GetPoints(Landscape* landscape, std::vector<glm::vec2> & mutable_points)
{
	if (vertice_count <= 0)
		vertice_count = 4;
	mutable_points.resize(size_t(vertice_count));

	float width = landscape->ori_bounding_box.half_size.x * 2.0f;
	float height = landscape->ori_bounding_box.half_size.y * 2.0f;

	size_t count = mutable_points.size();
	for (size_t i = 0; i < count; ++i)
	{
		glm::vec2& v = mutable_points[i];

		if (i < count / 2)
		{
			float factor = MathTools::CastAndDiv<float>(i, count / 2 - 1);
			v.x = 0.5f * width - width * factor;
			v.y = height * 0.5f;
		}
		else
		{
			size_t other_i = i - count / 2;

			size_t remainder_count = count - (count / 2);

			float factor = MathTools::CastAndDiv<float>(other_i, remainder_count - 1);
			v.x = -0.5f * width + width * factor;
			v.y = -height * 0.5f;
		}
	}
	return true;
}

bool LPMorph_Rectangle::Initialize(MORPH_DATA_MAP const & data_map, TMObjectReferenceSolver& reference_solver)
{
	LPMorph::Initialize(data_map, reference_solver);
	ReadDataMap(vertice_count, data_map, "VERTICE_COUNT");
	return true;
}

// =================================================================================

bool LPMorph_Function::GetPoints(Landscape* landscape, std::vector<glm::vec2> & mutable_points)
{
	if (vertice_count > 0)
		mutable_points.resize(size_t(vertice_count));

	float strength = arg1->GetStrength(landscape);

	box2 bx = landscape->ori_bounding_box;

	float width  = bx.half_size.x * 2.0f;
	float height = bx.half_size.y * 2.0f;

	size_t count = mutable_points.size();
	for (size_t i = 0; i < count; ++i)
	{
		glm::vec2& v = mutable_points[i];

		if (i < count / 2)
		{
			float f = 1.0f - MathTools::CastAndDiv<float>(i, count / 2 - 1);
			v.x = 0.5f * width - width * MathTools::CastAndDiv<float>(i, count / 2 - 1);
			v.y = height * 0.5f + GetHeightValue(f, strength);
		}
		else
		{
			size_t other_i = i - count / 2;

			size_t remainder_count = count - (count / 2);

			v.x = -0.5f * width + width * MathTools::CastAndDiv<float>(other_i, remainder_count - 1);
			v.y = -height * 0.5f;
		}
	}
	return true;
}

bool LPMorph_Function::Initialize(MORPH_DATA_MAP const & data_map, TMObjectReferenceSolver& reference_solver)
{
	LPMorph_Unary::Initialize(data_map, reference_solver);
	ReadDataMap(vertice_count, data_map, "VERTICE_COUNT");
	return true;
}

// =================================================================================

float LPMorph_Wave::GetHeightValue(float x, float strength)
{
	float A = std::sin(x * float(M_PI));

	return A * amplitude * (1.0f + std::cos(2.0f * float(M_PI) * x / wave_length + strength));
}

bool LPMorph_Wave::Initialize(MORPH_DATA_MAP const & data_map, TMObjectReferenceSolver& reference_solver)
{
	LPMorph_Function::Initialize(data_map, reference_solver);
	ReadDataMap(amplitude, data_map, "AMPLITUDE");
	ReadDataMap(wave_length, data_map, "WAVE_LENGTH");
	return true;
}


// =================================================================================

bool LPMorph_Mix::GetPoints(Landscape* landscape, std::vector<glm::vec2> & mutable_points)
{
	float strength = arg1->GetStrength(landscape);

	arg2->GetPoints(landscape, mutable_points);

	std::vector<glm::vec2> v2(mutable_points.size());
	arg3->GetPoints(landscape, v2);

	assert(mutable_points.size() == v2.size());

	size_t count = mutable_points.size();
	for (size_t i = 0 ; i < count ; ++i)
		mutable_points[i] = mutable_points[i] * (1.0f - strength) + v2[i] * strength;

	return true;
}

// =================================================================================

bool LPMorph_BaseMesh::GetPoints(Landscape* landscape, std::vector<glm::vec2>& mutable_points)
{
	mutable_points = landscape->points;
	return true;
}

// =================================================================================

bool LPMorph_Scale::GetPoints(Landscape* landscape, std::vector<glm::vec2>& mutable_points)
{
	float strength = arg1->GetStrength(landscape);

	glm::vec2 scale = { strength, strength };
	if (ignore_x != 0)
		scale.x = 1.0f;
	if (ignore_y != 0)
		scale.y = 1.0f;

	arg2->GetPoints(landscape, mutable_points);
	for (auto& p : mutable_points)
		p = p * scale;
	return true;
}

bool LPMorph_Scale::Initialize(MORPH_DATA_MAP const& data_map, TMObjectReferenceSolver& reference_solver)
{
	LPMorph::Initialize(data_map, reference_solver);
	ReadDataMap(ignore_x, data_map, "IGNORE_X");
	ReadDataMap(ignore_y, data_map, "IGNORE_Y");
	return true;
}



// =================================================================================

float LPMorph_Mod::GetStrength(Landscape * landscape)
{
	float value = arg1->GetStrength(landscape);

	return A + MathTools::Modulo(value, B - A);
}

bool LPMorph_Mod::Initialize(MORPH_DATA_MAP const & data_map, TMObjectReferenceSolver& reference_solver)
{
	LPMorph_Unary::Initialize(data_map, reference_solver);
	ReadDataMap(A, data_map, "A");
	ReadDataMap(B, data_map, "B");
	return true;
}


// =================================================================================

float LPMorph_LinearStep::GetStrength(Landscape * landscape)
{
	float value = arg1->GetStrength(landscape);

	return linearstep(value, A, B);
}

bool LPMorph_LinearStep::Initialize(MORPH_DATA_MAP const & data_map, TMObjectReferenceSolver& reference_solver)
{
	LPMorph_Unary::Initialize(data_map, reference_solver);
	ReadDataMap(A, data_map, "A");
	ReadDataMap(B, data_map, "B");
	return true;
}

// =================================================================================

float LPMorph_LinearStepTwice::GetStrength(Landscape * landscape)
{
	float value = arg1->GetStrength(landscape);

	return linearstep(value, A, B) * (1.0f - linearstep(value, C, D));
}

bool LPMorph_LinearStepTwice::Initialize(MORPH_DATA_MAP const & data_map, TMObjectReferenceSolver& reference_solver)
{
	LPMorph_Unary::Initialize(data_map, reference_solver);
	ReadDataMap(A, data_map, "A");
	ReadDataMap(B, data_map, "B");
	ReadDataMap(C, data_map, "C");
	ReadDataMap(D, data_map, "D");
	return true;
}


// =================================================================================

float LPMorph_Cos::GetStrength(Landscape * landscape)
{
	float value = arg1->GetStrength(landscape);
	return offset + radius * std::cos(speed * value + time_offset);
}

bool LPMorph_Cos::Initialize(MORPH_DATA_MAP const & data_map, TMObjectReferenceSolver& reference_solver)
{
	LPMorph_Unary::Initialize(data_map, reference_solver);
	ReadDataMap(offset, data_map, "OFFSET");
	ReadDataMap(radius, data_map, "RADIUS");
	ReadDataMap(time_offset, data_map, "TIME_OFFSET");
	ReadDataMap(speed, data_map, "SPEED");
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
	if (morph != nullptr)
	{
		morph->Tick(this, delta_time);

		std::vector<glm::vec2> mutable_points = points;
		morph->GetPoints(this, mutable_points);
		point_bounding_box = BoxFromPoints(mutable_points);

		BuildMesh(mutable_points);
	}
	return true;
}

namespace GlobalVariables
{
#if _DEBUG
	CHAOS_GLOBAL_VARIABLE(bool, Wireframe);
	CHAOS_GLOBAL_VARIABLE(bool, DebugDisplay);
#endif
};

int Landscape::DoDisplay(GPURenderContext* render_context, GPUProgramProviderInterface const * uniform_provider, GPURenderParams const& render_params)
{
	int result = 0;

	chaos::GPUProgramProviderChain main_provider(uniform_provider);

	glm::mat4 local_to_world = glm::translate(glm::vec3(GetBoundingBox(true).position, 0.0f));
	main_provider.AddVariable("local_to_world", local_to_world);

#if _DEBUG
	if (GlobalVariables::Wireframe.Get())
	{
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	}
#endif

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glDisable(GL_DEPTH_TEST);
	glDisable(GL_CULL_FACE);
	glPointSize(5.0f);

	if (mesh != nullptr)
		result += mesh->Display(render_context, &main_provider, render_params);

#if _DEBUG

	if (GlobalVariables::DebugDisplay.Get())
	{
		// shu49. ca pourrait etre partique d avoir une fonction d affichage de bounding box

		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

		WindowApplication * window_application = Application::GetInstance();
		GPUDrawInterface<VertexDefault> DI(window_application->GetGPUDevice(), DefaultParticleProgram::GetMaterial(), 4);
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

		DI.Display(render_context, &main_provider, render_params);
	}
#endif

	glDisable(GL_BLEND);
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);

	glPolygonMode(GL_FRONT_AND_BACK,  GL_FILL);

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

		//assert(a != prev && prev != next && next != a);

		glm::vec2 extra_next = a + (next - a) * smooth_factor;
		glm::vec2 extra_prev = a + (prev - a) * smooth_factor;

		result.push_back(extra_prev);
		result.push_back(extra_next);

	}
	return SmoothBoundary(result, loop_count - 1, smooth_factor);
}

void Landscape::BuildMesh(std::vector<glm::vec2> const & src)
{
	smoothed_points = SmoothBoundary(src, smooth_count, smooth_factor);

	std::vector<glm::vec2> v = smoothed_points;

	// orientation of the polygon

	float accum = GetPolygonOrientation(v);

	polygon_orientation = (accum > 0.0f)? 1.0f : -1.0f;

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
			{
				v.erase(v.begin() + index);
				continue;
			}

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
			//assert(0);
		}
	}

	GPURenderMaterial * RM = GetLayerInstance()->GetRenderMaterial();

	WindowApplication * window_application = Application::GetInstance();
	GPUDrawInterface<VertexDefault> DI(window_application->GetGPUDevice(), RM, 3 * triangles.size());

	for (auto const& t : triangles)
	{
		auto tri = DI.AddTriangles(1);

		for (int i = 0; i < 3; ++i)
		{
			tri[i].position = t[i];
			tri[i].color = color;
		}
	}
	mesh = DI.GetDynamicMesh(mesh.get());
}

box2 Landscape::GetBoundingBox(bool world_system) const
{
	box2 result = point_bounding_box;


	result.position += bounding_box.position;

	if (world_system)
		result.position += layer_instance->GetLayerOffset();
	return result;

}




	template<typename T>
	std::vector<std::pair<SubClassOf<T>, MORPH_DATA_MAP>> LD49GetSubClassesAndNameFromString(char const* src, char separator = '\n')
	{
		std::vector<std::pair<SubClassOf<T>, MORPH_DATA_MAP>>  result;

		std::vector<std::string> class_names = StringTools::Split(src, separator);
		class_names.erase(std::remove_if(class_names.begin(), class_names.end(), [](std::string const& s) { return s.size() == 0; }), class_names.end());

		for (auto& class_name : class_names)
		{
			char const* t = class_name.c_str();
			while (*t == '\t')
				++t;

			std::vector<std::string> tokens = StringTools::Split(t, ' ');
			tokens.erase(std::remove_if(tokens.begin(), tokens.end(), [](std::string const& s) { return s.size() == 0; }), tokens.end());

			if (tokens.size() == 0)
				continue;

			MORPH_DATA_MAP data_map;
			// expected syntax
			//
			// classname  value=13.f (tout accroche)
			std::string name;
			for (size_t i = 1; i < tokens.size() ; ++i)
			{
				std::string const& tok = tokens[i];

				char const * sep = strchr(tok.c_str(), '=');
				if (sep != nullptr && sep != tok.c_str())
				{
					name = std::string(tok.c_str(), sep - tok.c_str());
					data_map[name] = (float)atof(sep + 1);
				}
			}


			SubClassOf<T> cls = ClassManager::GetDefaultInstance()->FindClass((std::string("LPMorph_") + tokens[0]).c_str());
			if (cls.IsValid())
			{
				result.emplace_back(cls, std::move(data_map));
			}

		}
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

		std::vector<shared_ptr<LPMorph>> morphs;

		std::vector<std::pair<SubClassOf<LPMorph>, MORPH_DATA_MAP>> morph_classes = LD49GetSubClassesAndNameFromString<LPMorph>(names->c_str(), '\n');
		for (auto cls : morph_classes)
		{
			if (LPMorph* m = cls.first.CreateInstance())
			{
				m->Initialize(cls.second, reference_solver);
				morphs.push_back(m);
			}
		}

		// prefix composition of tree
		if (morphs.size() > 0)
		{
			auto it = morphs.begin();
			if (morphs[0]->ComposeTree(++it, morphs.end()))
				morph = morphs[0];
			//assert(it == morphs.end()); // all consumed
		}
	}

	color = in_geometric_object->GetPropertyValueColor("COLOR", color);
	color.a = 1.0f;

	ori_bounding_box = bounding_box;

	// capture the points
	if (TiledMap::GeometricObjectPolygon const* pn = auto_cast(in_geometric_object))
	{
		points = pn->points;
		point_bounding_box = BoxFromPoints(points);

		// pas optimize quand il n y a pas de morph mais bon
		std::vector<glm::vec2> mutable_points = points;
		if (morph != nullptr)
			morph->GetPoints(this, mutable_points);
		point_bounding_box = BoxFromPoints(mutable_points);
		BuildMesh(mutable_points);
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

int LudumLevelInstance::DoDisplay(GPURenderContext* render_context, GPUProgramProviderInterface const * uniform_provider, GPURenderParams const& render_params)
{



	return TMLevelInstance::DoDisplay(render_context, uniform_provider, render_params);
}

uint64_t LudumLevelInstance::GetCollisionFlagByName(char const* name) const
{
	assert(name != nullptr);

	if (StringTools::Stricmp(name, "Landscape") == 0)
		return COLLISION_LANDSCAPE;

	return TMLevelInstance::GetCollisionFlagByName(name);
}
