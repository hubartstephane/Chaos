#include <chaos/StandardHeaders.h> 
#include <chaos/FileTools.h> 
#include <chaos/LogTools.h> 
#include <chaos/GLTools.h> 
#include <chaos/StringTools.h> 
#include <chaos/MyGLFWGamepadManager.h> 
#include <chaos/MyGLFWWindow.h> 
#include <chaos/WinTools.h> 
#include <chaos/GLProgramLoader.h>
#include <chaos/Application.h>
#include <chaos/GeometryFramework.h>
#include <chaos/CollisionFramework.h>
#include <chaos/SimpleMeshGenerator.h>
#include <chaos/SkyBoxTools.h>
#include <chaos/GLDebugOnScreenDisplay.h>
#include <chaos/FPSViewInputController.h>
#include <chaos/SimpleMesh.h>
#include <chaos/MultiMeshGenerator.h>
#include <chaos/GLProgramData.h>
#include <chaos/GLProgram.h>
#include <chaos/VertexDeclaration.h>
#include <chaos/GLProgramVariableProvider.h>


static glm::vec4 const red   = glm::vec4(1.0f, 0.0f, 0.0f, 1.0f);
static glm::vec4 const green = glm::vec4(0.0f, 1.0f, 0.0f, 1.0f);
static glm::vec4 const blue  = glm::vec4(0.0f, 0.0f, 1.0f, 1.0f);
static glm::vec4 const white = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);

static glm::vec4 const solid       = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);
static glm::vec4 const translucent = glm::vec4(1.0f, 1.0f, 1.0f, 0.3f);

static int const RECTANGLE_DISPLAY_TEST        = 0;
static int const RECTANGLE_CORNERS_TEST        = 1;
static int const CORNERS_TO_RECTANGLE_TEST     = 2;
static int const BOX_INTERSECTION_TEST         = 3;
static int const BOX_UNION_TEST                = 4;
static int const RESTRICT_BOX_INSIDE_1_TEST    = 5;
static int const RESTRICT_BOX_INSIDE_2_TEST    = 6;
static int const RESTRICT_SPHERE_INSIDE_1_TEST = 7;
static int const RESTRICT_SPHERE_INSIDE_2_TEST = 8;
static int const SPHERE_DISPLAY_TEST           = 9;
static int const SPHERE_INTERSECTION_TEST      = 10;
static int const SPHERE_UNION_TEST             = 11;
static int const INNER_SPHERE_TEST             = 12;
static int const BOUNDING_SPHERE_TEST          = 13;
static int const BOUNDING_BOX_TEST             = 14;
static int const SPLIT_BOX_TEST                = 15;
static int const BOX_COLLISION_TEST            = 16;
static int const SPHERE_COLLISION_TEST         = 17;
static int const RESTRICT_BOX_OUTSIDE_TEST     = 18;
static int const RESTRICT_SPHERE_OUTSIDE_TEST  = 19;
static int const POINT_INSIDE_BOX_TEST         = 20;
static int const POINT_INSIDE_SPHERE_TEST      = 21;
static int const COLLISION_SHERE2_BOX2_TEST    = 22;


static int const TEST_COUNT = 23;

class RenderingContext
{
public:

  glm::mat4 projection;
  glm::mat4 world_to_camera;
};

class PrimitiveRenderingContext
{
public:

  glm::mat4 local_to_world;
  glm::vec4 color;
};


class MyGLFWWindowOpenGLTest1 : public chaos::MyGLFW::Window
{
public:

  MyGLFWWindowOpenGLTest1()
  {
    SetExample(0);
  }

protected:

  template<typename T, int dimension>
  chaos::type_box<T, dimension> SlightIncreaseSize(chaos::type_box<T, dimension> src) const
  {
    src.half_size *= static_cast<T>(1.01);
    return src;  
  }

  template<typename T>
  chaos::type_sphere3<T> SlightIncreaseSize(chaos::type_sphere3<T> src) const
  {
    src.radius *= static_cast<T>(1.01);
    return src;  
  }

  template<typename T, int dimension>
  chaos::type_box<T, dimension> SlightDecreaseSize(chaos::type_box<T, dimension> src) const
  {
    src.half_size *= static_cast<T>(0.90);
    return src;
  }

  template<typename T>
  chaos::type_sphere3<T> SlightDecreaseSize(chaos::type_sphere3<T> src) const
  {
    src.radius *= static_cast<T>(0.90);
    return src;
  }

  char const * GetExampleTitle(int example)
  {
    if (example == RECTANGLE_DISPLAY_TEST)        return "boxes touch each others";
    if (example == RECTANGLE_CORNERS_TEST)        return "box.GetCorner(...)";
    if (example == CORNERS_TO_RECTANGLE_TEST)     return "construct box from corners";
    if (example == BOX_INTERSECTION_TEST)         return "box intersection";
    if (example == BOX_UNION_TEST)                return "box union";
    if (example == RESTRICT_BOX_INSIDE_1_TEST)    return "restrict box displacement to inside : move bigger";
    if (example == RESTRICT_BOX_INSIDE_2_TEST)    return "restrict box displacement to inside : move smaller";
    if (example == RESTRICT_SPHERE_INSIDE_1_TEST) return "restrict sphere displacement to inside : move bigger";
    if (example == RESTRICT_SPHERE_INSIDE_2_TEST) return "restrict sphere displacement to inside : move smaller";
    if (example == SPHERE_DISPLAY_TEST)           return "sphere touch each others";
    if (example == SPHERE_INTERSECTION_TEST)      return "sphere intersection";
    if (example == SPHERE_UNION_TEST)             return "sphere union";
    if (example == INNER_SPHERE_TEST)             return "inner sphere";
    if (example == BOUNDING_SPHERE_TEST)          return "bounding sphere";
    if (example == BOUNDING_BOX_TEST)             return "bounding box";
    if (example == SPLIT_BOX_TEST)                return "split box";
    if (example == BOX_COLLISION_TEST)            return "box collision";
    if (example == SPHERE_COLLISION_TEST)         return "sphere collision";
    if (example == RESTRICT_BOX_OUTSIDE_TEST)     return "restrict box displacement to outside";
    if (example == RESTRICT_SPHERE_OUTSIDE_TEST)  return "restrict sphere displacement to outside";
    if (example == POINT_INSIDE_BOX_TEST)         return "point inside box";
    if (example == POINT_INSIDE_SPHERE_TEST)      return "point inside sphere";
    if (example == COLLISION_SHERE2_BOX2_TEST)    return "collision sphere2/box2";
      
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

  void PrepareObjectProgram(chaos::GLProgram * program, RenderingContext const & ctx, PrimitiveRenderingContext const & prim_ctx)
  {
    chaos::GLProgramData const & program_data = program->GetProgramData();

    glUseProgram(program->GetResourceID());

    chaos::GLProgramVariableProviderChain uniform_provider;
    uniform_provider.AddVariableValue("projection", ctx.projection);
    uniform_provider.AddVariableValue("world_to_camera", ctx.world_to_camera);
    uniform_provider.AddVariableValue("local_to_world", prim_ctx.local_to_world);
    uniform_provider.AddVariableValue("color", prim_ctx.color);
    program_data.BindUniforms(&uniform_provider);
  }

  void DrawPrimitiveImpl(
    RenderingContext const & ctx,
    chaos::SimpleMesh * mesh,
    chaos::GLProgram  * program,
    glm::vec4 const & color, 
    glm::mat4 const & local_to_world, 
    bool is_translucent)
  {
    glm::vec4 final_color = color;
    if (is_translucent)
    {
      BeginTranslucency();
      final_color *= translucent;
    }

    PrimitiveRenderingContext prim_ctx;
    prim_ctx.local_to_world = local_to_world;
    prim_ctx.color          = final_color;

    PrepareObjectProgram(program, ctx, prim_ctx);

    mesh->Render(program->GetProgramData(), nullptr, 0, 0);

    if (is_translucent)
      EndTranslucency();
  }

  void DrawPrimitive(RenderingContext const & ctx, chaos::sphere3 const & s, glm::vec4 const & color, bool is_translucent)
  {
    if (s.IsEmpty())
      return;

    glm::mat4 local_to_world = glm::translate(s.position) * glm::scale(glm::vec3(s.radius, s.radius, s.radius));

    DrawPrimitiveImpl(
      ctx,
      get_pointer(mesh_sphere),
      get_pointer(program_sphere),
      color,
      local_to_world,
      is_translucent
    );
  }

  void DrawPrimitive(RenderingContext const & ctx, chaos::sphere2 s, glm::vec4 const & color, bool is_translucent)
  {
    if (s.IsEmpty())
      return;
    chaos::sphere3 s3;
    s3.position = glm::vec3(s.position.x, 0.0f, s.position.y);
    s3.radius = s.radius;
    DrawPrimitive(ctx, s3, color, is_translucent);
  }

  void DrawPrimitive(RenderingContext const & ctx, chaos::box3 const & b, glm::vec4 const & color, bool is_translucent)
  {
    if (b.IsEmpty())
      return;

    glm::mat4 local_to_world = glm::translate(b.position) * glm::scale(b.half_size);

    DrawPrimitiveImpl(
      ctx,
      get_pointer(mesh_box),
      get_pointer(program_box),
      color,
      local_to_world,
      is_translucent
    );
  }

  void DrawPrimitive(RenderingContext const & ctx, chaos::box2 b, glm::vec4 const & color, bool is_translucent)
  {
    if (b.IsEmpty())
      return;
    chaos::box3 b3;
    b3.position = glm::vec3(b.position.x, 0.0f, b.position.y);
    b3.half_size = glm::vec3(b.half_size.x, 0.5f, b.half_size.y);
    DrawPrimitive(ctx, b3, color, is_translucent);
  }

  void DrawPoint(RenderingContext const & ctx, glm::vec3 const & p, glm::vec4 const & color)
  {
    glm::vec3 half_point_size(0.125f);
    DrawPrimitive(ctx, chaos::box3(p, half_point_size), color, false);  
  }












  void BeginTranslucency()
  {
    glEnable(GL_BLEND);
    glDepthMask(GL_FALSE);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
  }

  void EndTranslucency()
  {
    glDepthMask(GL_TRUE);
    glDisable(GL_BLEND);
  }

  template<typename T>
  void DrawIntersectionOrUnion(RenderingContext const & ctx, T p1, T p2, bool intersection)
  {
    double realtime = clock->GetClockTime();

    p1.position.x = 5.0f * (float)chaos::MathTools::Cos(1.5 * realtime * M_2_PI);
    p2.position.y = 5.0f * (float)chaos::MathTools::Cos(2.0 * realtime * M_2_PI);

    if (intersection)
    {
      DrawPrimitive(ctx, SlightIncreaseSize(p1 & p2), white, false);

      DrawPrimitive(ctx, p1, red, true);
      DrawPrimitive(ctx, p2, blue, true);
    }
    else
    {
      DrawPrimitive(ctx, p1, red, false);
      DrawPrimitive(ctx, p2, blue, false);

      DrawPrimitive(ctx, SlightIncreaseSize(p1 | p2), white, true);
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

    DrawPrimitive(ctx, smaller, blue, false);
    DrawPrimitive(ctx, SlightIncreaseSize(bigger), red, true);
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

    DrawPrimitive(ctx, src, blue, false);
    DrawPrimitive(ctx, target, red, false);
  }

  template<typename T>
  void DrawCollision(RenderingContext const & ctx, T p1, T p2)
  {
    double realtime = clock->GetClockTime();

    p1.position.x = 10.0f * (float)chaos::MathTools::Cos(1.5 * realtime * M_2_PI);
    p1.position.y = 0.0;
    p2.position.x = 0.0;
    p2.position.y = 10.0f * (float)chaos::MathTools::Cos(2.0 * realtime * M_2_PI);

    bool collision = chaos::Collide(p1, p2);

    DrawPrimitive(ctx, p1, blue, collision);
    DrawPrimitive(ctx, p2, red, collision);
  }

  void Draw2DCollision(RenderingContext const & ctx)
  {
    double realtime = clock->GetClockTime();

    chaos::box2 b2;
    b2.position.x = 20.0f * (float)chaos::MathTools::Cos(0.8 * realtime * M_2_PI);
    b2.position.y = 0.0;
    b2.half_size = glm::vec2(3.0f, 2.0f);

    chaos::sphere2 s2;
    s2.position.x = 10.0f * (float)chaos::MathTools::Cos(0.5 * realtime * M_2_PI);
    s2.position.y = 10.0f * (float)chaos::MathTools::Sin(0.5 * realtime * M_2_PI);
    s2.radius = 3.0f;

    bool collision = chaos::Collide(b2, s2);
    DrawPrimitive(ctx, b2, blue, true);
    DrawPrimitive(ctx, s2, red, collision);
  }





  template<typename T>
  void DrawPointInside(RenderingContext const & ctx, T p)
  {
    double realtime = clock->GetClockTime();

    glm::vec3 pos;
    pos.x = 15.0f * (float)chaos::MathTools::Cos(2.5 * realtime * M_2_PI);
    pos.y = 0.0f;
    pos.z = 0.0f;

    DrawPoint(ctx, pos, white);
    DrawPrimitive(ctx, p, red, p.Contains(pos));
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

      DrawPrimitive(ctx, b1, red, false);
      DrawPrimitive(ctx, b2, green, false);
      DrawPrimitive(ctx, b3, blue, false);
    }

    // display box and corners
    if (display_example == RECTANGLE_CORNERS_TEST)
    {
      chaos::box3 b(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(1.0f, 1.0f, 1.0f));

      DrawPrimitive(ctx, b, red, false);

      std::pair<glm::vec3, glm::vec3> corners = b.GetCorners();
      DrawPoint(ctx, corners.first, white);
      DrawPoint(ctx, corners.second, white);
    }

    // box construction from corners
    if (display_example == CORNERS_TO_RECTANGLE_TEST)
    {
      glm::vec3 p1(0.0f, 0.0f, 0.0f);
      glm::vec3 p2(1.0f, 2.0f, 3.0f);

      chaos::box3 b(std::make_pair(p1, p2));

      DrawPrimitive(ctx, b, red, false);
      DrawPoint(ctx, p1, white);
      DrawPoint(ctx, p2, white);
    }

    // box union or intersection
    if (display_example == BOX_INTERSECTION_TEST || display_example == BOX_UNION_TEST)
    {
      chaos::box3 b1(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(1.0f, 2.0f, 3.0f));
      chaos::box3 b2(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(3.0f, 1.0f, 2.0f));
      DrawIntersectionOrUnion(ctx, b1, b2, display_example == BOX_INTERSECTION_TEST);
    }
    
    // restrict displacement
    if (display_example == RESTRICT_BOX_INSIDE_1_TEST || display_example == RESTRICT_BOX_INSIDE_2_TEST)
      DrawRestrictToInside(ctx, smaller_box, bigger_box, display_example == RESTRICT_BOX_INSIDE_1_TEST);

    if (display_example == RESTRICT_SPHERE_INSIDE_1_TEST || display_example == RESTRICT_SPHERE_INSIDE_2_TEST)
      DrawRestrictToInside(ctx, smaller_sphere, bigger_sphere, display_example == RESTRICT_SPHERE_INSIDE_1_TEST);
    
    // ensure sphere touch alltogether
    if (display_example == SPHERE_DISPLAY_TEST)
    {
      chaos::sphere3 s1(glm::vec3(0.0f, 0.0f, 0.0f), 1.0f);
      chaos::sphere3 s2(glm::vec3(2.0f, 0.0f, 0.0f), 1.0f);
      chaos::sphere3 s3(glm::vec3(0.0f, 0.0f, 2.0f), 1.0f);

      DrawPrimitive(ctx, s1, red, false);
      DrawPrimitive(ctx, s2, green, false);
      DrawPrimitive(ctx, s3, blue, false);
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

      DrawPrimitive(ctx, s, blue, false);
      DrawPrimitive(ctx, b, red, true);
    }

    // bounding sphere
    if (display_example == BOUNDING_SPHERE_TEST)
    {
      chaos::box3 b(glm::vec3(2.0f, 3.0f, 4.0f), glm::vec3(1.0f, 2.0f, 3.0f));
      chaos::sphere3 s = GetBoundingSphere(b);

      DrawPrimitive(ctx, b, red, false);
      DrawPrimitive(ctx, s, blue, true);
    }
    // bounding box
    if (display_example == BOUNDING_BOX_TEST)
    {      
      chaos::sphere3 s(glm::vec3(1.0f, 2.0f, 3.0f), 3.0f);

      chaos::box3 b = GetBoundingBox(s);

      DrawPrimitive(ctx, s, red, false);
      DrawPrimitive(ctx, b, blue, true);
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
            DrawPrimitive(ctx, SlightDecreaseSize(split_b), red, false);
          }
        }
      }

      DrawPrimitive(ctx, b, blue, true);
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
      Draw2DCollision(ctx);

  }

  virtual bool OnDraw(glm::ivec2 size) override
  {
    glm::vec4 clear_color(0.0f, 0.0f, 0.0f, 0.0f);
    glClearBufferfv(GL_COLOR, 0, (GLfloat*)&clear_color);

    float far_plane = 1000.0f;
    glClearBufferfi(GL_DEPTH_STENCIL, 0, far_plane, 0);

    glViewport(0, 0, size.x, size.y);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);   // when viewer is inside the cube

    // XXX : the scaling is used to avoid the near plane clipping
    RenderingContext ctx;

    static float FOV = 60.0f;
    ctx.projection      = glm::perspectiveFov(FOV * (float)M_PI / 180.0f, (float)size.x, (float)size.y, 1.0f, far_plane);
    ctx.world_to_camera = fps_view_controller.GlobalToLocal();

    DrawGeometryObjects(ctx);

    debug_display.Display(size.x, size.y);

    return true;
  }

  virtual void Finalize() override
  {
    mesh_box    = nullptr;
    mesh_rect   = nullptr;
    mesh_sphere = nullptr;

    program_box    = nullptr;
    program_rect   = nullptr;
    program_sphere = nullptr;

    debug_display.Finalize();

    clock->RemoveFromParent();
  }

  boost::intrusive_ptr<chaos::GLProgram> LoadProgram(boost::filesystem::path const & resources_path, char const * ps_filename, char const * vs_filename)
  {
    chaos::GLProgramLoader loader;
    loader.AddShaderSourceFile(GL_FRAGMENT_SHADER, resources_path / ps_filename);
    loader.AddShaderSourceFile(GL_VERTEX_SHADER,   resources_path / vs_filename);

    return loader.GenerateProgramObject();
  }

  virtual bool Initialize(nlohmann::json configuration) override
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

    program_rect = LoadProgram(resources_path, "pixel_shader_rect.txt", "vertex_shader_rect.txt");
    if (program_rect == nullptr)
      return false;

    program_sphere = LoadProgram(resources_path, "pixel_shader_sphere.txt", "vertex_shader_sphere.txt");
    if (program_sphere == nullptr)
      return false;

    // create a timer
    clock = application->GetMainClock()->CreateChildClock(-1);

    // create meshes
    chaos::box3    b = chaos::box3(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(1.0f, 1.0f, 1.0f));
    chaos::sphere3 s = chaos::sphere3(glm::vec3(0.0f, 0.0f, 0.0f), 1.0f);

    chaos::MultiMeshGenerator generators;    
    generators.AddGenerator(new chaos::SphereMeshGenerator(s, 10), mesh_sphere);
    generators.AddGenerator(new chaos::CubeMeshGenerator(b), mesh_box);

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

  virtual void OnKeyEvent(int key, int scan_code, int action, int modifier) override
  {
    if (key == GLFW_KEY_T && action == GLFW_RELEASE)
    {
      chaos::MyGLFW::SingleWindowApplication * application = chaos::MyGLFW::SingleWindowApplication::GetGLFWApplicationInstance();
      if (application != nullptr)
        application->GetMainClock()->Toggle();
    }
    else if (key == GLFW_KEY_KP_ADD && action == GLFW_RELEASE)
    {
      SetExample(display_example + 1);
      DebugDisplayExampleTitle(false);
    }
    else if (key == GLFW_KEY_KP_SUBTRACT && action == GLFW_RELEASE)
    {
      SetExample(display_example - 1);
      DebugDisplayExampleTitle(false);      
    }
  }

  void SetExample(int new_display_example)
  {
    new_display_example = (new_display_example + TEST_COUNT) % TEST_COUNT;

    // reset the time
    if (clock != nullptr)
      clock->Reset();

    // restore the box position each time example change
    bigger_box  = chaos::box3(glm::vec3(3.0f, 0.0f, 0.0f), glm::vec3(5.0f, 6.0f, 7.0f));
    smaller_box = chaos::box3(glm::vec3(-3.0f, 0.0f, 0.0f), glm::vec3(2.0f, 3.0f, 4.0f));

    // restore the sphere position each time example change
    bigger_sphere  = chaos::sphere3(glm::vec3(3.0f, 0.0f, 0.0f), 7.0f);
    smaller_sphere = chaos::sphere3(glm::vec3(-3.0f, 0.0f, 0.0f), 3.0f);
  
    display_example = new_display_example;
  }

protected:

  // rendering for the box  
  boost::intrusive_ptr<chaos::SimpleMesh> mesh_box;
  boost::intrusive_ptr<chaos::GLProgram>  program_box;

  // rendering for the rect
  boost::intrusive_ptr<chaos::SimpleMesh> mesh_rect;
  boost::intrusive_ptr<chaos::GLProgram>  program_rect;

  // rendering for the rect
  boost::intrusive_ptr<chaos::SimpleMesh> mesh_sphere;
  boost::intrusive_ptr<chaos::GLProgram>  program_sphere;

  chaos::Clock * clock{ nullptr };

  chaos::box3 bigger_box;
  chaos::box3 smaller_box;

  chaos::sphere3 bigger_sphere;
  chaos::sphere3 smaller_sphere;

  int    display_example{ 0 };

  chaos::FPSViewInputController fps_view_controller;

  chaos::GLDebugOnScreenDisplay debug_display;
};

int _tmain(int argc, char ** argv, char ** env)
{
  chaos::MyGLFW::SingleWindowApplicationParams params;
  params.monitor = nullptr;
  params.width = 500;
  params.height = 500;
  params.monitor_index = 0;
  chaos::MyGLFW::RunWindowApplication<MyGLFWWindowOpenGLTest1>(argc, argv, env, params);

  return 0;
}


