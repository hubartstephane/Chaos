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
#include <chaos/SimpleMeshGenerator.h>
#include <chaos/SkyBoxTools.h>
#include <chaos/GLDebugOnScreenDisplay.h>
#include <chaos/FPSViewInputController.h>
#include <chaos/SimpleMesh.h>
#include <chaos/MultiMeshGenerator.h>
#include <chaos/GPUProgramData.h>
#include <chaos/GPUProgram.h>
#include <chaos/GPUVertexDeclaration.h>
#include <chaos/GPUProgramProvider.h>
#include <chaos/ConvexPolygonSplitter.h>

#include "DrawFunctions.h"



static glm::vec4 const red   = glm::vec4(1.0f, 0.0f, 0.0f, 1.0f);
static glm::vec4 const green = glm::vec4(0.0f, 1.0f, 0.0f, 1.0f);
static glm::vec4 const blue  = glm::vec4(0.0f, 0.0f, 1.0f, 1.0f);
static glm::vec4 const white = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);

static int EXAMPLE_COUNT = 0;

static int const RECTANGLE_DISPLAY_TEST        = EXAMPLE_COUNT++;
static int const RECTANGLE_CORNERS_TEST        = EXAMPLE_COUNT++;
static int const CORNERS_TO_RECTANGLE_TEST     = EXAMPLE_COUNT++;
static int const BOX_INTERSECTION_TEST         = EXAMPLE_COUNT++;
static int const BOX_UNION_TEST                = EXAMPLE_COUNT++;
static int const RESTRICT_BOX_INSIDE_1_TEST    = EXAMPLE_COUNT++;
static int const RESTRICT_BOX_INSIDE_2_TEST    = EXAMPLE_COUNT++;
static int const RESTRICT_BOX_INSIDE_3_TEST    = EXAMPLE_COUNT++;
static int const RESTRICT_BOX_INSIDE_4_TEST    = EXAMPLE_COUNT++;
static int const RESTRICT_SPHERE_INSIDE_1_TEST = EXAMPLE_COUNT++;
static int const RESTRICT_SPHERE_INSIDE_2_TEST = EXAMPLE_COUNT++;
static int const SPHERE_DISPLAY_TEST           = EXAMPLE_COUNT++;
static int const SPHERE_INTERSECTION_TEST      = EXAMPLE_COUNT++;
static int const SPHERE_UNION_TEST             = EXAMPLE_COUNT++;
static int const INNER_SPHERE_TEST             = EXAMPLE_COUNT++;
static int const BOUNDING_SPHERE_TEST          = EXAMPLE_COUNT++;
static int const BOUNDING_BOX_TEST             = EXAMPLE_COUNT++;
static int const SPLIT_BOX_TEST                = EXAMPLE_COUNT++;
static int const BOX_COLLISION_TEST            = EXAMPLE_COUNT++;
static int const SPHERE_COLLISION_TEST         = EXAMPLE_COUNT++;
static int const RESTRICT_BOX_OUTSIDE_TEST     = EXAMPLE_COUNT++;
static int const RESTRICT_SPHERE_OUTSIDE_TEST  = EXAMPLE_COUNT++;
static int const POINT_INSIDE_BOX_TEST         = EXAMPLE_COUNT++;
static int const POINT_INSIDE_SPHERE_TEST      = EXAMPLE_COUNT++;
static int const COLLISION_SHERE2_BOX2_TEST    = EXAMPLE_COUNT++;
static int const COLLISION_SHERE2_TRIANGLE_TEST = EXAMPLE_COUNT++;
static int const COLLISION_POINT_TRIANGLE_TEST  = EXAMPLE_COUNT++;

static int const OBOX_DISPLAY_TEST  = EXAMPLE_COUNT++;
static int const OBOX_CORNERS_TEST  = EXAMPLE_COUNT++;

static int const OBOX_BOUNDING_SPHERE_TEST = EXAMPLE_COUNT++;
static int const OBOX_BOUNDING_BOX_TEST    = EXAMPLE_COUNT++;

static int const OBOX_INNER_SPHERE_TEST = EXAMPLE_COUNT++;
static int const POINT_INSIDE_OBOX_TEST = EXAMPLE_COUNT++;


static int const TEST_COUNT = EXAMPLE_COUNT;


class MyGLFWWindowOpenGLTest1 : public chaos::MyGLFW::Window
{
public:

	MyGLFWWindowOpenGLTest1()
	{
		SetExample(0);
	}

protected:

	char const * GetExampleTitle(int example)
	{
		if (example == RECTANGLE_DISPLAY_TEST)         return "boxes touch each others";
		if (example == RECTANGLE_CORNERS_TEST)         return "box.GetCorner(...)";
		if (example == CORNERS_TO_RECTANGLE_TEST)      return "construct box from corners";
		if (example == BOX_INTERSECTION_TEST)          return "box intersection";
		if (example == BOX_UNION_TEST)                 return "box union";
		if (example == RESTRICT_BOX_INSIDE_1_TEST)     return "restrict box displacement to inside : move bigger";
		if (example == RESTRICT_BOX_INSIDE_2_TEST)     return "restrict box displacement to inside : move smaller";
		if (example == RESTRICT_BOX_INSIDE_3_TEST)     return "restrict box displacement to inside : move bigger  (smaller is REAL bigger)";
		if (example == RESTRICT_BOX_INSIDE_4_TEST)     return "restrict box displacement to inside : move smaller (smaller is REAL bigger)";
		if (example == RESTRICT_SPHERE_INSIDE_1_TEST)  return "restrict sphere displacement to inside : move bigger";
		if (example == RESTRICT_SPHERE_INSIDE_2_TEST)  return "restrict sphere displacement to inside : move smaller";
		if (example == SPHERE_DISPLAY_TEST)            return "sphere touch each others";
		if (example == SPHERE_INTERSECTION_TEST)       return "sphere intersection";
		if (example == SPHERE_UNION_TEST)              return "sphere union";
		if (example == INNER_SPHERE_TEST)              return "inner sphere";
		if (example == BOUNDING_SPHERE_TEST)           return "bounding sphere";
		if (example == BOUNDING_BOX_TEST)              return "bounding box";
		if (example == SPLIT_BOX_TEST)                 return "split box";
		if (example == BOX_COLLISION_TEST)             return "box collision";
		if (example == SPHERE_COLLISION_TEST)          return "sphere collision";
		if (example == RESTRICT_BOX_OUTSIDE_TEST)      return "restrict box displacement to outside";
		if (example == RESTRICT_SPHERE_OUTSIDE_TEST)   return "restrict sphere displacement to outside";
		if (example == POINT_INSIDE_BOX_TEST)          return "point inside box";
		if (example == POINT_INSIDE_SPHERE_TEST)       return "point inside sphere";
		if (example == COLLISION_SHERE2_BOX2_TEST)     return "collision sphere2/box2";
		if (example == COLLISION_SHERE2_TRIANGLE_TEST) return "collision sphere2/triangle2";
		if (example == COLLISION_POINT_TRIANGLE_TEST)  return "collision point2/triangle2";
		if (example == OBOX_DISPLAY_TEST)              return "obox";
		if (example == OBOX_CORNERS_TEST)              return "obox corners";
		if (example == OBOX_BOUNDING_SPHERE_TEST)      return "obox bounding sphere";
		if (example == OBOX_BOUNDING_BOX_TEST)         return "obox bounding box";
		if (example == OBOX_INNER_SPHERE_TEST)         return "obox inner sphere";
		if (example == POINT_INSIDE_OBOX_TEST)         return "point inside obox";
		
		return nullptr;
	}

	void DebugDisplayExampleTitle(bool display_commands)
	{
		debug_display.Clear();
		if (display_commands)
		{
			debug_display.AddLine("=> Use +/- to change example");
			debug_display.AddLine("=> Use T   to freeze time");
			debug_display.AddLine("");
		}
		debug_display.AddLine(chaos::StringTools::Printf("=> Example %d : %s", display_example, GetExampleTitle(display_example)).c_str());
	}

	template<typename T>
	void DrawIntersectionOrUnion(RenderingContext const & ctx, T p1, T p2, bool intersection)
	{
		double realtime = clock->GetClockTime();

		p1.position.x = 5.0f * (float)chaos::MathTools::Cos(1.5 * realtime * M_2_PI);
		p2.position.y = 5.0f * (float)chaos::MathTools::Cos(2.0 * realtime * M_2_PI);

		if (intersection)
		{
			ctx.DrawPrimitive(ctx.SlightIncreaseSize(p1 & p2), white, false);

			ctx.DrawPrimitive(p1, red, true);
			ctx.DrawPrimitive(p2, blue, true);
		}
		else
		{
			ctx.DrawPrimitive(p1, red, false);
			ctx.DrawPrimitive(p2, blue, false);

			ctx.DrawPrimitive(ctx.SlightIncreaseSize(p1 | p2), white, true);
		}  
	}

	template<typename T>
	void DrawRestrictToInside(RenderingContext const & ctx, T & smaller, T & bigger, bool move_bigger)
	{
		double realtime = clock->GetClockTime();

		if (move_bigger) // bigger should follow smaller
		{
			smaller.position.x = 20.0f * (float)chaos::MathTools::Cos(0.5 * realtime * M_2_PI);
			smaller.position.y =  5.0f * (float)chaos::MathTools::Sin(2.0 * realtime * M_2_PI);
		}
		else // smaller should follow bigger
		{
			bigger.position.x = 20.0f * (float)chaos::MathTools::Cos(0.5 * realtime * M_2_PI);
			bigger.position.y =  5.0f * (float)chaos::MathTools::Sin(2.0 * realtime * M_2_PI);
		}

		chaos::RestrictToInside(bigger, smaller, move_bigger);

		ctx.DrawPrimitive(smaller, blue, false);
		ctx.DrawPrimitive(ctx.SlightIncreaseSize(bigger), red, true);
	}

	template<typename T>
	void DrawRestrictToOutside(RenderingContext const & ctx, T & src, T & target)
	{
		double realtime = clock->GetClockTime();

		float x = (float)chaos::MathTools::Fmod(realtime, 2.0) - 1; // x in [-1 ... +1]

		float t = (x < 0) ? -x : x - 1.0f;

		int k = (int)chaos::MathTools::Fmod(realtime * 0.3, 3.0);

		src.position[(k + 0) % 3] = 20.0f * t;
		src.position[(k + 1) % 3] = 0.0f;
		src.position[(k + 2) % 3] = 0.0f;

		chaos::RestrictToOutside(src, target);

		ctx.DrawPrimitive(src, blue, false);
		ctx.DrawPrimitive(target, red, false);
	}


	template<typename T1, typename T2>
	void DrawCollisionImpl(RenderingContext const & ctx, T1 const & p1, T2 const & p2)
	{
		bool collision = chaos::Collide(p1, p2);
		ctx.DrawPrimitive(p1, blue, collision);
		ctx.DrawPrimitive(p2, red, collision);
	}


	template<typename T>
	void DrawCollision(RenderingContext const & ctx, T p1, T p2)
	{
		double realtime = clock->GetClockTime();

		p1.position.x = 10.0f * (float)chaos::MathTools::Cos(1.5 * realtime * M_2_PI);
		p1.position.y = 0.0;
		p2.position.x = 0.0;
		p2.position.y = 10.0f * (float)chaos::MathTools::Cos(2.0 * realtime * M_2_PI);

		DrawCollisionImpl(ctx, p1, p2);
	}

	void DrawSphereBox2Collision(RenderingContext const & ctx)
	{
		double realtime = clock->GetClockTime();

		chaos::box2 b2;
		b2.position.x = 0.0;
		b2.position.y = 20.0f * (float)chaos::MathTools::Cos(0.8 * realtime * M_2_PI);
		b2.half_size = glm::vec2(5.0f, 7.0f);

		chaos::sphere2 s2;
		s2.position.x = 10.0f * (float)chaos::MathTools::Cos(0.5 * realtime * M_2_PI);
		s2.position.y = 10.0f * (float)chaos::MathTools::Sin(0.5 * realtime * M_2_PI);
		s2.radius = 3.0f;

		DrawCollisionImpl(ctx, b2, s2);
	}

	void DrawTrianglePointCollision(RenderingContext const & ctx)
	{
		double realtime = 0.2 * clock->GetClockTime();

		chaos::triangle2 t2;
		t2.a = glm::vec2(5.0f, 0.0f);
		t2.c = glm::vec2(-5.0f, 0.0f);
		t2.b = glm::vec2(0.0f, 5.0f);
		t2 = chaos::PrepareTriangleForCollision(t2); // ensure the triangle order is good for collision function

		glm::vec2 p = glm::vec2(0.0f, 0.0f);
		p.x = 5.0f * (float)chaos::MathTools::Cos(realtime * M_2_PI);
		p.y = 5.0f * (float)chaos::MathTools::Sin(10.0f * realtime * M_2_PI);

		DrawCollisionImpl(ctx, glm::vec2(p.x, p.y), t2);
	}

	void DrawTriangleBox2Collision(RenderingContext const & ctx)
	{
		double realtime = 0.2 * clock->GetClockTime();

		chaos::triangle2 t2;
		t2.a = glm::vec2(5.0f, 0.0f);
		t2.c = glm::vec2(-5.0f, 0.0f);
		t2.b = glm::vec2(0.0f, 5.0f);
		t2 = chaos::PrepareTriangleForCollision(t2); // ensure the triangle order is good for collision function

		chaos::sphere2 s2;
		s2.position.x = 5.0f * (float)chaos::MathTools::Cos(realtime * M_2_PI);
		s2.position.y = 5.0f * (float)chaos::MathTools::Sin(10.0f * realtime * M_2_PI);
		s2.radius = 0.25f;

		DrawCollisionImpl(ctx, t2, s2);
	}

	template<typename T>
	void DrawPointInside(RenderingContext const & ctx, T p)
	{
		double realtime = clock->GetClockTime();

		glm::vec3 pos;
		pos.x = 15.0f * (float)chaos::MathTools::Cos(2.5 * realtime * M_2_PI);
		pos.y = 0.0f;
		pos.z = 0.0f;

		DrawCollisionImpl(ctx, pos, p);
	}

	void DrawGeometryObjects(RenderingContext const & ctx)
	{
		double realtime = clock->GetClockTime();

		// ensure box touch alltogether
		if (display_example == RECTANGLE_DISPLAY_TEST)
		{
			chaos::box3 b1(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(1.0f, 1.0f, 1.0f));
			chaos::box3 b2(glm::vec3(2.0f, 0.0f, 0.0f), glm::vec3(1.0f, 1.0f, 1.0f));
			chaos::box3 b3(glm::vec3(0.0f, 0.0f, 2.0f), glm::vec3(1.0f, 1.0f, 1.0f));

			ctx.DrawPrimitive(b1, red, false);
			ctx.DrawPrimitive(b2, green, false);
			ctx.DrawPrimitive(b3, blue, false);
		}

		// display box and corners
		if (display_example == RECTANGLE_CORNERS_TEST)
		{
			chaos::box3 b(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(1.0f, 1.0f, 1.0f));

			ctx.DrawPrimitive(b, red, false);

			std::pair<glm::vec3, glm::vec3> corners = GetBoxExtremums(b);
			ctx.DrawPrimitive(corners.first, white, false);
			ctx.DrawPrimitive(corners.second, white, false);
		}

		// box construction from corners
		if (display_example == CORNERS_TO_RECTANGLE_TEST)
		{
			glm::vec3 p1(0.0f, 0.0f, 0.0f);
			glm::vec3 p2(1.0f, 2.0f, 3.0f);

			chaos::box3 b(std::make_pair(p1, p2));

			ctx.DrawPrimitive(b, red, false);
			ctx.DrawPrimitive(p1, white, false);
			ctx.DrawPrimitive(p2, white, false);
		}

		// box union or intersection
		if (display_example == BOX_INTERSECTION_TEST || display_example == BOX_UNION_TEST)
		{
			chaos::box3 b1(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(1.0f, 2.0f, 3.0f));
			chaos::box3 b2(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(3.0f, 1.0f, 2.0f));
			DrawIntersectionOrUnion(ctx, b1, b2, display_example == BOX_INTERSECTION_TEST);
		}

		// restrict displacement
		if (
			display_example == RESTRICT_BOX_INSIDE_1_TEST || 
			display_example == RESTRICT_BOX_INSIDE_2_TEST ||
			display_example == RESTRICT_BOX_INSIDE_3_TEST ||
			display_example == RESTRICT_BOX_INSIDE_4_TEST )
			DrawRestrictToInside(ctx, smaller_box, bigger_box, display_example == RESTRICT_BOX_INSIDE_1_TEST || display_example == RESTRICT_BOX_INSIDE_3_TEST);

		if (display_example == RESTRICT_SPHERE_INSIDE_1_TEST || display_example == RESTRICT_SPHERE_INSIDE_2_TEST)
			DrawRestrictToInside(ctx, smaller_sphere, bigger_sphere, display_example == RESTRICT_SPHERE_INSIDE_1_TEST);

		// ensure sphere touch alltogether
		if (display_example == SPHERE_DISPLAY_TEST)
		{
			chaos::sphere3 s1(glm::vec3(0.0f, 0.0f, 0.0f), 1.0f);
			chaos::sphere3 s2(glm::vec3(2.0f, 0.0f, 0.0f), 1.0f);
			chaos::sphere3 s3(glm::vec3(0.0f, 0.0f, 2.0f), 1.0f);

			ctx.DrawPrimitive(s1, red, false);
			ctx.DrawPrimitive(s2, green, false);
			ctx.DrawPrimitive(s3, blue, false);
		}

		// sphere union or intersection
		if (display_example == SPHERE_INTERSECTION_TEST || display_example == SPHERE_UNION_TEST)
		{
			chaos::sphere3 s1(glm::vec3(0.0f, 0.0f, 0.0f), 3.0f);
			chaos::sphere3 s2(glm::vec3(0.0f, 0.0f, 0.0f), 2.0f);
			DrawIntersectionOrUnion(ctx, s1, s2, display_example == SPHERE_INTERSECTION_TEST);
		}

		// inner sphere
		if (display_example == INNER_SPHERE_TEST)
		{
			chaos::box3 b(glm::vec3(2.0f, 3.0f, 4.0f), glm::vec3(1.0f, 2.0f, 3.0f));
			chaos::sphere3 s = GetInnerSphere(b);

			ctx.DrawPrimitive(s, blue, false);
			ctx.DrawPrimitive(b, red, true);
		}

		// bounding sphere
		if (display_example == BOUNDING_SPHERE_TEST)
		{
			chaos::box3 b(glm::vec3(2.0f, 3.0f, 4.0f), glm::vec3(1.0f, 2.0f, 3.0f));
			chaos::sphere3 s = GetBoundingSphere(b);

			ctx.DrawPrimitive(b, red, false);
			ctx.DrawPrimitive(s, blue, true);
		}
		// bounding box
		if (display_example == BOUNDING_BOX_TEST)
		{      
			chaos::sphere3 s(glm::vec3(1.0f, 2.0f, 3.0f), 3.0f);

			chaos::box3 b = GetBoundingBox(s);

			ctx.DrawPrimitive(s, red, false);
			ctx.DrawPrimitive(b, blue, true);
		}

		// split box
		if (display_example == SPLIT_BOX_TEST)
		{
			chaos::box3 b(glm::vec3(2.0f, 3.0f, 4.0f), glm::vec3(1.0f, 2.0f, 3.0f));

			for (int i = 0 ; i < 2 ; ++i)
			{
				for (int j = 0 ; j < 2 ; ++j)
				{
					for (int k = 0 ; k < 2 ; ++k)
					{
						chaos::box3 split_b = GetSplitBox(b, i, j, k);
						ctx.DrawPrimitive(ctx.SlightDecreaseSize(split_b), red, false);
					}
				}
			}

			ctx.DrawPrimitive(b, blue, true);
		}

		// box collision
		if (display_example == BOX_COLLISION_TEST)
		{
			chaos::box3 b1;
			chaos::box3 b2;

			b1.half_size = glm::vec3(4.0f, 5.0f, 6.0f);
			b2.half_size = glm::vec3(1.0f, 2.0f, 3.0f);
			DrawCollision(ctx, b1, b2);
		}

		// sphere collision
		if (display_example == SPHERE_COLLISION_TEST)
		{
			chaos::sphere3 s1;
			chaos::sphere3 s2;

			s1.radius = 3.0f;
			s2.radius = 2.0f;
			DrawCollision(ctx, s1, s2);
		}

		// restrict displacement
		if (display_example == RESTRICT_BOX_OUTSIDE_TEST)
			DrawRestrictToOutside(ctx, smaller_box, bigger_box);

		if (display_example == RESTRICT_SPHERE_OUTSIDE_TEST)
			DrawRestrictToOutside(ctx, smaller_sphere, bigger_sphere);

		if (display_example == POINT_INSIDE_BOX_TEST)
			DrawPointInside(ctx, bigger_box);

		if (display_example == POINT_INSIDE_SPHERE_TEST)
			DrawPointInside(ctx, bigger_sphere);

		// 2D collision
		if (display_example == COLLISION_SHERE2_BOX2_TEST)
			DrawSphereBox2Collision(ctx);

		if (display_example == COLLISION_SHERE2_TRIANGLE_TEST)
			DrawTriangleBox2Collision(ctx);


		if (display_example == COLLISION_POINT_TRIANGLE_TEST)
			DrawTrianglePointCollision(ctx);


		// Obox
		if (display_example == OBOX_DISPLAY_TEST || display_example == OBOX_CORNERS_TEST || display_example == OBOX_BOUNDING_SPHERE_TEST || display_example == OBOX_BOUNDING_BOX_TEST || display_example == OBOX_INNER_SPHERE_TEST || display_example == POINT_INSIDE_OBOX_TEST)
		{

#if 1
			glm::vec3 pt = glm::vec3(10.0f, 0.0f, 0.0f);



			float speed = 0.3f;
			float angle = speed * (float)realtime;
			glm::vec3 pos;
			pos.x = 15.0f * (float)chaos::MathTools::Cos(speed * 2.5 * realtime * M_2_PI);
			pos.y = 0.0f;
			pos.z = 0.0f;

			glm::vec3 axis = glm::normalize(glm::vec3(0.0f, 1.0f, 1.0f));

			chaos::obox3 b(pos, glm::vec3(1.0f, 2.0f, 3.0f), glm::angleAxis(angle, axis));



			bool transparent_obox = false;
			if (display_example == OBOX_INNER_SPHERE_TEST)
				transparent_obox = true;
			else if (display_example == POINT_INSIDE_OBOX_TEST && Collide(pt, b))
				transparent_obox = true;





			ctx.DrawPrimitive(b, red, transparent_obox);

			if (display_example == OBOX_CORNERS_TEST)
			{
				glm::vec3 vertices[8];
				GetBoxVertices(b, vertices);
				for (int i = 0 ; i < 8 ; ++i)
					ctx.DrawPrimitive(vertices[i], white, false);
			}

			if (display_example == OBOX_BOUNDING_SPHERE_TEST)
			{
				chaos::sphere3 s = GetBoundingSphere(b);
				ctx.DrawPrimitive(s, blue, true);
			}

			if (display_example == OBOX_BOUNDING_BOX_TEST)
			{
				chaos::box3 box = GetBoundingBox(b);
				ctx.DrawPrimitive(box, blue, true);
			}

			if (display_example == OBOX_INNER_SPHERE_TEST)
			{
				chaos::sphere3 s = GetInnerSphere(b);
				ctx.DrawPrimitive(s, blue, false);
			}

			if (display_example == POINT_INSIDE_OBOX_TEST)
			{
				ctx.DrawPrimitive(pt, white, false);
			}
#else


			glm::vec2 pt = glm::vec2(10.0f, 0.0f);



			float speed = 0.3f;
			float angle = speed * (float)realtime;
			glm::vec2 pos;
			pos.x = 15.0f * (float)chaos::MathTools::Cos(speed * 2.5 * realtime * M_2_PI);
			pos.y = 0.0f;

			chaos::obox2 b(pos, glm::vec3(1.0f, 2.0f, 3.0f), angle);



			bool transparent_obox = false;
			if (display_example == OBOX_INNER_SPHERE_TEST)
				transparent_obox = true;
			else if (display_example == POINT_INSIDE_OBOX_TEST && Collide(pt, b))
				transparent_obox = true;

			ctx.DrawPrimitive(b, red, transparent_obox);

			if (display_example == OBOX_CORNERS_TEST)
			{
				glm::vec2 vertices[4];
				GetBoxVertices(b, vertices);
				for (int i = 0 ; i < 8 ; ++i)
					ctx.DrawPrimitive(glm::vec3(vertices[i].x, 0.0f, vertices[i].y), white, false);
			}

			if (display_example == OBOX_BOUNDING_SPHERE_TEST)
			{
				chaos::sphere2 s = GetBoundingSphere(b);
				ctx.DrawPrimitive(s, blue, true);
			}

			if (display_example == OBOX_BOUNDING_BOX_TEST)
			{
				chaos::box2 box = GetBoundingBox(b);
				ctx.DrawPrimitive(box, blue, true);
			}

			if (display_example == OBOX_INNER_SPHERE_TEST)
			{
				chaos::sphere2 s = GetBoundingSphere(b);
				ctx.DrawPrimitive(s, blue, false);
			}

			if (display_example == POINT_INSIDE_OBOX_TEST)
			{
				ctx.DrawPrimitive(glm::vec3(pt.x, 0.0f, pt.y), white, false);
			}













#endif



		}







	}

	virtual bool OnDraw(chaos::Renderer * renderer, glm::ivec2 size) override
	{
		glm::vec4 clear_color(0.0f, 0.7f, 0.0f, 0.0f);
		glClearBufferfv(GL_COLOR, 0, (GLfloat*)&clear_color);

		float far_plane = 1000.0f;
		glClearBufferfi(GL_DEPTH_STENCIL, 0, far_plane, 0);

		glViewport(0, 0, size.x, size.y);
		glEnable(GL_DEPTH_TEST);
		glEnable(GL_CULL_FACE);   // when viewer is inside the cube

								  // XXX : the scaling is used to avoid the near plane clipping
		RenderingContext ctx;
		ctx.renderer = renderer;
		ctx.mesh_box = mesh_box;
		ctx.mesh_triangle = mesh_triangle;
		ctx.mesh_sphere = mesh_sphere;

		ctx.program_box = program_box;
		ctx.program_triangle = program_triangle;
		ctx.program_sphere = program_sphere;

		static float FOV = 60.0f;
		ctx.projection      = glm::perspectiveFov(FOV * (float)M_PI / 180.0f, (float)size.x, (float)size.y, 1.0f, far_plane);
		ctx.world_to_camera = fps_view_controller.GlobalToLocal();

		DrawGeometryObjects(ctx);

		debug_display.Display(size.x, size.y);

		return true;
	}

	virtual void Finalize() override
	{
		mesh_box      = nullptr;
		mesh_triangle = nullptr;
		mesh_sphere   = nullptr;

		program_box      = nullptr;
		program_triangle = nullptr;
		program_sphere   = nullptr;

		debug_display.Finalize();

		if (clock != nullptr)
			clock->RemoveFromParent();
	}

	chaos::shared_ptr<chaos::GPUProgram> LoadProgram(boost::filesystem::path const & resources_path, char const * ps_filename, char const * vs_filename)
	{
		chaos::GPUProgramGenerator program_generator;
		program_generator.AddShaderSourceFile(GL_FRAGMENT_SHADER, resources_path / ps_filename);
		program_generator.AddShaderSourceFile(GL_VERTEX_SHADER,   resources_path / vs_filename);

		return program_generator.GenProgramObject();
	}

	virtual bool InitializeFromConfiguration(nlohmann::json const & config, boost::filesystem::path const & config_path) override
	{
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

		// load programs      
		program_box = LoadProgram(resources_path, "pixel_shader_box.txt", "vertex_shader_box.txt");
		if (program_box == nullptr)
			return false;

		program_triangle = LoadProgram(resources_path, "pixel_shader_triangle.txt", "vertex_shader_triangle.txt");
		if (program_triangle == nullptr)
			return false;

		program_sphere = LoadProgram(resources_path, "pixel_shader_sphere.txt", "vertex_shader_sphere.txt");
		if (program_sphere == nullptr)
			return false;

		// create a timer
		clock = application->GetMainClock()->CreateChildClock();

		// create meshes
		chaos::triangle3 t; // data will be initialized in vertex shader as uniform
		chaos::box3      b = chaos::box3(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(1.0f, 1.0f, 1.0f));
		chaos::sphere3   s = chaos::sphere3(glm::vec3(0.0f, 0.0f, 0.0f), 1.0f);

		chaos::MultiMeshGenerator generators;    
		generators.AddGenerator(new chaos::SphereMeshGenerator(s, 10), mesh_sphere);
		generators.AddGenerator(new chaos::CubeMeshGenerator(b), mesh_box);
		generators.AddGenerator(new chaos::TriangleMeshGenerator(t), mesh_triangle);

		if (!generators.GenerateMeshes())
			return false;

		// place camera
		fps_view_controller.fps_controller.position.y = 10.0f;
		fps_view_controller.fps_controller.position.z = 30.0f;

		// initial display
		DebugDisplayExampleTitle(true);

		return true;
	}

	virtual void TweakHints(chaos::MyGLFW::WindowHints & hints, GLFWmonitor * monitor, bool pseudo_fullscreen) const override
	{
		chaos::MyGLFW::Window::TweakHints(hints, monitor, pseudo_fullscreen);

		hints.toplevel  = 0;
		hints.decorated = 1;
	}

	virtual bool Tick(double delta_time) override
	{
		if (glfwGetKey(glfw_window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
			RequireWindowClosure();

		fps_view_controller.Tick(glfw_window, delta_time);

		debug_display.Tick(delta_time);

		return true; // refresh
	}

	virtual bool OnKeyEvent(int key, int scan_code, int action, int modifier) override
	{
		if (key == GLFW_KEY_T && action == GLFW_RELEASE)
		{
			chaos::Clock * clock = chaos::MyGLFW::SingleWindowApplication::GetMainClockInstance();
			if (clock != nullptr)
				clock->Toggle();
			return true;
		}
		else if (key == GLFW_KEY_KP_ADD && action == GLFW_RELEASE)
		{
			SetExample(display_example + 1);
			DebugDisplayExampleTitle(false);
			return true;
		}
		else if (key == GLFW_KEY_KP_SUBTRACT && action == GLFW_RELEASE)
		{
			SetExample(display_example - 1);
			DebugDisplayExampleTitle(false);     
			return true;
		}
		return chaos::MyGLFW::Window::OnKeyEvent(key, scan_code, action, modifier);
	}

	void SetExample(int new_display_example)
	{
		new_display_example = (new_display_example + EXAMPLE_COUNT) % EXAMPLE_COUNT;

		// reset the time
		if (clock != nullptr)
			clock->Reset();

		// restore the box position each time example change
		bigger_box  = chaos::box3(glm::vec3(3.0f, 0.0f, 0.0f), glm::vec3(5.0f, 6.0f, 7.0f));
		smaller_box = chaos::box3(glm::vec3(-3.0f, 0.0f, 0.0f), glm::vec3(2.0f, 3.0f, 4.0f));

		if (new_display_example == RESTRICT_BOX_INSIDE_3_TEST || new_display_example == RESTRICT_BOX_INSIDE_4_TEST)
			smaller_box.half_size.x *= 4.0f;
		// restore the sphere position each time example change
		bigger_sphere  = chaos::sphere3(glm::vec3(3.0f, 0.0f, 0.0f), 7.0f);
		smaller_sphere = chaos::sphere3(glm::vec3(-3.0f, 0.0f, 0.0f), 3.0f);

		display_example = new_display_example;
	}

protected:

	// rendering for the box  
	chaos::shared_ptr<chaos::SimpleMesh> mesh_box;
	chaos::shared_ptr<chaos::GPUProgram>  program_box;

	// rendering for the triangle  
	chaos::shared_ptr<chaos::SimpleMesh> mesh_triangle;
	chaos::shared_ptr<chaos::GPUProgram>  program_triangle;

	// rendering for the rect
	chaos::shared_ptr<chaos::SimpleMesh> mesh_sphere;
	chaos::shared_ptr<chaos::GPUProgram>  program_sphere;

	chaos::Clock * clock = nullptr;

	chaos::box3 bigger_box;
	chaos::box3 smaller_box;

	chaos::sphere3 bigger_sphere;
	chaos::sphere3 smaller_sphere;

	int    display_example = 0;

	chaos::FPSViewInputController fps_view_controller;

	chaos::GLDebugOnScreenDisplay debug_display;
};

int CHAOS_MAIN(int argc, char ** argv, char ** env)
{
	chaos::triangle2 t2;
	chaos::triangle3 t3;

	chaos::box2 b2;
	chaos::box3 b3;

	chaos::sphere2 s2;
	chaos::sphere3 s3;

	glm::vec2 p2;
	glm::vec3 p3;

	auto a1 = chaos::GetClosestPoint(t2, p2);
	auto a2 = chaos::GetClosestPoint(t3, p3);

	auto a3 = chaos::GetClosestPoint(b2, p2);
	auto a4 = chaos::GetClosestPoint(b3, p3);

	auto a5 = chaos::GetClosestPoint(s2, p2);
	auto a6 = chaos::GetClosestPoint(s3, p3);

	{
		auto p1 = chaos::MathTools::bsr(1);
		auto p2 = chaos::MathTools::bsr(2);
		auto p3 = chaos::MathTools::bsr(3);
		auto p4 = chaos::MathTools::bsr(4);

		p1 = p1;

		int e = 19;
		while (e != 0)
		{
			int edge_index = chaos::MathTools::bsf(e);
			e &= ~(1 << edge_index);


			e = e;
		}

	}

	{

		chaos::ray2::ray_type rrr1;
		chaos::triangle2::ray_type rrr2;


		chaos::ray2 r2;
		chaos::ray3 r3;

		chaos::triangle2 t2;
		chaos::triangle3 t3;

		chaos::box2 b2;
		chaos::box3 b3;

		chaos::obox2 o2;
		chaos::obox3 o3;

		chaos::sphere2 s2;
		chaos::sphere3 s3;

		glm::vec3 plane_2;
		glm::vec4 plane_3;

		glm::vec2 res2;
		glm::vec3 res3;
	
		chaos::GetIntersection(r2, t2, res2);
		chaos::GetIntersection(r2, plane_2, res2);
		chaos::GetIntersection(r2, s2, res2, res2);
		chaos::GetIntersection(r2, b2, res2, res2);
		chaos::GetIntersection(r2, o2, res2, res2);


		chaos::GetIntersection(r3, t3, res3);
		chaos::GetIntersection(r3, plane_3, res3);
		chaos::GetIntersection(r3, s3, res3, res3);
		chaos::GetIntersection(r3, b3, res3, res3);
		chaos::GetIntersection(r3, o3, res3, res3);
	
	

	}

	{
		glm::vec2 v1(1.0f, 2.0f);
		glm::vec3 v2(1.0f, 2.0f, 0.0f);
		glm::vec3 v3(1.0f, 2.0f, 3.0f);
		glm::vec4 v4(1.0f, 2.0f, 3.0f, 0.0f);
		glm::vec4 v5(1.0f, 2.0f, 3.0f, 4.0f);

		auto a = chaos::GLMTools::RecastVector<glm::vec3>(v1);
		auto b = chaos::GLMTools::RecastVector<glm::vec4>(v1);

		auto c = chaos::GLMTools::RecastVector<glm::vec2>(v2);
		auto d = chaos::GLMTools::RecastVector<glm::vec4>(v2);

		auto e = chaos::GLMTools::RecastVector<glm::vec2>(v3);
		auto f = chaos::GLMTools::RecastVector<glm::vec4>(v3);


		auto g = chaos::GLMTools::RecastVector<glm::vec2>(v4);
		auto h = chaos::GLMTools::RecastVector<glm::vec3>(v4);

		auto i = chaos::GLMTools::RecastVector<glm::vec2>(v5);
		auto j = chaos::GLMTools::RecastVector<glm::vec3>(v5);
	
		a =a;
	}

	

	return 0;


	chaos::MyGLFW::SingleWindowApplicationParams params;
	params.monitor = nullptr;
	params.width = 500;
	params.height = 500;
	params.monitor_index = 0;
	chaos::MyGLFW::RunWindowApplication<MyGLFWWindowOpenGLTest1>(argc, argv, env, params);

	return 0;
}


