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

static const int MAX_DISPLAY_EXAMPLE = 5;

class RenderingContext
{
public:

  glm::mat4 projection_matrix;
  glm::mat4 world_to_camera_matrix;
};


class MyGLFWWindowOpenGLTest1 : public chaos::MyGLFWWindow
{
public:

  MyGLFWWindowOpenGLTest1() :
    program_box(0),
    mesh_box(nullptr),
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
      //debug_display.AddLine("");
    }
    debug_display.AddLine(chaos::StringTools::Printf("=> Example %d : %s", display_example, GetExampleTitle(display_example)).c_str());
  }

  void PrepareObjectProgram(GLuint program, chaos::GLProgramData & program_data, RenderingContext const & ctx, )
  {
    glUseProgram(program);
    program_data.SetUniform("projection", ctx.projection_matrix);    
    program_data.SetUniform("world_to_camera", ctx.world_to_camera_matrix);

    program_data.SetUniform("local_to_world", local_to_world_matrix);
    program_data.SetUniform("color", color);
  
  }

  void DrawBox(RenderingContext const & ctx, chaos::box3 const & b, glm::vec4 const & color)
  {
    if (b.IsEmpty())
      return;

    PrepareObjectProgram(program_box, program_box_data, ctx, color);


    glm::mat4 local_to_world_matrix =
      glm::translate(b.position) *
      glm::scale(b.half_size);
         
    mesh_box->Render(program_box_data, nullptr, 0, 0);
  }

  void DrawPoint(RenderingContext const & ctx, glm::vec3 const & p, glm::vec4 const & color)
  {
    glm::vec3 half_point_size(0.125f);
    DrawBox(ctx, chaos::box3(p, half_point_size), color);  
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

        glEnable(GL_BLEND);
        glDepthMask(GL_FALSE);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        
        DrawBox(ctx, b1, red  * translucent);
        DrawBox(ctx, b2, blue * translucent);

        glDepthMask(GL_TRUE);
        glDisable(GL_BLEND);
      }
      else
      {
        DrawBox(ctx, b1, red  * solid);
        DrawBox(ctx, b2, blue * solid);

        glEnable(GL_BLEND);
        glDepthMask(GL_FALSE);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        
        DrawBox(ctx, SlightIncreaseSize(b1 | b2), white * translucent);
        
        glDepthMask(GL_TRUE);
        glDisable(GL_BLEND);
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
    ctx.projection_matrix      = glm::perspectiveFov(FOV * (float)M_PI / 180.0f, (float)width, (float)height, 1.0f, far_plane);
    ctx.world_to_camera_matrix = fps_camera.GlobalToLocal();

    DrawGeometryObjects(ctx);

    debug_display.Display(width, height);

    return true;
  }

  virtual void Finalize() override
  {
    if (program_box != 0)
      glDeleteProgram(program_box);
    if (mesh_box != nullptr)
      delete(mesh_box);

    debug_display.Finalize();
  }

  virtual bool Initialize() override
  {
    chaos::Application * application = chaos::Application::GetInstance();
    if (application == nullptr)
      return false;

    boost::filesystem::path resources_path = application->GetApplicationPath() / "resources";
    boost::filesystem::path image_path = resources_path / "font.png";

    chaos::GLDebugOnScreenDisplay::Params debug_params;
    debug_params.texture_path = image_path;
    debug_params.font_characters = " !\"#$%&'()*+,-./0123456789:;<=>?@ABCDEFGHIJKLMNOPQRSTUVWXYZ[\\]^_`abcdefghijklmnopqrstuvwxyz{|}~";
    debug_params.font_characters_per_line = 10;
    debug_params.character_size           = 30;
    debug_params.horiz_spacing            = -15;
    debug_params.vert_spacing             = -10;

    if (!debug_display.Initialize(debug_params))
      return false;
    
    DebugDisplayExampleTitle(true);
   
    chaos::GLProgramLoader loader;
    loader.AddShaderSourceFile(GL_FRAGMENT_SHADER, resources_path / "pixel_shader_cube.txt");
    loader.AddShaderSourceFile(GL_VERTEX_SHADER, resources_path / "vertex_shader.txt");

    program_box = loader.GenerateProgram();
    if (program_box == 0)
      return false;

    program_box_data = chaos::GLProgramData::GetData(program_box);

    mesh_box = chaos::CubeMeshDef::CreateMesh(glm::vec3(1.0f, 1.0f, 1.0f), true);
    if (mesh_box == nullptr)
      return false;

    fps_camera.fps_controller.position.y =  3.0f;
    fps_camera.fps_controller.position.z = 10.0f;

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
      display_example = display_example + 1;
      display_example = (display_example + MAX_DISPLAY_EXAMPLE) % MAX_DISPLAY_EXAMPLE;

      DebugDisplayExampleTitle(false);
    }
    else if (key == GLFW_KEY_KP_SUBTRACT && action == GLFW_RELEASE)
    {
      display_example = display_example - 1;
      display_example = (display_example + MAX_DISPLAY_EXAMPLE) % MAX_DISPLAY_EXAMPLE;

      DebugDisplayExampleTitle(false);
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

  // rendering for the square
  

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


