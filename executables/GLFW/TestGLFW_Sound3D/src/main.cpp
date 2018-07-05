#include <chaos/StandardHeaders.h> 
#include <chaos/FileTools.h> 
#include <chaos/LogTools.h> 
#include <chaos/GLTools.h> 
#include <chaos/GLTextureTools.h>
#include <chaos/MyGLFWGamepadManager.h> 
#include <chaos/MyGLFWSingleWindowApplication.h> 
#include <chaos/MyGLFWWindow.h> 
#include <chaos/WinTools.h> 
#include <chaos/GPUProgramGenerator.h>
#include <chaos/Application.h>
#include <chaos/SimpleMeshGenerator.h>
#include <chaos/SkyBoxTools.h>
#include <chaos/GLDebugOnScreenDisplay.h>
#include <chaos/FPSViewInputController.h>
#include <chaos/SimpleMesh.h>
#include <chaos/GPUProgramData.h>
#include <chaos/GPUProgram.h>
#include <chaos/GPUTexture.h>
#include <chaos/GPUVertexDeclaration.h>
#include <chaos/GPUProgramProvider.h>
#include <chaos/SoundManager.h>

class MyGLFWWindowOpenGLTest1 : public chaos::MyGLFW::Window
{

protected:

  virtual void OnKeyEvent(int key, int scan_code, int action, int modifier) override
  {

  }

  virtual bool OnDraw(glm::ivec2 size) override
  {
    glm::vec4 clear_color(0.0f, 0.0f, 0.0f, 0.0f);
    glClearBufferfv(GL_COLOR, 0, (GLfloat*)&clear_color);

    float far_plane = 1000.0f;
    glClearBufferfi(GL_DEPTH_STENCIL, 0, far_plane, 0);

    glViewport(0, 0, size.x, size.y);
    glEnable(GL_DEPTH_TEST);
    glDisable(GL_CULL_FACE);   // when viewer is inside the cube
   
    // XXX : the scaling is used to avoid the near plane clipping      
    static float FOV =  60.0f;
    glm::mat4 projection_matrix      = glm::perspectiveFov(FOV * (float)M_PI / 180.0f,(float)size.x, (float)size.y, 1.0f, far_plane);
    glm::mat4 local_to_world_matrix  = glm::translate(GetBoxPosition());
    glm::mat4 world_to_camera_matrix = fps_view_controller.GlobalToLocal();
      
    chaos::GPUProgramProvider uniform_provider;
    uniform_provider.AddVariableValue("projection",      projection_matrix);
    uniform_provider.AddVariableValue("local_to_world",  local_to_world_matrix);
    uniform_provider.AddVariableValue("world_to_camera", world_to_camera_matrix);

    mesh->Render(program.get(), &uniform_provider);

    debug_display.Display(size.x, size.y);    

    return true;
  }

  virtual void Finalize() override
  {
    sound_manager = nullptr;
    sound_source  = nullptr;
    sound         = nullptr;
    program       = nullptr;
    mesh          = nullptr;

    debug_display.Finalize();
  }

  virtual bool InitializeFromConfiguration(nlohmann::json const & config, boost::filesystem::path const & config_path) override
  {   
    chaos::Application * application = chaos::Application::GetInstance();
    if (application == nullptr)
      return false;

    boost::filesystem::path resources_path = application->GetResourcesPath();
    boost::filesystem::path image_path     = resources_path / "font.png";

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

    sound_manager = new chaos::SoundManager;
    if (sound_manager == nullptr)
      return false;
    if (!sound_manager->StartManager())
      return false;

    sound_source = sound_manager->AddSource(resources_path / "Tom.wav");
    if (sound_source == nullptr)
      return false;

    chaos::PlaySoundDesc desc;
    desc.looping = true;
    desc.SetPosition(GetBoxPosition());

    sound = sound_source->PlaySound(desc);

    chaos::GPUProgramGenerator program_generator;
    program_generator.AddShaderSourceFile(GL_FRAGMENT_SHADER, resources_path / "pixel_shader_cube.txt");
    program_generator.AddShaderSourceFile(GL_VERTEX_SHADER,   resources_path / "vertex_shader.txt");
    
    program = program_generator.GenProgramObject();
    if (program == nullptr)
      return false;

    chaos::box3 b = chaos::box3(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(1.0f, 1.0f, 1.0f));

    mesh = chaos::CubeMeshGenerator(b).GenerateMesh(); 
    if (mesh == nullptr)
      return false;
   
    return true;
  }

  virtual void TweakHints(chaos::MyGLFW::WindowHints & hints, GLFWmonitor * monitor, bool pseudo_fullscreen) const override
  {
    chaos::MyGLFW::Window::TweakHints(hints, monitor, pseudo_fullscreen);

    hints.toplevel  = 1;
    hints.decorated = 1;
  }

  virtual bool Tick(double delta_time) override
  {
    if (glfwGetKey(glfw_window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
      RequireWindowClosure();

    if (sound_manager != nullptr)
    {
    //  glm::mat4 view = fps_view_controller.GlobalToLocal();
      glm::mat4 view = fps_view_controller.LocalToGlobal();
      sound_manager->SetListenerPosition(view);
      sound_manager->Tick((float)delta_time);
    }

    fps_view_controller.Tick(glfw_window, delta_time);

    if (rotation_started)
    {
      box_alpha += rotation_speed * (float)delta_time;
      while (box_alpha > 2.0f * (float)M_PI)
        box_alpha -= 2.0f * (float)M_PI;
    }

    glm::vec3 pos = GetBoxPosition();
    sound->SetPosition(pos);

    debug_display.Tick(delta_time);
    
    return true; // refresh
  }

  virtual void OnMouseButton(int button, int action, int modifier) override
  {
    if (button == GLFW_MOUSE_BUTTON_2 && action == GLFW_PRESS)
      rotation_started = !rotation_started;

  }

  glm::vec3 GetBoxPosition() const
  {
    return glm::vec3(
      radius * chaos::MathTools::Cos(-box_alpha),
      0.0f,
      radius * chaos::MathTools::Sin(-box_alpha)
    );
  }

protected:

  boost::intrusive_ptr<chaos::SoundManager> sound_manager;
  boost::intrusive_ptr<chaos::SoundSource> sound_source;
  boost::intrusive_ptr<chaos::Sound> sound;
  
  boost::intrusive_ptr<chaos::GPUProgram>  program;
  boost::intrusive_ptr<chaos::SimpleMesh> mesh;
 
  chaos::FPSViewInputController fps_view_controller;

  chaos::GLDebugOnScreenDisplay debug_display;

  bool rotation_started = true;

  float radius = 15.0f;
  float box_alpha = 0.0f;
  float rotation_speed = 0.6f;
};

int _tmain(int argc, char ** argv, char ** env)
{
  chaos::MyGLFW::SingleWindowApplicationParams params;
  params.monitor = nullptr;
  params.width = 700;
  params.height = 700;
  params.monitor_index = 0;
  chaos::MyGLFW::RunWindowApplication<MyGLFWWindowOpenGLTest1>(argc, argv, env, params);

  return 0;
}


