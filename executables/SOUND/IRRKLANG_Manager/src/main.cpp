#include "chaos/Chaos.h"

class WindowOpenGLTest : public chaos::Window
{
    CHAOS_DECLARE_OBJECT_CLASS(WindowOpenGLTest, chaos::Window);

protected:

  virtual bool OnDraw(chaos::GPURenderer * renderer, chaos::WindowDrawParams const& draw_params, chaos::GPUProgramProviderInterface const * uniform_provider) override
  {
    glClearColor(0.0f, 0.0, 0.0, 0.0);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    return true;
  }

  virtual void Finalize() override
  {
    sound1    = nullptr;
    category1 = nullptr;
    source1   = nullptr;

    if (sound_manager != nullptr)
    {
      sound_manager->StopManager();
      sound_manager = nullptr;
    }
		chaos::Window::Finalize();
  }

  virtual bool Tick(float delta_time) override
  {
    sound_manager->Tick(delta_time);

    return false; // no redraw
  }

  virtual bool OnMouseButtonImpl(int button, int action, int modifier) override
  {
    if (button == 0 && action == GLFW_PRESS)
    {
      chaos::PlaySoundDesc desc;
      desc.categories.push_back(category1.get());
      desc.looping = true;
      sound1 = source1->Play(desc);
			return true;
    }
    else if (button == 1 && action == GLFW_PRESS)
    {
      if (sound1 != nullptr)
      {
        chaos::BlendVolumeDesc desc;
        desc.blend_type = chaos::SoundBlendType::BLEND_OUT;
        desc.blend_time = 2.0f;
        desc.kill_at_end = true;
        sound1->StartBlend(desc, true);
      }
			return true;
    }
    else if (button == 2 && action == GLFW_PRESS)
    {
      if (category1 != nullptr)
      {
        chaos::BlendVolumeDesc desc;
        desc.blend_type = chaos::SoundBlendType::BLEND_OUT;
        desc.blend_time = 2.0f;
        desc.kill_at_end = true;
        category1->StartBlend(desc, true);
      }
			return true;
    }
		return false;
  }

  virtual bool InitializeFromConfiguration(nlohmann::json const & config) override
  {
		if (!chaos::Window::InitializeFromConfiguration(config))
			return false;

    chaos::Application * application = chaos::Application::GetInstance();
    if (application == nullptr)
      return false;

    // create the sound manager
    sound_manager = new chaos::SoundManager;
    if (sound_manager == nullptr)
      return false;

    sound_manager->StartManager();

    // create the sound
    boost::filesystem::path resources_path = application->GetApplicationPath() / "resources";
    boost::filesystem::path src1_path = resources_path / "The Pretender.ogg";

    source1 = sound_manager->AddSource(src1_path, nullptr);

    category1 = sound_manager->AddCategory(nullptr);

    return true;
  }

  virtual void TweakHints(chaos::WindowHints & hints, GLFWmonitor * monitor, bool pseudo_fullscreen) const override
  {
    chaos::Window::TweakHints(hints, monitor, pseudo_fullscreen);
    hints.toplevel = 0;
    hints.decorated = 1;
  }

protected:

  chaos::shared_ptr<chaos::SoundManager> sound_manager;

  chaos::shared_ptr<chaos::SoundSource> source1;

  chaos::shared_ptr<chaos::Sound> sound1;

  chaos::shared_ptr<chaos::SoundCategory> category1;
};












// ================================================================



int main(int argc, char ** argv, char ** env)
{
    chaos::WindowParams params;
    params.monitor = nullptr;
    params.width = 500;
    params.height = 500;
    params.monitor_index = 0;

    chaos::WindowHints hints;

    return chaos::RunWindowApplication<WindowOpenGLTest>(argc, argv, env, params, hints);
}


