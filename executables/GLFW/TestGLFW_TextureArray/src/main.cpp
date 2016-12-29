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
#include <chaos/FPSViewInputController.h>
#include <chaos/SimpleMesh.h>
#include <chaos/MultiMeshGenerator.h>
#include <chaos/GLProgramData.h>
#include <chaos/GLProgram.h>
#include <chaos/VertexDeclaration.h>
#include <chaos/GLTextureTools.h>
#include <chaos/TextureArrayGenerator.h>
#include <chaos/GLProgramVariableProvider.h>

class MyGLFWWindowOpenGLTest1 : public chaos::MyGLFWWindow
{

protected:

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
    chaos::box3 b(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(1.0f, 1.0f, 1.0f));

    static float FOV = 60.0f;
    glm::mat4 projection      = glm::perspectiveFov(FOV * (float)M_PI / 180.0f, (float)width, (float)height, 1.0f, far_plane);
    glm::mat4 world_to_camera = fps_view_controller.GlobalToLocal();
    glm::mat4 local_to_world  = glm::translate(b.position) * glm::scale(b.half_size);

    chaos::GLProgramData const & program_data = program_box->GetProgramData();

    glUseProgram(program_box->GetResourceID());

    chaos::GLProgramVariableProviderChain uniform_provider;

    uniform_provider.AddVariableValue("projection",      projection);
    uniform_provider.AddVariableValue("world_to_camera", world_to_camera);
    uniform_provider.AddVariableValue("local_to_world",  local_to_world);
    uniform_provider.AddVariableValue("texture_slice",   (float)texture_slice);
    uniform_provider.AddVariableTexture("material", texture);

    program_data.BindUniforms(&uniform_provider);
    
    mesh_box->Render(program_box->GetProgramData(), nullptr, 0, 0);

    debug_display.Display(width, height);

    return true;
  }

  virtual void Finalize() override
  {
    mesh_box    = nullptr;
    program_box = nullptr;
    texture     = nullptr;

    debug_display.Finalize();
  }

  boost::intrusive_ptr<chaos::GLProgram> LoadProgram(boost::filesystem::path const & resources_path, char const * ps_filename, char const * vs_filename)
  {
    chaos::GLProgramLoader loader;
    loader.AddShaderSourceFile(GL_FRAGMENT_SHADER, resources_path / ps_filename);
    loader.AddShaderSourceFile(GL_VERTEX_SHADER, resources_path / vs_filename);

    return loader.GenerateProgramObject();
  }


  boost::intrusive_ptr<chaos::Texture> LoadTextureArray(boost::filesystem::path const & resources_path)
  {
   chaos::TextureArrayGenerator generator;

   // fill the generator
   int i = 0;
   while (true)
   {
     std::string filename = chaos::StringTools::Printf("Brick_%d.png", i + 1);

     if (!generator.AddGenerator(chaos::ImageSliceGenerator(resources_path / filename)))
       break;
     ++i;
   }
   texture_slice_count = i;

   // generate the texture
   return generator.GenerateTexture();  
  }

  virtual bool Initialize() override
  {
    chaos::Application * application = chaos::Application::GetInstance();
    if (application == nullptr)
      return false;

    // compute resource path
    boost::filesystem::path resources_path = application->GetResourcesPath();
    boost::filesystem::path font_path = resources_path / "font.png";

    // initialize debug font display 
    chaos::GLDebugOnScreenDisplay::Params debug_params;
    debug_params.texture_path               = font_path;
    debug_params.font_characters            = " !\"#$%&'()*+,-./0123456789:;<=>?@ABCDEFGHIJKLMNOPQRSTUVWXYZ[\\]^_`abcdefghijklmnopqrstuvwxyz{|}~";
    debug_params.font_characters_per_line   = 10;
    debug_params.font_characters_line_count = 10;
    debug_params.character_width            = 20;
    debug_params.spacing                    = glm::ivec2( 0, 0);
    debug_params.crop_texture               = glm::ivec2(15, 7);

    if (!debug_display.Initialize(debug_params))
      return false;
    
    // load texture
    texture = LoadTextureArray(resources_path);
    if (texture == nullptr)
      return false;

    // load programs      
    program_box = LoadProgram(resources_path, "pixel_shader_box.txt", "vertex_shader_box.txt");
    if (program_box == nullptr)
      return false;

    // create meshes
    chaos::box3 b = chaos::box3(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(1.0f, 1.0f, 1.0f));

    chaos::MultiMeshGenerator generators;    
    generators.AddGenerator(chaos::CubeMeshGenerator(b), mesh_box);

    if (!generators.GenerateMeshes())
      return false;

    // place camera
    fps_view_controller.fps_controller.position.y = 0.0f;
    fps_view_controller.fps_controller.position.z = 10.0f;

    // initial display
    debug_display.AddLine("Draw a box with a texture array : \n  Use +/- to change slice.\n  Array composed of images with different size and BPP");

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

    fps_view_controller.Tick(glfw_window, delta_time);

    debug_display.Tick(delta_time);

    return true; // refresh
  }

  virtual void OnKeyEvent(int key, int scan_code, int action, int modifier) override
  {
    if (key == GLFW_KEY_KP_ADD && action == GLFW_RELEASE)
    {
      texture_slice = (texture_slice + 1) % texture_slice_count;
    }
    else if (key == GLFW_KEY_KP_SUBTRACT && action == GLFW_RELEASE)
    {
      texture_slice = (texture_slice - 1 + texture_slice_count) % texture_slice_count;
    }
  }

protected:

  // rendering for the box  
  boost::intrusive_ptr<chaos::SimpleMesh> mesh_box;
  boost::intrusive_ptr<chaos::GLProgram>  program_box;
  boost::intrusive_ptr<chaos::Texture>    texture;

  int texture_slice{0};
  int texture_slice_count{0};

  chaos::FPSViewInputController fps_view_controller;

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


