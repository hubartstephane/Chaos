#include <chaos/StandardHeaders.h> 
#include <chaos/FileTools.h> 
#include <chaos/LogTools.h> 
#include <chaos/GLTools.h> 
#include <chaos/MyGLFWWindow.h> 
#include <chaos/WinTools.h> 
#include <chaos/GLProgramLoader.h>
#include <chaos/GLProgram.h>
#include <chaos/Application.h>
#include <chaos/MyGLFWFpsCamera.h>
#include <chaos/MyFbxImporter.h>
#include <chaos/GLProgramData.h>
#include <chaos/SimpleMesh.h>
#include <chaos/SimpleMeshGenerator.h>

class MyGLFWWindowOpenGLTest1 : public chaos::MyGLFWWindow
{
public:

  MyGLFWWindowOpenGLTest1() : 
    realtime(0.0){}

protected:

  virtual bool OnDraw(int width, int height) override
  {
    float     far_plane = 1000.0f;
    glm::vec4 clear_color(0.0f, 0.0f, 0.0f, 0.0f);
    glClearBufferfv(GL_COLOR, 0, (GLfloat*)&clear_color);
    glClearBufferfi(GL_DEPTH_STENCIL, 0, far_plane, 0);

    glViewport(0, 0, width, height);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);   
   
    glUseProgram(program->GetResourceID());

    // XXX : the scaling is used to avoid the near plane clipping      
    static float FOV =  60.0f;
    glm::mat4 projection_matrix = glm::perspectiveFov(FOV * (float)M_PI / 180.0f,(float)width, (float)height, 1.0f, far_plane);

    glm::mat4 local_to_world_matrix = glm::mat4(10.0f);

    glm::mat4 world_to_camera_matrix = fps_camera.GlobalToLocal();

    int instance_cube_size = 20;

    program_data.SetUniform("projection",         projection_matrix);
    program_data.SetUniform("local_to_world",     local_to_world_matrix);
    program_data.SetUniform("world_to_camera",    world_to_camera_matrix);
    program_data.SetUniform("instance_cube_size", instance_cube_size);
    program_data.SetUniform("realtime",           realtime);


    int instance_count = instance_cube_size * instance_cube_size * instance_cube_size;
    int base_instance  = 0;
    mesh->Render(program_data, nullptr, instance_count, base_instance);

    return true;
  }

  virtual void Finalize() override
  {
    program = nullptr;
    mesh    = nullptr;
  }

  virtual bool Initialize() override
  {   
    chaos::Application * application = chaos::Application::GetInstance();
    if (application == nullptr)
      return false;

    boost::filesystem::path resources_path = application->GetApplicationPath() / "resources";

    chaos::box3 b = chaos::box3(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(1.0f, 1.0f, 1.0f));

    mesh = chaos::CubeMeshGenerator(b, false).GenerateMesh(); 
    if (mesh == nullptr)
      return false;

	  // create shader
    chaos::GLProgramLoader loader;
    loader.AddShaderSourceFile(GL_FRAGMENT_SHADER, resources_path / "pixel_shader_cube.txt");
    loader.AddShaderSourceFile(GL_VERTEX_SHADER,   resources_path / "vertex_shader.txt");
    
    program = loader.GenerateProgram();
    if (program == nullptr)
      return false;

    program_data = chaos::GLProgramData::GetData(program->GetResourceID());

    fps_camera.fps_controller.position.z = 100.0f;
   
    return true;
  }

  virtual void TweakSingleWindowApplicationHints(chaos::MyGLFWWindowHints & hints, GLFWmonitor * monitor, bool pseudo_fullscreen) const override
  {
    chaos::MyGLFWWindow::TweakSingleWindowApplicationHints(hints, monitor, pseudo_fullscreen);

    hints.toplevel  = 1;
    hints.decorated = 0;
  }

  virtual bool Tick(double delta_time) override
  {
    realtime += delta_time;

    if (glfwGetKey(glfw_window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
      RequireWindowClosure();

    fps_camera.Tick(glfw_window, delta_time);

    return true; // refresh
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

  double realtime;

  boost::intrusive_ptr<chaos::GLProgram> program;
  
  boost::intrusive_ptr<chaos::SimpleMesh> mesh;

  chaos::GLProgramData program_data;

  chaos::MyGLFWFpsCamera fps_camera;
};

int _tmain(int argc, char ** argv, char ** env)
{
  chaos::Application::Initialize<chaos::Application>(argc, argv, env);

  chaos::WinTools::AllocConsoleAndRedirectStdOutput();
    
  FreeImage_Initialise(); // glew will be initialized 
  glfwInit();

  chaos::MyGLFWSingleWindowApplicationParams params;
  params.monitor       = nullptr;
  params.monitor_index = -1;
  params.width         = 800;
  params.height        = 800;
  params.monitor_index = 0;
  chaos::MyGLFWWindow::RunSingleWindowApplication<MyGLFWWindowOpenGLTest1>(params);

  glfwTerminate();
  FreeImage_DeInitialise();

  return 0;
}


