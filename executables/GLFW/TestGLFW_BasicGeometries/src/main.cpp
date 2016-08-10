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

class MyGLFWWindowOpenGLTest1 : public chaos::MyGLFWWindow
{
public:

  MyGLFWWindowOpenGLTest1() :
    program(0),
    mesh(nullptr),
    realtime(0.0),
    display_example(0){}

protected:

  void DebugDisplayExampleTitle()
  {
    debug_display.Clear();
    debug_display.AddLine(chaos::StringTools::Printf("Example [%d]", display_example).c_str(), 3.0f);
  }

  void DrawBox(chaos::box3 const & b, glm::vec4 const & color)
  {
    glm::mat4 local_to_world_matrix =
      glm::translate(b.position) *
      glm::scale(b.half_size);
      
    
    program_data.SetUniform("local_to_world", local_to_world_matrix);
    program_data.SetUniform("color", color);

    mesh->Render(program_data, nullptr, 0, 0);
  }

  void DrawPoint(glm::vec3 const & p, glm::vec4 const & color)
  {
    glm::vec3 half_point_size(0.25f);
    DrawBox(chaos::box3(p, half_point_size), color);  
  }

  void DrawGeometryObjects()
  {
    glm::vec4 const red   = glm::vec4(1.0f, 0.0f, 0.0f, 1.0f);
    glm::vec4 const green = glm::vec4(0.0f, 1.0f, 0.0f, 1.0f);
    glm::vec4 const blue  = glm::vec4(0.0f, 0.0f, 1.0f, 1.0f);
    glm::vec4 const white = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);
    /*
    chaos::box3 b1(glm::vec3( 0.0f, 0.0f,  0.0f), glm::vec3(1.0f, 2.0f, 3.0f));
    chaos::box3 b2(glm::vec3(10.0f, 0.0f,  0.0f), glm::vec3(1.0f, 2.0f, 3.0f));
    chaos::box3 b3(glm::vec3( 0.0f, 0.0f, 10.0f), glm::vec3(1.0f, 2.0f, 3.0f));
    */
    
    chaos::box3 b1(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(1.0f, 1.0f, 1.0f));
    chaos::box3 b2(glm::vec3(2.0f, 0.0f, 0.0f), glm::vec3(1.0f, 1.0f, 1.0f));
    chaos::box3 b3(glm::vec3(0.0f, 0.0f, 2.0f), glm::vec3(1.0f, 1.0f, 1.0f));
    

    DrawBox(b1, red);
    DrawBox(b2, green);
    DrawBox(b3, blue);

    std::pair<glm::vec3, glm::vec3> b1_corners = b1.GetCorners();
    DrawPoint(b1_corners.first, red);
    DrawPoint(b1_corners.second, red);
    


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

    glUseProgram(program);

    // XXX : the scaling is used to avoid the near plane clipping      
    static float FOV = 60.0f;
    glm::mat4 projection_matrix      = glm::perspectiveFov(FOV * (float)M_PI / 180.0f, (float)width, (float)height, 1.0f, far_plane);
    glm::mat4 world_to_camera_matrix = fps_camera.GlobalToLocal();

    program_data.SetUniform("projection", projection_matrix);    
    program_data.SetUniform("world_to_camera", world_to_camera_matrix);

    DrawGeometryObjects();

    debug_display.Display(width, height);

    return true;
  }

  virtual void Finalize() override
  {
    if (program != 0)
      glDeleteProgram(program);
    if (mesh != nullptr)
      delete(mesh);

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
    debug_params.character_size = 30;

    if (!debug_display.Initialize(debug_params))
      return false;

    DebugDisplayExampleTitle();

    chaos::GLProgramLoader loader;
    loader.AddShaderSourceFile(GL_FRAGMENT_SHADER, resources_path / "pixel_shader_cube.txt");
    loader.AddShaderSourceFile(GL_VERTEX_SHADER, resources_path / "vertex_shader.txt");

    program = loader.GenerateProgram();
    if (program == 0)
      return false;

    program_data = chaos::GLProgramData::GetData(program);

    mesh = chaos::CubeMeshDef::CreateMesh(glm::vec3(1.0f, 1.0f, 1.0f), true);
    if (mesh == nullptr)
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
    realtime += delta_time;

    if (glfwGetKey(glfw_window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
      RequireWindowClosure();

    fps_camera.Tick(glfw_window, delta_time);

    debug_display.Tick(delta_time);

    return true; // refresh
  }

  virtual void OnMouseButton(int button, int action, int modifier) override
  {
    if ((button == 0 || button == 1) && action == GLFW_RELEASE)
    {
      display_example = display_example - (button * 2 - 1);

      display_example = (display_example + MAX_DISPLAY_EXAMPLE) % MAX_DISPLAY_EXAMPLE;

      DebugDisplayExampleTitle();
    }

    fps_camera.OnMouseButton(glfw_window, button, action, modifier);
  }

  virtual void OnMouseMove(double x, double y) override
  {
    fps_camera.OnMouseMove(glfw_window, x, y);
  }

protected:

  GLuint program;

  chaos::SimpleMesh * mesh;

  chaos::GLProgramData program_data;

  double realtime;

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
  params.width = 500;
  params.height = 500;
  params.monitor_index = 0;
  chaos::MyGLFWWindow::RunSingleWindowApplication<MyGLFWWindowOpenGLTest1>(params);

  glfwTerminate();
  FreeImage_DeInitialise();

  return 0;
}


