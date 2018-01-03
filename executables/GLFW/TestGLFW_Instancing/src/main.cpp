#include <chaos/StandardHeaders.h> 
#include <chaos/FileTools.h> 
#include <chaos/LogTools.h> 
#include <chaos/GLTools.h> 
#include <chaos/MyGLFWSingleWindowApplication.h> 
#include <chaos/MyGLFWWindow.h> 
#include <chaos/WinTools.h> 
#include <chaos/GLProgramLoader.h>
#include <chaos/GLProgram.h>
#include <chaos/Application.h>
#include <chaos/FPSViewInputController.h>
#include <chaos/MyFbxImporter.h>
#include <chaos/GLProgramData.h>
#include <chaos/SimpleMesh.h>
#include <chaos/SimpleMeshGenerator.h>
#include <chaos/GLProgramVariableProvider.h>

#include <libavutil/imgutils.h>
#include <libavutil/parseutils.h>
#include <libswscale/swscale.h>

class MyGLFWWindowOpenGLTest1 : public chaos::MyGLFW::Window
{

protected:

  virtual bool OnDraw(glm::ivec2 size) override
  {
    float     far_plane = 1000.0f;
    glm::vec4 clear_color(0.0f, 0.0f, 0.0f, 0.0f);
    glClearBufferfv(GL_COLOR, 0, (GLfloat*)&clear_color);
    glClearBufferfi(GL_DEPTH_STENCIL, 0, far_plane, 0);

    glViewport(0, 0, size.x, size.y);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);

    // XXX : the scaling is used to avoid the near plane clipping      
    static float FOV = 60.0f;
    glm::mat4 projection_matrix = glm::perspectiveFov(FOV * (float)M_PI / 180.0f, (float)size.x, (float)size.y, 1.0f, far_plane);

    glm::mat4 local_to_world_matrix = glm::mat4(10.0f);

    glm::mat4 world_to_camera_matrix = fps_view_controller.GlobalToLocal();

    int instance_cube_size = 20;

    double realtime = 0.0;

    chaos::MyGLFW::SingleWindowApplication * application = chaos::MyGLFW::SingleWindowApplication::GetGLFWApplicationInstance();
    if (application != nullptr)
      application->GetMainClock()->GetClockTime();

    chaos::GLProgramVariableProviderChain uniform_provider;
    uniform_provider.AddVariableValue("projection", projection_matrix);
    uniform_provider.AddVariableValue("local_to_world", local_to_world_matrix);
    uniform_provider.AddVariableValue("world_to_camera", world_to_camera_matrix);
    uniform_provider.AddVariableValue("instance_cube_size", instance_cube_size);
    uniform_provider.AddVariableValue("realtime", realtime);

    int instance_count = instance_cube_size * instance_cube_size * instance_cube_size;
    int base_instance = 0;
    mesh->Render(program.get(), &uniform_provider, instance_count, base_instance);

    return true;
  }

  virtual void Finalize() override
  {
    program = nullptr;
    mesh = nullptr;
  }

  virtual bool Initialize(nlohmann::json const & configuration) override
  {
    int w = 0;
    int h = 0;
    av_parse_video_size(&w, &h, "toto");




    chaos::Application * application = chaos::Application::GetInstance();
    if (application == nullptr)
      return false;

    boost::filesystem::path resources_path = application->GetResourcesPath();

    chaos::box3 b = chaos::box3(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(1.0f, 1.0f, 1.0f));

    mesh = chaos::CubeMeshGenerator(b).GenerateMesh();
    if (mesh == nullptr)
      return false;

    // create shader
    chaos::GLProgramLoader loader;
    loader.AddShaderSourceFile(GL_FRAGMENT_SHADER, resources_path / "pixel_shader_cube.txt");
    loader.AddShaderSourceFile(GL_VERTEX_SHADER, resources_path / "vertex_shader.txt");

    program = loader.GenerateProgramObject();
    if (program == nullptr)
      return false;

    fps_view_controller.fps_controller.position.z = 100.0f;

    return true;
  }

  virtual void TweakHints(chaos::MyGLFW::WindowHints & hints, GLFWmonitor * monitor, bool pseudo_fullscreen) const override
  {
    chaos::MyGLFW::Window::TweakHints(hints, monitor, pseudo_fullscreen);

    hints.toplevel = 1;
    hints.decorated = 0;
  }

  virtual bool Tick(double delta_time) override
  {
    if (glfwGetKey(glfw_window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
      RequireWindowClosure();

    fps_view_controller.Tick(glfw_window, delta_time);

    return true; // refresh
  }

protected:

  boost::intrusive_ptr<chaos::GLProgram> program;

  boost::intrusive_ptr<chaos::SimpleMesh> mesh;

  chaos::FPSViewInputController fps_view_controller;
};

int _tmain(int argc, char ** argv, char ** env)
{
  chaos::MyGLFW::SingleWindowApplicationParams params;
  params.monitor = nullptr;
  params.width = 800;
  params.height = 800;
  params.monitor_index = 0;
  chaos::MyGLFW::RunWindowApplication<MyGLFWWindowOpenGLTest1>(argc, argv, env, params);

  return 0;
}


