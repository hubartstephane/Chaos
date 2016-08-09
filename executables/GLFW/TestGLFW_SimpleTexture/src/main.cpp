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
#include <chaos/SimpleMeshDef.h>
#include <chaos/SkyBoxTools.h>
#include <chaos/GeometryFramework.h>

bool RECTANGLE_TEXTURE = true;

 
class MyGLFWWindowOpenGLTest1 : public chaos::MyGLFWWindow
{
public:

  MyGLFWWindowOpenGLTest1() : 
    program(0),
    mesh(nullptr){}

protected:

  virtual bool OnDraw(int width, int height) override
  {
    glm::vec4 clear_color(0.0f, 0.0f, 0.0f, 0.0f);
    glClearBufferfv(GL_COLOR, 0, (GLfloat*)&clear_color);

    float far_plane = 1000.0f;
    glClearBufferfi(GL_DEPTH_STENCIL, 0, far_plane, 0);

    glViewport(0, 0, width, height);
    glEnable(GL_DEPTH_TEST);
    glDisable(GL_CULL_FACE);   
       
    glValidateProgram(program);
    GLint validation = chaos::GLTools::CheckProgramStatus(program, GL_VALIDATE_STATUS, "Program validation failure : %s");
    
    glUseProgram(program);
      
    static float FOV =  60.0f;
    static float Z   = -20.0f;
    glm::mat4 projection      = glm::perspectiveFov(FOV * (float)M_PI / 180.0f,(float)width, (float)height, 1.0f, far_plane);
    glm::mat4 local_to_world  = glm::scale(glm::vec3(10.0f, 10.0f, 10.0f));
    glm::mat4 world_to_camera = glm::translate(glm::vec3(0.0f, 0.0f, Z));

    program_data.SetUniform("projection",      projection);
    program_data.SetUniform("local_to_world",  local_to_world);
    program_data.SetUniform("world_to_camera", world_to_camera);

    glBindTextureUnit(0, texture.texture_id);
    program_data.SetUniform("material", 0);

    mesh->Render(program_data, nullptr, 0, 0);

    return true;
  }

  virtual void Finalize() override
  {
    if (program != 0)
      glDeleteProgram(program);
    if (mesh != nullptr)
      delete(mesh);
    if (texture.texture_id != 0)
      glDeleteTextures(1, &texture.texture_id);
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
    boost::filesystem::path fragment_shader_path = resources_path / "pixel_shader_face.txt";
    boost::filesystem::path vertex_shader_path   = resources_path / "vertex_shader.txt";

    FIBITMAP * image = chaos::ImageTools::LoadImageFromFile(image_path.string().c_str());
    if (image == nullptr)
      return false;

    texture = chaos::GLTools::GenTexture(image);
    if (texture.texture_id == 0)
      return false;

    FreeImage_Unload(image);

    chaos::GLProgramLoader loader;
    loader.AddShaderSourceFile(GL_FRAGMENT_SHADER, fragment_shader_path);
    loader.AddShaderSourceFile(GL_VERTEX_SHADER,   vertex_shader_path);
    
    program = loader.GenerateProgram();
    if (program == 0)
      return false;

    program_data = chaos::GLProgramData::GetData(program);

    // create the mesh
    mesh = chaos::FaceFacingQuadMeshDef::CreateMesh(true);
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

  GLuint program;

  chaos::SimpleMesh         * mesh; 
  chaos::TextureDescription   texture;
  chaos::GLProgramData        program_data;
};



int _tmain(int argc, char ** argv, char ** env)
{

#if 0

  type_ray<float, 2>::type r;

  r = r;

  chaos::box3 bigger, smaller;

  bigger.half_size  = glm::vec3(5.0f, 6.0f, 7.0f);
  smaller.half_size = glm::vec3(2.0f, 2.0f, 5.0f);

  chaos::ForceStayInside(bigger, smaller, true);



 // auto x = b1.half_size > b2.half_size;

  glm::vec3 v(1.0f, 2.0f, 3.0f);
  v[1] = 6.0f;
  auto vv = v[1];

  return 0;
#endif

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


