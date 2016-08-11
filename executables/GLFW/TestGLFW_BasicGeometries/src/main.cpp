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
#include <chaos/SimpleMeshDef.h>
#include <chaos/SkyBoxTools.h>
#include <chaos/GLDebugOnScreenDisplay.h>
#include <chaos/MyGLFWFpsCamera.h>
#include <chaos/SimpleMesh.h>
#include <chaos/GLProgramData.h>
#include <chaos/VertexDeclaration.h>

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


class MyGLFWWindowOpenGLTest1 : public chaos::MyGLFWWindow
{
public:

  MyGLFWWindowOpenGLTest1() :
    program_box(0),
    mesh_box(nullptr),

    program_rect(0),
    mesh_rect(nullptr),

    program_sphere(0),
    mesh_sphere(nullptr),

    realtime(0.0),
    time_scale(1.0),
    display_example(0){}

protected:

  template<typename T, int dimension>
  chaos::type_box<T, dimension> SlightIncreaseSize(chaos::type_box<T, dimension> src) const
  {
    src.half_size *= static_cast<T>(1.01);
    return src;  
  }

  char const * GetExampleTitle(int example)
  {
    if (example == 0)
      return "boxes touch each others";
    if (example == 1)
      return "box.GetCorner(...)";
    if (example == 2)
      return "construct box from corners";
    if (example == 3)
      return "box intersection";
    if (example == 4)
      return "box union";
  
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

  void PrepareObjectProgram(GLuint program, chaos::GLProgramData & program_data, RenderingContext const & ctx, PrimitiveRenderingContext const & prim_ctx)
  {
    glUseProgram(program);
    program_data.SetUniform("projection", ctx.projection);    
    program_data.SetUniform("world_to_camera", ctx.world_to_camera);
    program_data.SetUniform("local_to_world", prim_ctx.local_to_world);
    program_data.SetUniform("color", prim_ctx.color);  
  }

  void DrawBox(RenderingContext const & ctx, chaos::box2 const & b, glm::vec4 const & color)
  {
    if (b.IsEmpty())
      return;

    PrimitiveRenderingContext prim_ctx;
    prim_ctx.local_to_world = glm::translate(glm::vec3(b.position.x, b.position.y, 0.0f)) * glm::scale(glm::vec3(b.half_size.x, b.half_size.y, 1.0f));
    prim_ctx.color = color;

    PrepareObjectProgram(program_rect, program_rect_data, ctx, prim_ctx);

    mesh_rect->Render(program_rect_data, nullptr, 0, 0);
  }

  void DrawBox(RenderingContext const & ctx, chaos::box3 const & b, glm::vec4 const & color)
  {
    if (b.IsEmpty())
      return;

    PrimitiveRenderingContext prim_ctx;
    prim_ctx.local_to_world  = glm::translate(b.position) * glm::scale(b.half_size);
    prim_ctx.color           = color;

    PrepareObjectProgram(program_box, program_box_data, ctx, prim_ctx);

    mesh_box->Render(program_box_data, nullptr, 0, 0);
  }

  void DrawPoint(RenderingContext const & ctx, glm::vec3 const & p, glm::vec4 const & color)
  {
    glm::vec3 half_point_size(0.125f);
    DrawBox(ctx, chaos::box3(p, half_point_size), color);  
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

  void DrawGeometryObjects(RenderingContext const & ctx)
  {
    glm::vec4 const red   = glm::vec4(1.0f, 0.0f, 0.0f, 1.0f);
    glm::vec4 const green = glm::vec4(0.0f, 1.0f, 0.0f, 1.0f);
    glm::vec4 const blue  = glm::vec4(0.0f, 0.0f, 1.0f, 1.0f);
    glm::vec4 const white = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);

    glm::vec4 const solid       = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);
    glm::vec4 const translucent = glm::vec4(1.0f, 1.0f, 1.0f, 0.3f);

    // ensure box touch alltogether
    if (display_example == 0)
    {
      chaos::box3 b1(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(1.0f, 1.0f, 1.0f));
      chaos::box3 b2(glm::vec3(2.0f, 0.0f, 0.0f), glm::vec3(1.0f, 1.0f, 1.0f));
      chaos::box3 b3(glm::vec3(0.0f, 0.0f, 2.0f), glm::vec3(1.0f, 1.0f, 1.0f));

      DrawBox(ctx, b1, red);
      DrawBox(ctx, b2, green);
      DrawBox(ctx, b3, blue);
    }

    // display box and corners
    if (display_example == 1)
    {
      chaos::box3 b(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(1.0f, 1.0f, 1.0f));

      DrawBox(ctx, b, red);

      std::pair<glm::vec3, glm::vec3> corners = b.GetCorners();
      DrawPoint(ctx, corners.first, white);
      DrawPoint(ctx, corners.second, white);
    }

    // box construction from corners
    if (display_example == 2)
    {
      glm::vec3 p1(0.0f, 0.0f, 0.0f);
      glm::vec3 p2(1.0f, 2.0f, 3.0f);

      chaos::box3 b(std::make_pair(p1, p2));

      DrawBox(ctx, b, red);
      DrawPoint(ctx, p1, white);
      DrawPoint(ctx, p2, white);
    }

    // box union or intersection
    if (display_example == 3 || display_example == 4)
    {
      chaos::box3 b1(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(1.0f, 2.0f, 3.0f));
      chaos::box3 b2(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(3.0f, 1.0f, 2.0f));

      b1.position.x = 5.0f * (float)chaos::MathTools::Cos(3.0 * realtime * M_2_PI);
      b2.position.y = 5.0f * (float)chaos::MathTools::Cos(4.0 * realtime * M_2_PI);

      if (display_example == 3)
      {
        DrawBox(ctx, SlightIncreaseSize(b1 & b2), white * solid);

        BeginTranslucency();        
        DrawBox(ctx, b1, red  * translucent);
        DrawBox(ctx, b2, blue * translucent);
        EndTranslucency();
      }
      else
      {
        DrawBox(ctx, b1, red  * solid);
        DrawBox(ctx, b2, blue * solid);

        BeginTranslucency();        
        DrawBox(ctx, SlightIncreaseSize(b1 | b2), white * translucent);
        EndTranslucency();        
      }
    }






  }

  virtual bool OnDraw(int width, int height) override
  {
    glm::vec4 clear_color(0.0f, 0.0f, 0.0f, 0.0f);
    glClearBufferfv(GL_COLOR, 0, (GLfloat*)&clear_color);

    float far_plane = 1000.0f;
    glClearBufferfi(GL_DEPTH_STENCIL, 0, far_plane, 0);

    glViewport(0, 0, width, height);
    glEnable(GL_DEPTH_TEST);
    glDisable(GL_CULL_FACE);   // when viewer is inside the cube

    // XXX : the scaling is used to avoid the near plane clipping
    RenderingContext ctx;

    static float FOV = 60.0f;
    ctx.projection      = glm::perspectiveFov(FOV * (float)M_PI / 180.0f, (float)width, (float)height, 1.0f, far_plane);
    ctx.world_to_camera = fps_camera.GlobalToLocal();

    DrawGeometryObjects(ctx);

    debug_display.Display(width, height);

    return true;
  }

  void FinalizeMeshAndProgram(chaos::SimpleMesh * mesh, GLuint program)
  {
    if (program != 0)
      glDeleteProgram(program);
    if (mesh != nullptr)
      delete(mesh);  
  }

  virtual void Finalize() override
  {
    FinalizeMeshAndProgram(mesh_box,    program_box);
    FinalizeMeshAndProgram(mesh_rect,   program_rect);
    FinalizeMeshAndProgram(mesh_sphere, program_sphere);

    debug_display.Finalize();
  }

  GLuint LoadProgram(boost::filesystem::path const & resources_path, char const * ps_filename, char const * vs_filename, chaos::GLProgramData & program_data)
  {
    chaos::GLProgramLoader loader;
    loader.AddShaderSourceFile(GL_FRAGMENT_SHADER, resources_path / ps_filename);
    loader.AddShaderSourceFile(GL_VERTEX_SHADER,   resources_path / vs_filename);

    GLuint result = loader.GenerateProgram();
    if (result != 0)
      program_data = chaos::GLProgramData::GetData(result);

    return result;
  }

  virtual bool Initialize() override
  {
    chaos::Application * application = chaos::Application::GetInstance();
    if (application == nullptr)
      return false;

    // compute resource path
    boost::filesystem::path resources_path = application->GetApplicationPath() / "resources";
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
    program_box = LoadProgram(resources_path, "pixel_shader_box.txt", "vertex_shader_box.txt", program_box_data);
    if (program_box == 0)
      return false;

    program_rect = LoadProgram(resources_path, "pixel_shader_rect.txt", "vertex_shader_rect.txt", program_rect_data);
    if (program_rect == 0)
      return false;

    program_sphere = LoadProgram(resources_path, "pixel_shader_sphere.txt", "vertex_shader_sphere.txt", program_sphere_data);
    if (program_sphere == 0)
      return false;

    // create meshes
    mesh_box = chaos::CubeMeshDef::CreateMesh(glm::vec3(1.0f, 1.0f, 1.0f), true);
    if (mesh_box == nullptr)
      return false;





    // place camera
    fps_camera.fps_controller.position.y =  3.0f;
    fps_camera.fps_controller.position.z = 10.0f;

    // initial display
    DebugDisplayExampleTitle(true);

    return true;
  }

  virtual void TweakSingleWindowApplicationHints(chaos::MyGLFWWindowHints & hints, GLFWmonitor * monitor, bool pseudo_fullscreen) const override
  {
    chaos::MyGLFWWindow::TweakSingleWindowApplicationHints(hints, monitor, pseudo_fullscreen);

    hints.toplevel = 1;
    hints.decorated = 1;
  }

  virtual bool Tick(double delta_time) override
  {
    realtime += delta_time * time_scale;

    if (glfwGetKey(glfw_window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
      RequireWindowClosure();

    fps_camera.Tick(glfw_window, delta_time);

    debug_display.Tick(delta_time);

    return true; // refresh
  }

  virtual void OnKeyEvent(int key, int scan_code, int action, int modifier) override
  {
    if (key == GLFW_KEY_T && action == GLFW_RELEASE)
    {
      time_scale = 1.0 - time_scale;
    }
    else if (key == GLFW_KEY_KP_ADD && action == GLFW_RELEASE)
    {
      if (GetExampleTitle(display_example + 1) != nullptr)
      {
        ++display_example;
        DebugDisplayExampleTitle(false);   
      }
    }
    else if (key == GLFW_KEY_KP_SUBTRACT && action == GLFW_RELEASE)
    {
      if (display_example > 0)
      {
        --display_example;
        DebugDisplayExampleTitle(false);      
      }
    }
  }

  virtual void OnMouseButton(int button, int action, int modifier) override
  {
    fps_camera.OnMouseButton(glfw_window, button, action, modifier);
  }

  virtual void OnMouseMove(double x, double y) override
  {
    fps_camera.OnMouseMove(glfw_window, x, y);
  }

protected:

  // rendering for the box
  GLuint               program_box;
  chaos::GLProgramData program_box_data;
  chaos::SimpleMesh  * mesh_box;

  // rendering for the rect
  GLuint               program_rect;
  chaos::GLProgramData program_rect_data;
  chaos::SimpleMesh  * mesh_rect;

  // rendering for the rect
  GLuint               program_sphere;
  chaos::GLProgramData program_sphere_data;
  chaos::SimpleMesh  * mesh_sphere;
  

  double realtime;

  double time_scale;

  int    display_example;

  chaos::MyGLFWFpsCamera fps_camera;

  chaos::GLDebugOnScreenDisplay debug_display;
};

int _tmain(int argc, char ** argv, char ** env)
{
  chaos::Application::Initialize<chaos::Application>(argc, argv, env);

  chaos::WinTools::AllocConsoleAndRedirectStdOutput();

  FreeImage_Initialise(); // glew will be initialized 
  glfwInit();

  chaos::MyGLFWSingleWindowApplicationParams params;
  params.monitor = nullptr;
  params.width  = 1000;
  params.height = 600;
  params.monitor_index = 0;
  chaos::MyGLFWWindow::RunSingleWindowApplication<MyGLFWWindowOpenGLTest1>(params);

  glfwTerminate();
  FreeImage_DeInitialise();

  return 0;
}


