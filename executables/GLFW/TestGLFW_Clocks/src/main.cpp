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
#include <chaos/SimpleMeshGenerator.h>
#include <chaos/SkyBoxTools.h>
#include <chaos/GLDebugOnScreenDisplay.h>
#include <chaos/MyGLFWFpsCamera.h>
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

protected:

  void DebugDisplayTips()
  {
    debug_display.Clear();
    debug_display.AddLine(chaos::StringTools::Printf("update clock [%d] with NUM1 & NUM2 : %f", clock1->GetClockID(), clock1->GetTimeScale()).c_str());
    debug_display.AddLine(chaos::StringTools::Printf("update clock [%d] with NUM3 & NUM4 : %f", clock2->GetClockID(), clock2->GetTimeScale()).c_str());
    debug_display.AddLine(chaos::StringTools::Printf("update clock [%d] with NUM5 & NUM6 : %f", clock3->GetClockID(), clock3->GetTimeScale()).c_str());
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

  void DrawPrimitiveImpl(RenderingContext const & ctx, chaos::SimpleMesh * mesh, chaos::GLProgram * program, glm::vec4 const & color,  glm::mat4 const & local_to_world)
  {
    glm::vec4 final_color = color;

    PrimitiveRenderingContext prim_ctx;
    prim_ctx.local_to_world = local_to_world;
    prim_ctx.color          = final_color;

    PrepareObjectProgram(program, ctx, prim_ctx);

    mesh->Render(program->GetProgramData(), nullptr, 0, 0);
  }

  void DrawPrimitive(RenderingContext const & ctx, chaos::box3 const & b, glm::vec4 const & color)
  {
    if (b.IsEmpty())
      return;

    glm::mat4 local_to_world = glm::translate(b.position) * glm::scale(b.half_size);

    DrawPrimitiveImpl(ctx, get_pointer(mesh_box), get_pointer(program_box), color, local_to_world );
  }

  void DrawGeometryObjects(RenderingContext const & ctx)
  {
    double realtime1 = clock1->GetClockTime();
    double realtime2 = clock2->GetClockTime();
    double realtime3 = clock3->GetClockTime();

    float Y1 = 5.0f * (float)chaos::MathTools::Cos(realtime1);
    float Y2 = 5.0f * (float)chaos::MathTools::Cos(realtime2);
    float Y3 = 5.0f * (float)chaos::MathTools::Cos(realtime3);

    chaos::box3 b1(glm::vec3(0.0f, Y1, 0.0f), glm::vec3(1.0f, 1.0f, 1.0f));
    chaos::box3 b2(glm::vec3(2.0f, Y2, 0.0f), glm::vec3(1.0f, 1.0f, 1.0f));
    chaos::box3 b3(glm::vec3(4.0f, Y3, 0.0f), glm::vec3(1.0f, 1.0f, 1.0f));

    DrawPrimitive(ctx, b1, red);
    DrawPrimitive(ctx, b2, green);
    DrawPrimitive(ctx, b3, blue);
  }

  virtual bool OnDraw(int width, int height) override
  {
    glm::vec4 clear_color(0.0f, 0.0f, 0.0f, 0.0f);
    glClearBufferfv(GL_COLOR, 0, (GLfloat*)&clear_color);

    float far_plane = 1000.0f;
    glClearBufferfi(GL_DEPTH_STENCIL, 0, far_plane, 0);

    glViewport(0, 0, width, height);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);   // when viewer is inside the cube

    // XXX : the scaling is used to avoid the near plane clipping
    RenderingContext ctx;

    static float FOV = 60.0f;
    ctx.projection      = glm::perspectiveFov(FOV * (float)M_PI / 180.0f, (float)width, (float)height, 1.0f, far_plane);
    ctx.world_to_camera = fps_camera.GlobalToLocal();

    DrawGeometryObjects(ctx);

    debug_display.Display(width, height);

    return true;
  }

  virtual void Finalize() override
  {
    mesh_box    = nullptr;
    program_box = nullptr;

    debug_display.Finalize();

//    GetMainClock().RemoveChildClock(clock1->GetClockID());
//    GetMainClock().RemoveChildClock(clock2->GetClockID());
//    GetMainClock().RemoveChildClock(clock3->GetClockID());
  }

  boost::intrusive_ptr<chaos::GLProgram> LoadProgram(boost::filesystem::path const & resources_path, char const * ps_filename, char const * vs_filename)
  {
    chaos::GLProgramLoader loader;
    loader.AddShaderSourceFile(GL_FRAGMENT_SHADER, resources_path / ps_filename);
    loader.AddShaderSourceFile(GL_VERTEX_SHADER,   resources_path / vs_filename);

    return loader.GenerateProgramObject();
  }

  virtual bool Initialize() override
  {
    chaos::Application * application = chaos::Application::GetInstance();
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

    // create a timer
    clock1 = GetMainClock().AddChildClock(-1);
    clock2 = clock1->AddChildClock(-1);
    clock3 = clock2->AddChildClock(-1);

    // create meshes
    chaos::box3    b = chaos::box3(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(1.0f, 1.0f, 1.0f));
    chaos::sphere3 s = chaos::sphere3(glm::vec3(0.0f, 0.0f, 0.0f), 1.0f);

    chaos::MultiMeshGenerator generators;    
    generators.AddGenerator(chaos::CubeMeshGenerator(b), mesh_box);

    if (!generators.GenerateMeshes())
      return false;

    // place camera
    fps_camera.fps_controller.position.y = 10.0f;
    fps_camera.fps_controller.position.z = 30.0f;

    // initial display
    DebugDisplayTips();

    return true;
  }

  virtual void TweakSingleWindowApplicationHints(chaos::MyGLFWWindowHints & hints, GLFWmonitor * monitor, bool pseudo_fullscreen) const override
  {
    chaos::MyGLFWWindow::TweakSingleWindowApplicationHints(hints, monitor, pseudo_fullscreen);

    hints.toplevel  = 0;
    hints.decorated = 1;
  }

  virtual bool Tick(double delta_time) override
  {
    if (glfwGetKey(glfw_window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
      RequireWindowClosure();

    fps_camera.Tick(glfw_window, delta_time);

    debug_display.Tick(delta_time);

    return true; // refresh
  }

  void UpdateClockTimeScale(chaos::Clock * clock, double offset)
  {
    if (clock != nullptr)
      clock->SetTimeScale(clock->GetTimeScale() + offset);
    DebugDisplayTips();
  }

  bool UpdateClockTimeScaleWithKeys(chaos::Clock * clock, int key, int incr_key, int decr_key, int action)
  {
    if (action == GLFW_RELEASE)
    {
      if (key == incr_key)
      {
        UpdateClockTimeScale(clock, 0.2);
        return true;
      }
      else if (key == decr_key)
      {
        UpdateClockTimeScale(clock, -0.2);
        return true;
      }
    }
    return false;
  }

  virtual void OnKeyEvent(int key, int scan_code, int action, int modifier) override
  {
    if (key == GLFW_KEY_T && action == GLFW_RELEASE)
    {
      GetMainClock().Toggle();
    }
    else
    {
      if (!UpdateClockTimeScaleWithKeys(clock1, key, GLFW_KEY_KP_1, GLFW_KEY_KP_2, action))
        if (!UpdateClockTimeScaleWithKeys(clock2, key, GLFW_KEY_KP_3, GLFW_KEY_KP_4, action))
          UpdateClockTimeScaleWithKeys(clock3, key, GLFW_KEY_KP_5, GLFW_KEY_KP_6, action);
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
  boost::intrusive_ptr<chaos::SimpleMesh> mesh_box;
  boost::intrusive_ptr<chaos::GLProgram>  program_box;

  chaos::Clock * clock1{ nullptr };
  chaos::Clock * clock2{ nullptr };
  chaos::Clock * clock3{ nullptr };

  chaos::MyGLFWFpsCamera fps_camera;

  chaos::GLDebugOnScreenDisplay debug_display;
};

int _tmain(int argc, char ** argv, char ** env)
{
  chaos::Application::Initialize<chaos::Application>(argc, argv, env);

  chaos::WinTools::AllocConsoleAndRedirectStdOutput();

  chaos::MyGLFWSingleWindowApplicationParams params;
  params.monitor = nullptr;
  params.width  = 1200;
  params.height = 600;
  params.monitor_index = 0;
  chaos::MyGLFWWindow::RunSingleWindowApplication<MyGLFWWindowOpenGLTest1>(params);

  chaos::Application::Finalize();

  return 0;
}


