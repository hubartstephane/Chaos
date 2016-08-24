#include <chaos/StandardHeaders.h> 
#include <chaos/FileTools.h> 
#include <chaos/LogTools.h> 
#include <chaos/GLTools.h> 
#include <chaos/MyGLFWGamepadManager.h> 
#include <chaos/MyGLFWWindow.h> 
#include <chaos/WinTools.h> 
#include <chaos/GLProgramLoader.h>
#include <chaos/GLProgramData.h>
#include <chaos/Application.h>
#include <chaos/SimpleMeshGenerator.h>
#include <chaos/SkyBoxTools.h>
#include <chaos/GeometryFramework.h>
#include <chaos/GLProgram.h>
#include <chaos/Texture.h>

bool RECTANGLE_TEXTURE = false;

 
class MyGLFWWindowOpenGLTest1 : public chaos::MyGLFWWindow
{
public:

  MyGLFWWindowOpenGLTest1(){}

protected:

  virtual bool OnDraw(int width, int height) override
  {
    glm::vec4 clear_color(0.0f, 0.0f, 0.0f, 0.0f);
    glClearBufferfv(GL_COLOR, 0, (GLfloat*)&clear_color);

    float far_plane = 1000.0f;
    glClearBufferfi(GL_DEPTH_STENCIL, 0, far_plane, 0);

    glViewport(0, 0, width, height);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);

    glUseProgram(program->GetResourceID());
         
    glBindTextureUnit(0, texture->GetResourceID());

    chaos::GLProgramData const & program_data = program->GetProgramData();
    program_data.SetUniform("material", 0);

    mesh->Render(program_data, nullptr, 0, 0);

    return true;
  }

  virtual void Finalize() override
  {
    program = nullptr;
    mesh    = nullptr;
    texture = nullptr;
  }

  virtual bool Initialize() override
  {   
    chaos::Application * application = chaos::Application::GetInstance();
    if (application == nullptr)
      return false;

    boost::filesystem::path resources_path       = application->GetApplicationPath() / "resources";
    boost::filesystem::path image_path           = (RECTANGLE_TEXTURE)?
      resources_path / "opengl_logo_rectangle.png":
      resources_path / "opengl_logo.png";
    boost::filesystem::path fragment_shader_path = resources_path / "pixel_shader.txt";
    boost::filesystem::path vertex_shader_path   = resources_path / "vertex_shader.txt";

    texture = chaos::GLTools::GenTextureObject(image_path.string().c_str());
    if (texture == nullptr)
      return false;

    chaos::GLProgramLoader loader;
    loader.AddShaderSourceFile(GL_FRAGMENT_SHADER, fragment_shader_path);
    loader.AddShaderSourceFile(GL_VERTEX_SHADER,   vertex_shader_path);
    
    program = loader.GenerateProgramObject();
    if (program == nullptr)
      return false;

    // create the mesh
    chaos::box2 b = chaos::box2(glm::vec2(0.0f, 0.0f), glm::vec2(1.0f, 1.0f));

    mesh = chaos::QuadMeshGenerator(b).GenerateMesh();
    if (mesh == nullptr)
      return false;

    return true;
  }

  virtual void TweakSingleWindowApplicationHints(chaos::MyGLFWWindowHints & hints, GLFWmonitor * monitor, bool pseudo_fullscreen) const override
  {
    chaos::MyGLFWWindow::TweakSingleWindowApplicationHints(hints, monitor, pseudo_fullscreen);
    hints.toplevel  = 0;
    hints.decorated = 1;
  }

protected:

  boost::intrusive_ptr<chaos::GLProgram>  program;
  boost::intrusive_ptr<chaos::SimpleMesh> mesh;
  boost::intrusive_ptr<chaos::Texture>    texture;
};



int _tmain(int argc, char ** argv, char ** env)
{
  chaos::Application::Initialize<chaos::Application>(argc, argv, env);

  chaos::WinTools::AllocConsoleAndRedirectStdOutput();
    
  FreeImage_Initialise(); // glew will be initialized 
  glfwInit();

  chaos::MyGLFWSingleWindowApplicationParams params;
  params.monitor       = nullptr;
  params.width         = 500;
  params.height        = 500;
  params.monitor_index = 0;
  chaos::MyGLFWWindow::RunSingleWindowApplication<MyGLFWWindowOpenGLTest1>(params);

  glfwTerminate();
  FreeImage_DeInitialise();

  return 0;
}


