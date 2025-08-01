#include "chaos/Chaos.h"

class WindowOpenGLTest : public chaos::Window
{
    CHAOS_DECLARE_OBJECT_CLASS(WindowOpenGLTest, chaos::Window);

protected:

  virtual bool OnDraw(chaos::GPURenderContext * render_context, chaos::GPUProgramProviderInterface const * uniform_provider, chaos::WindowDrawParams const& draw_params) override
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

  virtual bool DoTick(float delta_time) override
  {
    sound_manager->Tick(delta_time);

    return false; // no redraw
  }

  virtual bool OnMouseButtonImpl(chaos::MouseButtonEvent const &mouse_button_event) override
  {
    if (mouse_button_event.IsKeyPressed(chaos::MouseButton::BUTTON_1))
    {
      chaos::PlaySoundDesc desc;
      desc.categories.push_back(category1.get());
      desc.looping = true;
      sound1 = source1->Play(desc);
			return true;
    }
    else if (mouse_button_event.IsKeyPressed(chaos::MouseButton::BUTTON_2))
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
    else if (mouse_button_event.IsKeyPressed(chaos::MouseButton::BUTTON_3))
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

  virtual bool OnInitialize(chaos::JSONReadConfiguration config) override
  {
		if (!chaos::Window::OnInitialize(config))
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

protected:

  chaos::shared_ptr<chaos::SoundManager> sound_manager;

  chaos::shared_ptr<chaos::SoundSource> source1;

  chaos::shared_ptr<chaos::Sound> sound1;

  chaos::shared_ptr<chaos::SoundCategory> category1;
};












// ================================================================



int main(int argc, char ** argv, char ** env)
{
    return chaos::RunSimpleWindowApplication<WindowOpenGLTest>(argc, argv, env);
}