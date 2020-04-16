#include <chaos/StandardHeaders.h> 
#include <chaos/FileTools.h> 
#include <chaos/LogTools.h> 
#include <chaos/GLTools.h> 
#include <chaos/StringTools.h> 
#include <chaos/GLMTools.h> 
#include <chaos/MyGLFWGamepadManager.h> 
#include <chaos/MyGLFWSingleWindowApplication.h> 
#include <chaos/MyGLFWWindow.h> 
#include <chaos/WinTools.h> 
#include <chaos/GPUProgramGenerator.h>
#include <chaos/Application.h>
#include <chaos/GeometryFramework.h>
#include <chaos/CollisionFramework.h>
#include <chaos/GPUSimpleMeshGenerator.h>
#include <chaos/SkyBoxTools.h>
#include <chaos/GLDebugOnScreenDisplay.h>
#include <chaos/FPSViewInputController.h>
#include <chaos/GPUSimpleMesh.h>
#include <chaos/GPUMultiMeshGenerator.h>
#include <chaos/GPUProgramData.h>
#include <chaos/GPUProgram.h>
#include <chaos/GPUVertexDeclaration.h>
#include <chaos/GPUProgramProvider.h>
#include <chaos/ConvexPolygonSplitter.h>

#include "PrimitiveRenderer.h"



static glm::vec4 const red   = glm::vec4(1.0f, 0.0f, 0.0f, 1.0f);
static glm::vec4 const green = glm::vec4(0.0f, 1.0f, 0.0f, 1.0f);
static glm::vec4 const blue  = glm::vec4(0.0f, 0.0f, 1.0f, 1.0f);
static glm::vec4 const white = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);

enum class TestID : int
{
	BOX2_DISPLAY_TEST,
	OBOX2_DISPLAY_TEST,
	SPHERE2_DISPLAY_TEST,
	TRIANGLE2_DISPLAY_TEST,

	BOX3_DISPLAY_TEST,
	OBOX3_DISPLAY_TEST,
	SPHERE3_DISPLAY_TEST,
	TRIANGLE3_DISPLAY_TEST,

	COLLISION_2D_TEST,
	COLLISION_3D_TEST,

#if 0

	COLLISION_BOX2_BOX2,
	COLLISION_BOX3_BOX3,
	COLLISION_SPHERE2_SPHERE2,
	COLLISION_SPHERE3_SPHERE3,
	COLLISION_TRIANGLE2_TRIANGLE2,
	COLLISION_TRIANGLE3_TRIANGLE3,

	COLLISION_BOX2_SPHERE2,
	COLLISION_BOX2_TRIANGLE2,


	COLLISION_BOX3_SPHERE3,
	COLLISION_BOX2_SPHERE2,
	COLLISION_BOX3_SPHERE3,

	RECTANGLE_CORNERS_TEST,
	CORNERS_TO_RECTANGLE_TEST,
	BOX_INTERSECTION_TEST,
	BOX_UNION_TEST,
	RESTRICT_BOX_INSIDE_1_TEST,
	RESTRICT_BOX_INSIDE_2_TEST,
	RESTRICT_BOX_INSIDE_3_TEST,
	RESTRICT_BOX_INSIDE_4_TEST,
	RESTRICT_SPHERE_INSIDE_1_TEST,
	RESTRICT_SPHERE_INSIDE_2_TEST,

	SPHERE_INTERSECTION_TEST,
	SPHERE_UNION_TEST,
	INNER_SPHERE_TEST,
	BOUNDING_SPHERE_TEST,
	BOUNDING_BOX_TEST,
	SPLIT_BOX_TEST,
	RESTRICT_BOX_OUTSIDE_TEST,
	RESTRICT_SPHERE_OUTSIDE_TEST,

	POINT_INSIDE_BOX_TEST,
	POINT_INSIDE_SPHERE_TEST,
	COLLISION_SHERE2_BOX2_TEST,
	COLLISION_SHERE2_TRIANGLE_TEST,
	COLLISION_POINT_TRIANGLE_TEST,
	BOX_COLLISION_TEST,
	SPHERE_COLLISION_TEST,


	OBOX_DISPLAY_TEST,
	OBOX_CORNERS_TEST,

	OBOX_BOUNDING_SPHERE_TEST,
	OBOX_BOUNDING_BOX_TEST,

	OBOX_INNER_SPHERE_TEST,
	POINT_INSIDE_OBOX_TEST,
#endif

	EXAMPLE_COUNT
};

enum class PrimitiveType : int
{
	PRIMITIVE_TYPE_BOX = 0,
	PRIMITIVE_TYPE_OBOX = 1,
	PRIMITIVE_TYPE_SPHERE = 2,
	PRIMITIVE_TYPE_TRIANGLE = 3,
	PRIMITIVE_TYPE_COUNT = 4
};


char const * GetPrimitiveName(PrimitiveType type)
{
	if (type == PrimitiveType::PRIMITIVE_TYPE_BOX)
		return "box";
	if (type == PrimitiveType::PRIMITIVE_TYPE_OBOX)
		return "obox";
	if (type == PrimitiveType::PRIMITIVE_TYPE_SPHERE)
		return "sphere";
	if (type == PrimitiveType::PRIMITIVE_TYPE_TRIANGLE)
		return "triangle";
	return "unknown";
}

class MyGLFWWindowOpenGLTest1 : public chaos::MyGLFW::Window
{
protected:

	char const * GetExampleTitle(TestID example)
	{
		if (example == TestID::BOX2_DISPLAY_TEST)         return "box2 display tests";
		if (example == TestID::OBOX2_DISPLAY_TEST)				return "obox2 display tests";
		if (example == TestID::SPHERE2_DISPLAY_TEST)      return "sphere2 display tests";
		if (example == TestID::TRIANGLE2_DISPLAY_TEST)    return "triangle2 display tests";

		if (example == TestID::BOX3_DISPLAY_TEST)         return "box3 display tests";
		if (example == TestID::OBOX3_DISPLAY_TEST)				return "obox3 display tests";
		if (example == TestID::SPHERE3_DISPLAY_TEST)      return "sphere3 display tests";
		if (example == TestID::TRIANGLE3_DISPLAY_TEST)    return "triangle3 display tests";

		if (example == TestID::COLLISION_2D_TEST || example == TestID::COLLISION_3D_TEST)
		{
			char const * type = (example == TestID::COLLISION_2D_TEST) ? "collision 2D" : "collision 3D";
			char const * p1 = GetPrimitiveName(prim_type_object1);
			char const * p2 = GetPrimitiveName(prim_type_object2);

			static std::string result;
			result = chaos::StringTools::Printf("%s: %s/%s", type, p1, p2);
			return result.c_str();
		}

		


#if 0


		if (example == TestID::RECTANGLE_CORNERS_TEST)         return "box.GetCorner(...)";
		if (example == TestID::CORNERS_TO_RECTANGLE_TEST)      return "construct box from corners";
		if (example == TestID::BOX_INTERSECTION_TEST)          return "box intersection";
		if (example == TestID::BOX_UNION_TEST)                 return "box union";
		if (example == TestID::RESTRICT_BOX_INSIDE_1_TEST)     return "restrict box displacement to inside : move bigger";
		if (example == TestID::RESTRICT_BOX_INSIDE_2_TEST)     return "restrict box displacement to inside : move smaller";
		if (example == TestID::RESTRICT_BOX_INSIDE_3_TEST)     return "restrict box displacement to inside : move bigger  (smaller is REAL bigger)";
		if (example == TestID::RESTRICT_BOX_INSIDE_4_TEST)     return "restrict box displacement to inside : move smaller (smaller is REAL bigger)";
		if (example == TestID::RESTRICT_SPHERE_INSIDE_1_TEST)  return "restrict sphere displacement to inside : move bigger";
		if (example == TestID::RESTRICT_SPHERE_INSIDE_2_TEST)  return "restrict sphere displacement to inside : move smaller";
		
		if (example == TestID::SPHERE_INTERSECTION_TEST)       return "sphere intersection";
		if (example == TestID::SPHERE_UNION_TEST)              return "sphere union";
		if (example == TestID::INNER_SPHERE_TEST)              return "inner sphere";
		if (example == TestID::BOUNDING_SPHERE_TEST)           return "bounding sphere";
		if (example == TestID::BOUNDING_BOX_TEST)              return "bounding box";
		if (example == TestID::SPLIT_BOX_TEST)                 return "split box";
		if (example == TestID::BOX_COLLISION_TEST)             return "box collision";
		if (example == TestID::SPHERE_COLLISION_TEST)          return "sphere collision";
		if (example == TestID::RESTRICT_BOX_OUTSIDE_TEST)      return "restrict box displacement to outside";
		if (example == TestID::RESTRICT_SPHERE_OUTSIDE_TEST)   return "restrict sphere displacement to outside";
		if (example == TestID::POINT_INSIDE_BOX_TEST)          return "point inside box";
		if (example == TestID::POINT_INSIDE_SPHERE_TEST)       return "point inside sphere";
		if (example == TestID::COLLISION_SHERE2_BOX2_TEST)     return "collision sphere2/box2";
		if (example == TestID::COLLISION_SHERE2_TRIANGLE_TEST) return "collision sphere2/triangle2";
		if (example == TestID::COLLISION_POINT_TRIANGLE_TEST)  return "collision point2/triangle2";
		if (example == TestID::OBOX_DISPLAY_TEST)              return "obox";
		if (example == TestID::OBOX_CORNERS_TEST)              return "obox corners";
		if (example == TestID::OBOX_BOUNDING_SPHERE_TEST)      return "obox bounding sphere";
		if (example == TestID::OBOX_BOUNDING_BOX_TEST)         return "obox bounding box";
		if (example == TestID::OBOX_INNER_SPHERE_TEST)         return "obox inner sphere";
		if (example == TestID::POINT_INSIDE_OBOX_TEST)         return "point inside obox";
		
#endif

		return nullptr;
	}

	void DebugDisplayExampleTitle()
	{
		debug_display.Clear();
		debug_display.AddLine(chaos::StringTools::Printf("=> Example %d : %s", display_example, GetExampleTitle(display_example)).c_str());

		debug_display.AddLine("");
		debug_display.AddLine("  +/-       : change example");
		debug_display.AddLine("  T         : freeze time");

		if (display_example == TestID::COLLISION_2D_TEST || display_example == TestID::COLLISION_3D_TEST)
		{			
			debug_display.AddLine("  LEFT CTRL : select object modified");
			debug_display.AddLine("  NUM 5     : change primitive type");
			debug_display.AddLine("  NUM 4-6   : move X");
			debug_display.AddLine("  NUM 1-7   : move Y");
			debug_display.AddLine("  NUM 2-8   : move Z");
			debug_display.AddLine("  NUM 3-9   : rotation");
		}		
	}

	template<typename T>
	void DrawIntersectionOrUnion(T p1, T p2, bool intersection)
	{
		double realtime = clock->GetClockTime();

		p1.position.x = 5.0f * (float)std::cos(1.5 * realtime * M_2_PI);
		p2.position.y = 5.0f * (float)std::cos(2.0 * realtime * M_2_PI);

		if (intersection)
		{
			primitive_renderer->GPUDrawPrimitive(primitive_renderer->SlightIncreaseSize(p1 & p2), white, false);

			primitive_renderer->GPUDrawPrimitive(p1, red, true);
			primitive_renderer->GPUDrawPrimitive(p2, blue, true);
		}
		else
		{
			primitive_renderer->GPUDrawPrimitive(p1, red, false);
			primitive_renderer->GPUDrawPrimitive(p2, blue, false);

			primitive_renderer->GPUDrawPrimitive(primitive_renderer->SlightIncreaseSize(p1 | p2), white, true);
		}  
	}

	template<typename T>
	void DrawRestrictToInside(T & smaller, T & bigger, bool move_bigger)
	{
		double realtime = clock->GetClockTime();

		if (move_bigger) // bigger should follow smaller
		{
			smaller.position.x = 20.0f * (float)std::cos(0.5 * realtime * M_2_PI);
			smaller.position.y =  5.0f * (float)std::sin(2.0 * realtime * M_2_PI);
		}
		else // smaller should follow bigger
		{
			bigger.position.x = 20.0f * (float)std::cos(0.5 * realtime * M_2_PI);
			bigger.position.y =  5.0f * (float)std::sin(2.0 * realtime * M_2_PI);
		}

		chaos::RestrictToInside(bigger, smaller, move_bigger);

		primitive_renderer->GPUDrawPrimitive(smaller, blue, false);
		primitive_renderer->GPUDrawPrimitive(primitive_renderer->SlightIncreaseSize(bigger), red, true);
	}

	template<typename T>
	void DrawRestrictToOutside(T & src, T & target)
	{
		double realtime = clock->GetClockTime();

		float x = (float)std::fmod(realtime, 2.0) - 1; // x in [-1 ... +1]

		float t = (x < 0) ? -x : x - 1.0f;

		int k = (int)std::fmod(realtime * 0.3, 3.0);

		src.position[(k + 0) % 3] = 20.0f * t;
		src.position[(k + 1) % 3] = 0.0f;
		src.position[(k + 2) % 3] = 0.0f;

		chaos::RestrictToOutside(src, target);

		primitive_renderer->GPUDrawPrimitive(src, blue, false);
		primitive_renderer->GPUDrawPrimitive(target, red, false);
	}


	template<typename T1, typename T2>
	void DrawCollisionImpl(T1 const & p1, T2 const & p2)
	{
		bool collision = chaos::Collide(p1, p2);
		primitive_renderer->GPUDrawPrimitive(p1, blue, collision);
		primitive_renderer->GPUDrawPrimitive(p2, red, collision);
	}


	template<typename T>
	void DrawCollision(T p1, T p2)
	{
		double realtime = clock->GetClockTime();

		p1.position.x = 10.0f * (float)std::cos(1.5 * realtime * M_2_PI);
		p1.position.y = 0.0;
		p2.position.x = 0.0;
		p2.position.y = 10.0f * (float)std::cos(2.0 * realtime * M_2_PI);

		DrawCollisionImpl(p1, p2);
	}

	void DrawSphereBox2Collision()
	{
		double realtime = clock->GetClockTime();

		chaos::box2 b2;
		b2.position.x = 0.0;
		b2.position.y = 20.0f * (float)std::cos(0.8 * realtime * M_2_PI);
		b2.half_size = glm::vec2(5.0f, 7.0f);

		chaos::sphere2 s2;
		s2.position.x = 10.0f * (float)std::cos(0.5 * realtime * M_2_PI);
		s2.position.y = 10.0f * (float)std::sin(0.5 * realtime * M_2_PI);
		s2.radius = 3.0f;

		DrawCollisionImpl(b2, s2);
	}

	void DrawTrianglePointCollision()
	{
		double realtime = 0.2 * clock->GetClockTime();

		chaos::triangle2 t2;
		t2.a = glm::vec2(5.0f, 0.0f);
		t2.c = glm::vec2(-5.0f, 0.0f);
		t2.b = glm::vec2(0.0f, 5.0f);
		t2 = chaos::PrepareTriangleForCollision(t2); // ensure the triangle order is good for collision function

		glm::vec2 p = glm::vec2(0.0f, 0.0f);
		p.x = 5.0f * (float)std::cos(realtime * M_2_PI);
		p.y = 5.0f * (float)std::sin(10.0f * realtime * M_2_PI);

		DrawCollisionImpl(glm::vec2(p.x, p.y), t2);
	}

	void DrawTriangleBox2Collision()
	{
		double realtime = 0.2 * clock->GetClockTime();

		chaos::triangle2 t2;
		t2.a = glm::vec2(5.0f, 0.0f);
		t2.c = glm::vec2(-5.0f, 0.0f);
		t2.b = glm::vec2(0.0f, 5.0f);
		t2 = chaos::PrepareTriangleForCollision(t2); // ensure the triangle order is good for collision function

		chaos::sphere2 s2;
		s2.position.x = 5.0f * (float)std::cos(realtime * M_2_PI);
		s2.position.y = 5.0f * (float)std::sin(10.0f * realtime * M_2_PI);
		s2.radius = 0.25f;

		DrawCollisionImpl(t2, s2);
	}

	template<typename T>
	void DrawPointInside(T p)
	{
		double realtime = clock->GetClockTime();

		glm::vec3 pos;
		pos.x = 15.0f * (float)std::cos(2.5 * realtime * M_2_PI);
		pos.y = 0.0f;
		pos.z = 0.0f;

		DrawCollisionImpl(pos, p);
	}






	// ========================================================
	glm::vec2 GetMovingPoint2() const
	{
		double realtime = clock->GetClockTime();

		glm::vec2 pos;
		pos.x = 5.0f * (float)std::cos(0.5 * realtime * M_2_PI);
		pos.y = 0.0f;
		return pos;
	}

	glm::vec3 GetMovingPoint3() const
	{
		double realtime = clock->GetClockTime();

		glm::vec3 pos;
		pos.x = 5.0f * (float)std::cos(0.5 * realtime * M_2_PI);
		pos.y = 0.0f;
		pos.z = 0.0f;
		return pos;
	}

	chaos::box2 GetMovingBox2() const
	{
		return chaos::box2(GetMovingPoint2(), glm::vec3(1.0f, 2.0f, 3.0f));
	}

	chaos::box3 GetMovingBox3() const
	{
		return chaos::box3(GetMovingPoint3(), glm::vec3(1.0f, 2.0f, 3.0f));
	}

	chaos::obox2 GetMovingOBox2() const
	{
		double realtime = clock->GetClockTime();
		float speed = 0.3f;
		float angle = speed * (float)realtime;

		return chaos::obox2(GetMovingPoint2(), glm::vec2(1.0f, 2.0f), angle);
	}

	chaos::obox3 GetMovingOBox3() const
	{
		double realtime = clock->GetClockTime();
		float speed = 0.3f;
		float angle = speed * (float)realtime;

		glm::vec3 axis = glm::normalize(glm::vec3(0.0f, 1.0f, 1.0f));
		return chaos::obox3(GetMovingPoint3(), glm::vec3(1.0f, 2.0f, 3.0f) , glm::angleAxis(angle, axis));
	}

	chaos::sphere2 GetMovingSphere2() const
	{
		return chaos::sphere2(GetMovingPoint2(), 1.0f);
	}

	chaos::sphere3 GetMovingSphere3() const
	{
		return chaos::sphere3(GetMovingPoint3(), 1.0f);
	}

	chaos::triangle2 GetMovingTriangle2() const
	{
		chaos::triangle2 t;
		t.a = glm::vec2(5.0f, 0.0f);
		t.c = glm::vec2(-5.0f, 0.0f);
		t.b = glm::vec2(0.0f, 5.0f);
		return t;
	}

	chaos::triangle3 GetMovingTriangle3() const
	{
		chaos::triangle3 t;
		t.a = glm::vec3(5.0f, 7.0f, 0.0f);
		t.c = glm::vec3(-5.0f, 6.0f, 0.0f);
		t.b = glm::vec3(0.0f, 8.0f, 5.0f);
		return t;
	}



	// ========================================================

	glm::vec2 Get2DVector(glm::vec3 const & src)
	{
		glm::vec2 res;
		res.x = src.x;
		res.y = src.z;
		return src;
	}

	void GetCollisionPrimitive(chaos::box2 & res, glm::vec3 const & p, float r)
	{
		res.position = Get2DVector(p);
		res.half_size = Get2DVector(glm::vec3(1.0f, 2.0f, 3.0f));
	}

	void GetCollisionPrimitive(chaos::box3 & res, glm::vec3 const & p, float r)
	{
		res.position  = p;
		res.half_size = glm::vec3(1.0f, 2.0f, 3.0f);
	}

	void GetCollisionPrimitive(chaos::obox2 & res, glm::vec3 const & p, float r)
	{
		res.position = Get2DVector(p);
		res.half_size = Get2DVector(glm::vec3(1.0f, 2.0f, 3.0f));
		res.rotator = r;
	}

	void GetCollisionPrimitive(chaos::obox3 & res, glm::vec3 const & p, float r)
	{
		res.position = p;
		res.half_size = glm::vec3(1.0f, 2.0f, 3.0f);

		glm::vec3 axis = glm::normalize(glm::vec3(0.0f, 1.0f, 1.0f));
		res.rotator = glm::angleAxis(r, axis);
	}

	void GetCollisionPrimitive(chaos::sphere2 & res, glm::vec3 const & p, float r)
	{
		res.position = Get2DVector(p); 
		res.radius = 1.0f;
	}

	void GetCollisionPrimitive(chaos::sphere3 & res, glm::vec3 const & p, float r)
	{
		res.position = p;
		res.radius = 1.0f;
	}

	void GetCollisionPrimitive(chaos::triangle2 & res, glm::vec3 const & p, float r)
	{
		glm::vec2 a = glm::vec2(2.0f, 0.0f);
		glm::vec2 b = glm::vec2(-2.0f, 0.0f);
		glm::vec2 c = glm::vec2(0.0f, 2.0f);

		glm::vec2 o = (a + b + c) / 3.0f;

		float cs = std::cos(r);
		float sn = std::sin(r);

		a = chaos::GLMTools::Rotate(a - o, cs, sn) + o;
		b = chaos::GLMTools::Rotate(b - o, cs, sn) + o;
		c = chaos::GLMTools::Rotate(c - o, cs, sn) + o;

		res.a = a + glm::vec2(p);
		res.b = b + glm::vec2(p);
		res.c = c + glm::vec2(p);
	}

	void GetCollisionPrimitive(chaos::triangle3 & res, glm::vec3 const & p, float r)
	{
		
		res.a = glm::vec3(5.0f, 7.0f, 0.0f);				
		res.b = glm::vec3(-5.0f, 6.0f, 0.0f);
		res.c = glm::vec3(0.0f, 8.0f, 5.0f);

		glm::vec3 axis = glm::normalize(glm::vec3(0.0f, 1.0f, 1.0f));
		glm::quat rot = glm::angleAxis(r, axis);

		res.a = glm::rotate(rot, res.a) + p;
		res.b = glm::rotate(rot, res.b) + p;
		res.c = glm::rotate(rot, res.c) + p;
	}



	template<typename PRIM1, typename PRIM2>
	void DrawPrimitiveCollision(PrimitiveType type1, PrimitiveType type2)
	{
		if (prim_type_object1 == type1 && prim_type_object2 == type2)
		{
			PRIM1 prim1;
			PRIM2 prim2;
			GetCollisionPrimitive(prim1, position_object1, rotation_object1);
			GetCollisionPrimitive(prim2, position_object2, rotation_object2);

			bool collision = chaos::Collide(prim1, prim2);
			primitive_renderer->GPUDrawPrimitive(prim1, blue, collision);
			primitive_renderer->GPUDrawPrimitive(prim2, red, collision);
		}

		if (prim_type_object1 == type2 && prim_type_object2 == type1)
		{
			PRIM2 prim1;
			PRIM1 prim2;
			GetCollisionPrimitive(prim1, position_object1, rotation_object1);
			GetCollisionPrimitive(prim2, position_object2, rotation_object2);

			bool collision = chaos::Collide(prim1, prim2);
			primitive_renderer->GPUDrawPrimitive(prim1, blue, collision);
			primitive_renderer->GPUDrawPrimitive(prim2, red, collision);
		}

	}

	// ========================================================

	void DrawGeometryObjects()
	{
		double realtime = clock->GetClockTime();

		// base display 2D
		if (display_example == TestID::BOX2_DISPLAY_TEST)
		{
			chaos::box2 b = GetMovingBox2();
			primitive_renderer->GPUDrawPrimitive(b, red, false);
		}
		if (display_example == TestID::OBOX2_DISPLAY_TEST)
		{
			chaos::obox2 b = GetMovingOBox2();
			primitive_renderer->GPUDrawPrimitive(b, red, false);
		}
		if (display_example == TestID::SPHERE2_DISPLAY_TEST)
		{
			chaos::sphere2 s = GetMovingSphere2();
			primitive_renderer->GPUDrawPrimitive(s, red, false);
		}
		if (display_example == TestID::TRIANGLE2_DISPLAY_TEST)
		{
			chaos::triangle2 t = GetMovingTriangle2();
			primitive_renderer->GPUDrawPrimitive(t, red, false);
		}

		// base display 3D
		if (display_example == TestID::BOX3_DISPLAY_TEST)
		{
			chaos::box3 b = GetMovingBox3();
			primitive_renderer->GPUDrawPrimitive(b, red, false);
		}
		if (display_example == TestID::OBOX3_DISPLAY_TEST)
		{
			chaos::obox3 b = GetMovingOBox3();
			primitive_renderer->GPUDrawPrimitive(b, red, false);
		}
		if (display_example == TestID::SPHERE3_DISPLAY_TEST)
		{
			chaos::sphere3 s = GetMovingSphere3();
			primitive_renderer->GPUDrawPrimitive(s, red, false);
		}
		if (display_example == TestID::TRIANGLE3_DISPLAY_TEST)
		{
			chaos::triangle3 t = GetMovingTriangle3();
			primitive_renderer->GPUDrawPrimitive(t, red, false);
		}

		// collisions
		if (display_example == TestID::COLLISION_2D_TEST)
		{
			DrawPrimitiveCollision<chaos::box2, chaos::box2>(PrimitiveType::PRIMITIVE_TYPE_BOX, PrimitiveType::PRIMITIVE_TYPE_BOX);
			DrawPrimitiveCollision<chaos::obox2, chaos::obox2>(PrimitiveType::PRIMITIVE_TYPE_OBOX, PrimitiveType::PRIMITIVE_TYPE_OBOX);
			DrawPrimitiveCollision<chaos::sphere2, chaos::sphere2>(PrimitiveType::PRIMITIVE_TYPE_SPHERE, PrimitiveType::PRIMITIVE_TYPE_SPHERE);
			DrawPrimitiveCollision<chaos::triangle2, chaos::triangle2>(PrimitiveType::PRIMITIVE_TYPE_TRIANGLE, PrimitiveType::PRIMITIVE_TYPE_TRIANGLE);

			DrawPrimitiveCollision<chaos::box2, chaos::obox2>(PrimitiveType::PRIMITIVE_TYPE_BOX, PrimitiveType::PRIMITIVE_TYPE_OBOX);
			DrawPrimitiveCollision<chaos::box2, chaos::sphere2>(PrimitiveType::PRIMITIVE_TYPE_BOX, PrimitiveType::PRIMITIVE_TYPE_SPHERE);
			DrawPrimitiveCollision<chaos::box2, chaos::triangle2>(PrimitiveType::PRIMITIVE_TYPE_BOX, PrimitiveType::PRIMITIVE_TYPE_TRIANGLE);

			DrawPrimitiveCollision<chaos::obox2, chaos::sphere2>(PrimitiveType::PRIMITIVE_TYPE_OBOX, PrimitiveType::PRIMITIVE_TYPE_SPHERE);
			DrawPrimitiveCollision<chaos::obox2, chaos::triangle2>(PrimitiveType::PRIMITIVE_TYPE_OBOX, PrimitiveType::PRIMITIVE_TYPE_TRIANGLE);

			DrawPrimitiveCollision<chaos::sphere2, chaos::triangle2>(PrimitiveType::PRIMITIVE_TYPE_SPHERE, PrimitiveType::PRIMITIVE_TYPE_TRIANGLE);
		}

		if (display_example == TestID::COLLISION_3D_TEST)
		{
			DrawPrimitiveCollision<chaos::box3, chaos::box3>(PrimitiveType::PRIMITIVE_TYPE_BOX, PrimitiveType::PRIMITIVE_TYPE_BOX);
			DrawPrimitiveCollision<chaos::obox3, chaos::obox3>(PrimitiveType::PRIMITIVE_TYPE_OBOX, PrimitiveType::PRIMITIVE_TYPE_OBOX);
			DrawPrimitiveCollision<chaos::sphere3, chaos::sphere3>(PrimitiveType::PRIMITIVE_TYPE_SPHERE, PrimitiveType::PRIMITIVE_TYPE_SPHERE);
			DrawPrimitiveCollision<chaos::triangle3, chaos::triangle3>(PrimitiveType::PRIMITIVE_TYPE_TRIANGLE, PrimitiveType::PRIMITIVE_TYPE_TRIANGLE);

			DrawPrimitiveCollision<chaos::box3, chaos::obox3>(PrimitiveType::PRIMITIVE_TYPE_BOX, PrimitiveType::PRIMITIVE_TYPE_OBOX);
			DrawPrimitiveCollision<chaos::box3, chaos::sphere3>(PrimitiveType::PRIMITIVE_TYPE_BOX, PrimitiveType::PRIMITIVE_TYPE_SPHERE);
			DrawPrimitiveCollision<chaos::box3, chaos::triangle3>(PrimitiveType::PRIMITIVE_TYPE_BOX, PrimitiveType::PRIMITIVE_TYPE_TRIANGLE);

			DrawPrimitiveCollision<chaos::obox3, chaos::sphere3>(PrimitiveType::PRIMITIVE_TYPE_OBOX, PrimitiveType::PRIMITIVE_TYPE_SPHERE);
			DrawPrimitiveCollision<chaos::obox3, chaos::triangle3>(PrimitiveType::PRIMITIVE_TYPE_OBOX, PrimitiveType::PRIMITIVE_TYPE_TRIANGLE);

			DrawPrimitiveCollision<chaos::sphere3, chaos::triangle3>(PrimitiveType::PRIMITIVE_TYPE_SPHERE, PrimitiveType::PRIMITIVE_TYPE_TRIANGLE);
		}









#if  0




		// display box and corners
		if (display_example == TestID::RECTANGLE_CORNERS_TEST)
		{
			chaos::box3 b(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(1.0f, 1.0f, 1.0f));

			primitive_renderer->GPUDrawPrimitive(b, red, false);

			std::pair<glm::vec3, glm::vec3> corners = GetBoxCorners(b);
			primitive_renderer->GPUDrawPrimitive(corners.first, white, false);
			primitive_renderer->GPUDrawPrimitive(corners.second, white, false);
		}

		// box construction from corners
		if (display_example == TestID::CORNERS_TO_RECTANGLE_TEST)
		{
			glm::vec3 p1(0.0f, 0.0f, 0.0f);
			glm::vec3 p2(1.0f, 2.0f, 3.0f);

			chaos::box3 b(std::make_pair(p1, p2));

			primitive_renderer->GPUDrawPrimitive(b, red, false);
			primitive_renderer->GPUDrawPrimitive(p1, white, false);
			primitive_renderer->GPUDrawPrimitive(p2, white, false);
		}

		// box union or intersection
		if (display_example == TestID::BOX_INTERSECTION_TEST || display_example == TestID::BOX_UNION_TEST)
		{
			chaos::box3 b1(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(1.0f, 2.0f, 3.0f));
			chaos::box3 b2(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(3.0f, 1.0f, 2.0f));
			DrawIntersectionOrUnion(b1, b2, display_example == BOX_INTERSECTION_TEST);
		}

		// restrict displacement
		if (
			display_example == TestID::RESTRICT_BOX_INSIDE_1_TEST ||
			display_example == TestID::RESTRICT_BOX_INSIDE_2_TEST ||
			display_example == TestID::RESTRICT_BOX_INSIDE_3_TEST ||
			display_example == TestID::RESTRICT_BOX_INSIDE_4_TEST )
			DrawRestrictToInside(smaller_box, bigger_box, display_example == TestID::RESTRICT_BOX_INSIDE_1_TEST || display_example == TestID::RESTRICT_BOX_INSIDE_3_TEST);

		if (display_example == TestID::RESTRICT_SPHERE_INSIDE_1_TEST || display_example == TestID::RESTRICT_SPHERE_INSIDE_2_TEST)
			DrawRestrictToInside(smaller_sphere, bigger_sphere, display_example == TestID::RESTRICT_SPHERE_INSIDE_1_TEST);



		// sphere union or intersection
		if (display_example == TestID::SPHERE_INTERSECTION_TEST || display_example == TestID::SPHERE_UNION_TEST)
		{
			chaos::sphere3 s1(glm::vec3(0.0f, 0.0f, 0.0f), 3.0f);
			chaos::sphere3 s2(glm::vec3(0.0f, 0.0f, 0.0f), 2.0f);
			DrawIntersectionOrUnion(s1, s2, display_example == TestID::SPHERE_INTERSECTION_TEST);
		}

		// inner sphere
		if (display_example == TestID::INNER_SPHERE_TEST)
		{
			chaos::box3 b(glm::vec3(2.0f, 3.0f, 4.0f), glm::vec3(1.0f, 2.0f, 3.0f));
			chaos::sphere3 s = GetInnerSphere(b);

			primitive_renderer->GPUDrawPrimitive(s, blue, false);
			primitive_renderer->GPUDrawPrimitive(b, red, true);
		}

		// bounding sphere
		if (display_example == TestID::BOUNDING_SPHERE_TEST)
		{
			chaos::box3 b(glm::vec3(2.0f, 3.0f, 4.0f), glm::vec3(1.0f, 2.0f, 3.0f));
			chaos::sphere3 s = GetBoundingSphere(b);

			primitive_renderer->GPUDrawPrimitive(b, red, false);
			primitive_renderer->GPUDrawPrimitive(s, blue, true);
		}
		// bounding box
		if (display_example == TestID::BOUNDING_BOX_TEST)
		{      
			chaos::sphere3 s(glm::vec3(1.0f, 2.0f, 3.0f), 3.0f);

			chaos::box3 b = GetBoundingBox(s);

			primitive_renderer->GPUDrawPrimitive(s, red, false);
			primitive_renderer->GPUDrawPrimitive(b, blue, true);
		}

		// split box
		if (display_example == TestID::SPLIT_BOX_TEST)
		{
			chaos::box3 b(glm::vec3(2.0f, 3.0f, 4.0f), glm::vec3(1.0f, 2.0f, 3.0f));

			for (int i = 0 ; i < 2 ; ++i)
			{
				for (int j = 0 ; j < 2 ; ++j)
				{
					for (int k = 0 ; k < 2 ; ++k)
					{
						chaos::box3 split_b = GetSplitBox(b, i, j, k);
						primitive_renderer->GPUDrawPrimitive(primitive_renderer->SlightDecreaseSize(split_b), red, false);
					}
				}
			}

			primitive_renderer->GPUDrawPrimitive(b, blue, true);
		}

		// box collision
		if (display_example == TestID::BOX_COLLISION_TEST)
		{
			chaos::box3 b1;
			chaos::box3 b2;

			b1.half_size = glm::vec3(4.0f, 5.0f, 6.0f);
			b2.half_size = glm::vec3(1.0f, 2.0f, 3.0f);
			DrawCollision(b1, b2);
		}

		// sphere collision
		if (display_example == TestID::SPHERE_COLLISION_TEST)
		{
			chaos::sphere3 s1;
			chaos::sphere3 s2;

			s1.radius = 3.0f;
			s2.radius = 2.0f;
			DrawCollision(s1, s2);
		}

		// restrict displacement
		if (display_example == TestID::RESTRICT_BOX_OUTSIDE_TEST)
			DrawRestrictToOutside(smaller_box, bigger_box);

		if (display_example == TestID::RESTRICT_SPHERE_OUTSIDE_TEST)
			DrawRestrictToOutside(smaller_sphere, bigger_sphere);

		if (display_example == TestID::POINT_INSIDE_BOX_TEST)
			DrawPointInside(bigger_box);

		if (display_example == TestID::POINT_INSIDE_SPHERE_TEST)
			DrawPointInside(bigger_sphere);

		// 2D collision
		if (display_example == TestID::COLLISION_SHERE2_BOX2_TEST)
			DrawSphereBox2Collision();

		if (display_example == TestID::COLLISION_SHERE2_TRIANGLE_TEST)
			DrawTriangleBox2Collision();


		if (display_example == TestID::COLLISION_POINT_TRIANGLE_TEST)
			DrawTrianglePointCollision();


		// Obox
		if (display_example == TestID::OBOX_DISPLAY_TEST || display_example == TestID::OBOX_CORNERS_TEST || display_example == TestID::OBOX_BOUNDING_SPHERE_TEST || display_example == TestID::OBOX_BOUNDING_BOX_TEST || display_example == TestID::OBOX_INNER_SPHERE_TEST || display_example == TestID::POINT_INSIDE_OBOX_TEST)
		{

#if 1
			glm::vec3 pt = glm::vec3(10.0f, 0.0f, 0.0f);



			float speed = 0.3f;
			float angle = speed * (float)realtime;
			glm::vec3 pos;
			pos.x = 15.0f * (float)std::cos(speed * 2.5 * realtime * M_2_PI);
			pos.y = 0.0f;
			pos.z = 0.0f;

			glm::vec3 axis = glm::normalize(glm::vec3(0.0f, 1.0f, 1.0f));

			chaos::obox3 b(pos, glm::vec3(1.0f, 2.0f, 3.0f), glm::angleAxis(angle, axis));



			bool transparent_obox = false;
			if (display_example == TestID::OBOX_INNER_SPHERE_TEST)
				transparent_obox = true;
			else if (display_example == TestID::POINT_INSIDE_OBOX_TEST && Collide(pt, b))
				transparent_obox = true;





			primitive_renderer->GPUDrawPrimitive(b, red, transparent_obox);

			if (display_example == TestID::OBOX_CORNERS_TEST)
			{
				glm::vec3 vertices[8];
				GetBoxVertices(b, vertices);
				for (int i = 0 ; i < 8 ; ++i)
					primitive_renderer->GPUDrawPrimitive(vertices[i], white, false);
			}

			if (display_example == TestID::OBOX_BOUNDING_SPHERE_TEST)
			{
				chaos::sphere3 s = GetBoundingSphere(b);
				primitive_renderer->GPUDrawPrimitive(s, blue, true);
			}

			if (display_example == TestID::OBOX_BOUNDING_BOX_TEST)
			{
				chaos::box3 box = GetBoundingBox(b);
				primitive_renderer->GPUDrawPrimitive(box, blue, true);
			}

			if (display_example == TestID::OBOX_INNER_SPHERE_TEST)
			{
				chaos::sphere3 s = GetInnerSphere(b);
				primitive_renderer->GPUDrawPrimitive(s, blue, false);
			}

			if (display_example == TestID::POINT_INSIDE_OBOX_TEST)
			{
				primitive_renderer->GPUDrawPrimitive(pt, white, false);
			}
#else


			glm::vec2 pt = glm::vec2(10.0f, 0.0f);



			float speed = 0.3f;
			float angle = speed * (float)realtime;
			glm::vec2 pos;
			pos.x = 15.0f * (float)std::cos(speed * 2.5 * realtime * M_2_PI);
			pos.y = 0.0f;

			chaos::obox2 b(pos, glm::vec3(1.0f, 2.0f, 3.0f), angle);



			bool transparent_obox = false;
			if (display_example == TestID::OBOX_INNER_SPHERE_TEST)
				transparent_obox = true;
			else if (display_example == TestID::POINT_INSIDE_OBOX_TEST && Collide(pt, b))
				transparent_obox = true;

			primitive_renderer->GPUDrawPrimitive(b, red, transparent_obox);

			if (display_example == TestID::OBOX_CORNERS_TEST)
			{
				glm::vec2 vertices[4];
				GetBoxVertices(b, vertices);
				for (int i = 0 ; i < 8 ; ++i)
					primitive_renderer->GPUDrawPrimitive(glm::vec3(vertices[i].x, 0.0f, vertices[i].y), white, false);
			}

			if (display_example == TestID::OBOX_BOUNDING_SPHERE_TEST)
			{
				chaos::sphere2 s = GetBoundingSphere(b);
				primitive_renderer->GPUDrawPrimitive(s, blue, true);
			}

			if (display_example == TestID::OBOX_BOUNDING_BOX_TEST)
			{
				chaos::box2 box = GetBoundingBox(b);
				primitive_renderer->GPUDrawPrimitive(box, blue, true);
			}

			if (display_example == TestID::OBOX_INNER_SPHERE_TEST)
			{
				chaos::sphere2 s = GetBoundingSphere(b);
				primitive_renderer->GPUDrawPrimitive(s, blue, false);
			}

			if (display_example == TestID::POINT_INSIDE_OBOX_TEST)
			{
				primitive_renderer->GPUDrawPrimitive(glm::vec3(pt.x, 0.0f, pt.y), white, false);
			}













#endif



		}

#endif





	}

	virtual bool OnDraw(chaos::GPURenderer * renderer, chaos::box2 const & viewport, glm::ivec2 window_size) override
	{
		glm::vec4 clear_color(0.0f, 0.7f, 0.0f, 0.0f);
		glClearBufferfv(GL_COLOR, 0, (GLfloat*)&clear_color);

		float far_plane = 1000.0f;
		glClearBufferfi(GL_DEPTH_STENCIL, 0, far_plane, 0);

		glEnable(GL_DEPTH_TEST);
		glEnable(GL_CULL_FACE);   // when viewer is inside the cube

		// XXX : the scaling is used to avoid the near plane clipping
		static float FOV = 60.0f;
		primitive_renderer->projection      = glm::perspectiveFov(FOV * (float)M_PI / 180.0f, 2.0f * viewport.half_size.x, 2.0f * viewport.half_size.y, 1.0f, far_plane);
		primitive_renderer->world_to_camera = fps_view_controller.GlobalToLocal();
		primitive_renderer->renderer        = renderer;

		DrawGeometryObjects();

		debug_display.Display((int)(2.0f * viewport.half_size.x), (int)(2.0f * viewport.half_size.y));

		return true;
	}

	virtual void Finalize() override
	{
		primitive_renderer = nullptr;

		debug_display.Finalize();

		if (clock != nullptr)
			clock->RemoveFromParent();

		chaos::MyGLFW::Window::Finalize();
	}

	virtual bool InitializeFromConfiguration(nlohmann::json const & config, boost::filesystem::path const & config_path) override
	{
		if (!chaos::MyGLFW::Window::InitializeFromConfiguration(config, config_path))
			return false;

		chaos::MyGLFW::SingleWindowApplication * application = chaos::MyGLFW::SingleWindowApplication::GetGLFWApplicationInstance();
		if (application == nullptr)
			return false;

		// compute resource path
		boost::filesystem::path resources_path = application->GetResourcesPath();
		boost::filesystem::path image_path = resources_path / "font.png";

		// initialize debug font display 
		chaos::GLDebugOnScreenDisplay::Params debug_params;
		debug_params.texture_path               = image_path;
		debug_params.font_characters            = " !\"#$%&'()*+,-./0123456789:;<=>?@ABCDEFGHIJKLMNOPQRSTUVWXYZ[\\]^_`abcdefghijklmnopqrstuvwxyz{|}~";
		debug_params.font_characters_per_line   = 10;
		debug_params.font_characters_line_count = 10;
		debug_params.character_width            = 20;
		debug_params.spacing                    = glm::ivec2( 0, 0);
		debug_params.crop_texture               = glm::ivec2(15, 7);

		if (!debug_display.Initialize(debug_params))
			return false;

		primitive_renderer = new PrimitiveRenderer();
		if (primitive_renderer == nullptr)
			return false;
		if (!primitive_renderer->Initialize())
			return false;

		// create a timer
		clock = application->GetMainClock()->CreateChildClock();

		// place camera
		fps_view_controller.fps_controller.position.y = 10.0f;
		fps_view_controller.fps_controller.position.z = 30.0f;
		fps_view_controller.keyboard_config.key_pitch_down = GLFW_KEY_UNKNOWN; // disable some inputs so they can be used else where
		fps_view_controller.keyboard_config.key_pitch_up   = GLFW_KEY_UNKNOWN;
		fps_view_controller.keyboard_config.key_yaw_left   = GLFW_KEY_UNKNOWN;
		fps_view_controller.keyboard_config.key_yaw_right  = GLFW_KEY_UNKNOWN;

		// initialize the example
		SetExample(TestID::BOX2_DISPLAY_TEST);

		// initial display
		DebugDisplayExampleTitle();

		return true;
	}

	virtual void TweakHints(chaos::MyGLFW::WindowHints & hints, GLFWmonitor * monitor, bool pseudo_fullscreen) const override
	{
		chaos::MyGLFW::Window::TweakHints(hints, monitor, pseudo_fullscreen);

		hints.toplevel  = 0;
		hints.decorated = 1;
	}

	void UpdateObjectPosition(int key, float delta_time, glm::vec3 const & factor)
	{
		static float SPEED = 5.0f;
		if (glfwGetKey(glfw_window, key) == GLFW_PRESS)
		{
			if (glfwGetKey(glfw_window, GLFW_KEY_LEFT_CONTROL) != GLFW_RELEASE)
				position_object1 += SPEED * (float)(delta_time)* factor;
			else
				position_object2 += SPEED * (float)(delta_time)* factor;
		}
	}

	void UpdateObjectRotation(int key, float delta_time, float factor)
	{
		static float SPEED = 1.0f;

		if (glfwGetKey(glfw_window, key) == GLFW_PRESS)
		{
			if (glfwGetKey(glfw_window, GLFW_KEY_LEFT_CONTROL) != GLFW_RELEASE)
				rotation_object1 += SPEED * (float)(delta_time)* factor;
			else
				rotation_object2 += SPEED * (float)(delta_time)* factor;
		}
	}

	void UpdateObjectType()
	{
		if (glfwGetKey(glfw_window, GLFW_KEY_LEFT_CONTROL) != GLFW_RELEASE)
			prim_type_object1 = (PrimitiveType)(((int)prim_type_object1 + 1) % (int)PrimitiveType::PRIMITIVE_TYPE_COUNT);
		else
			prim_type_object2 = (PrimitiveType)(((int)prim_type_object2 + 1) % (int)PrimitiveType::PRIMITIVE_TYPE_COUNT);
	}

	virtual bool Tick(float delta_time) override
	{
		fps_view_controller.Tick(glfw_window, delta_time);

		debug_display.Tick(delta_time);

		// update primitives
		if (display_example == TestID::COLLISION_2D_TEST || display_example == TestID::COLLISION_3D_TEST)
		{
			UpdateObjectPosition(GLFW_KEY_KP_6, delta_time, glm::vec3( 1.0f,  0.0f,  0.0f));
			UpdateObjectPosition(GLFW_KEY_KP_4, delta_time, glm::vec3(-1.0f,  0.0f,  0.0f));
			UpdateObjectPosition(GLFW_KEY_KP_8, delta_time, glm::vec3( 0.0f,  1.0f,  0.0f));
			UpdateObjectPosition(GLFW_KEY_KP_2, delta_time, glm::vec3( 0.0f, -1.0f,  0.0f));
			UpdateObjectPosition(GLFW_KEY_KP_1, delta_time, glm::vec3( 0.0f,  0.0f,  1.0f));
			UpdateObjectPosition(GLFW_KEY_KP_7, delta_time, glm::vec3( 0.0f,  0.0f, -1.0f));

			UpdateObjectRotation(GLFW_KEY_KP_9, delta_time,  1.0f);
			UpdateObjectRotation(GLFW_KEY_KP_3, delta_time, -1.0f);
		}

		return true; // refresh
	}

	virtual bool OnKeyEventImpl(chaos::KeyEvent const & event) override
	{
		if (event.IsKeyReleased(GLFW_KEY_T))
		{
			chaos::Clock * clock = chaos::MyGLFW::SingleWindowApplication::GetMainClockInstance();
			if (clock != nullptr)
				clock->Toggle();
			return true;
		}
		else if (event.IsKeyReleased(GLFW_KEY_KP_ADD))
		{
			SetExample((TestID)((int)display_example + 1));
			DebugDisplayExampleTitle();
			return true;
		}
		else if (event.IsKeyReleased(GLFW_KEY_KP_SUBTRACT))
		{
			SetExample((TestID)((int)display_example - 1));
			DebugDisplayExampleTitle();     
			return true;
		}
		else if (event.IsKeyReleased(GLFW_KEY_KP_5))
		{
			UpdateObjectType();
			DebugDisplayExampleTitle();
		}
		return chaos::MyGLFW::Window::OnKeyEventImpl(event);
	}

	void SetExample(TestID new_display_example)
	{
		new_display_example = (TestID)(((int)new_display_example + (int)TestID::EXAMPLE_COUNT) % (int)TestID::EXAMPLE_COUNT);
		display_example = new_display_example;

		// reset the time
		if (clock != nullptr)
			clock->Reset();

		// reset all collisions primitives
		if (clock != nullptr)
		{
			position_object1 = glm::vec3(-5.0f, 0.0f, 0.0f);
			position_object2 = glm::vec3( 5.0f, 0.0f, 0.0f);
			rotation_object1 = 0.0f;
			rotation_object2 = 0.0f;
		}		
	}

protected:

	// an object for debug rendering
	chaos::shared_ptr<PrimitiveRenderer> primitive_renderer;

	chaos::Clock * clock = nullptr;

	chaos::box3 bigger_box;
	chaos::box3 smaller_box;

	chaos::sphere3 bigger_sphere;
	chaos::sphere3 smaller_sphere;

	TestID display_example = TestID::BOX2_DISPLAY_TEST;

	chaos::FPSViewInputController fps_view_controller;

	chaos::GLDebugOnScreenDisplay debug_display;

	// some objects for collisions tests
	glm::vec3 position_object1 = glm::vec3(0.0f, 0.0f, 0.0f);
	glm::vec3 position_object2 = glm::vec3(0.0f, 0.0f, 0.0f);
	float     rotation_object1 = 0.0f;
	float     rotation_object2 = 0.0f;

	PrimitiveType prim_type_object1 = PrimitiveType::PRIMITIVE_TYPE_BOX;
	PrimitiveType prim_type_object2 = PrimitiveType::PRIMITIVE_TYPE_BOX;
};

int CHAOS_MAIN(int argc, char ** argv, char ** env)
{
	chaos::MyGLFW::SingleWindowApplicationParams params;
	params.monitor = nullptr;
	params.width = 800;
	params.height = 800;
	params.monitor_index = 0;
	chaos::MyGLFW::RunWindowApplication<MyGLFWWindowOpenGLTest1>(argc, argv, env, params);

	return 0;
}


