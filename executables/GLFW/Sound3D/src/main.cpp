#include "chaos/Chaos.h"

class WindowOpenGLTest : public chaos::Window
{
    CHAOS_DECLARE_OBJECT_CLASS(WindowOpenGLTest, chaos::Window);

protected:

  virtual bool OnDraw(chaos::GPURenderer * renderer, chaos::GPUProgramProviderInterface const * uniform_provider, chaos::WindowDrawParams const& draw_params) override
  {
    glm::vec4 clear_color(0.0f, 0.0f, 0.0f, 0.0f);
    glClearBufferfv(GL_COLOR, 0, (GLfloat*)&clear_color);

    float far_plane = 1000.0f;
    glClearBufferfi(GL_DEPTH_STENCIL, 0, far_plane, 0);

    glEnable(GL_DEPTH_TEST);
    glDisable(GL_CULL_FACE);   // when viewer is inside the cube

    // XXX : the scaling is used to avoid the near plane clipping
    static float FOV =  60.0f;
    glm::mat4 projection_matrix      = glm::perspectiveFov(FOV * (float)M_PI / 180.0f, float(draw_params.viewport.size.x), float(draw_params.viewport.size.y), 1.0f, far_plane);
    glm::mat4 local_to_world_matrix  = glm::translate(GetBoxPosition());
    glm::mat4 world_to_camera_matrix = fps_view_controller.GlobalToLocal();

    chaos::GPUProgramProviderChain main_uniform_provider(uniform_provider);
    main_uniform_provider.AddVariable("projection",      projection_matrix);
    main_uniform_provider.AddVariable("local_to_world",  local_to_world_matrix);
    main_uniform_provider.AddVariable("world_to_camera", world_to_camera_matrix);

		chaos::GPURenderParams render_params;
    mesh->DisplayWithProgram(program.get(), renderer, &main_uniform_provider, render_params);

		debug_display.Display(renderer, int(draw_params.viewport.size.x), int(draw_params.viewport.size.y));

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
		chaos::Window::Finalize();
  }

  virtual bool InitializeFromConfiguration(nlohmann::json const & config) override
  {
		if (!chaos::Window::InitializeFromConfiguration(config))
			return false;

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

    sound = sound_source->Play(desc);

    chaos::GPUProgramGenerator program_generator;
    program_generator.AddShaderSourceFile(chaos::ShaderType::FRAGMENT, resources_path / "pixel_shader_cube.txt");
    program_generator.AddShaderSourceFile(chaos::ShaderType::VERTEX,   resources_path / "vertex_shader.txt");

    program = program_generator.GenProgramObject();
    if (program == nullptr)
      return false;

    chaos::box3 b = chaos::box3(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(1.0f, 1.0f, 1.0f));

    mesh = chaos::GPUBoxMeshGenerator(b).GenerateMesh();
    if (mesh == nullptr)
      return false;

    return true;
  }

  virtual bool DoTick(float delta_time) override
  {
    if (sound_manager != nullptr)
    {
    //  glm::mat4 view = fps_view_controller.GlobalToLocal();
      glm::mat4 view = fps_view_controller.LocalToGlobal();
      sound_manager->SetListenerPosition(view[3]); // ???
      sound_manager->Tick(delta_time);
    }

    fps_view_controller.Tick(glfw_window, delta_time);

    if (rotation_started)
    {
      box_alpha += rotation_speed * delta_time;
      while (box_alpha > 2.0f * (float)M_PI)
        box_alpha -= 2.0f * (float)M_PI;
    }

    glm::vec3 pos = GetBoxPosition();
    sound->SetPosition(pos);

    debug_display.Tick(delta_time);

    return true; // refresh
  }

  virtual bool OnMouseButtonImpl(int button, int action, int modifier) override
  {
		if (button == GLFW_MOUSE_BUTTON_2 && action == GLFW_PRESS)
		{
			rotation_started = !rotation_started;
			return true;
		}
		return false;

  }

  glm::vec3 GetBoxPosition() const
  {
    return glm::vec3(
      radius * std::cos(-box_alpha),
      0.0f,
      radius * std::sin(-box_alpha)
    );
  }

protected:

  chaos::shared_ptr<chaos::SoundManager> sound_manager;
  chaos::shared_ptr<chaos::SoundSource> sound_source;
  chaos::shared_ptr<chaos::Sound> sound;

  chaos::shared_ptr<chaos::GPUProgram>  program;
  chaos::shared_ptr<chaos::GPUMesh> mesh;

  chaos::FPSViewInputController fps_view_controller;

  chaos::GLDebugOnScreenDisplay debug_display;

  bool rotation_started = true;

  float radius = 15.0f;
  float box_alpha = 0.0f;
  float rotation_speed = 0.6f;
};

int main(int argc, char ** argv, char ** env)
{
    chaos::WindowCreateParams create_params;
    create_params.monitor = nullptr;
    create_params.width = 700;
    create_params.height = 700;
    create_params.monitor_index = 0;

    return chaos::RunWindowApplication<WindowOpenGLTest>(argc, argv, env, create_params);
}