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

	void DropAllSounds()
	{
		for (auto sound : playing_sounds)
			if (sound != nullptr)
				sound->stop();

		playing_sounds.clear();
	}

	virtual void Finalize() override
	{
		DropAllSounds();

		if (engine != nullptr)
		{
			if (sound_source1 != nullptr)
				engine->removeSoundSource(sound_source1.get());
			if (sound_source2 != nullptr)
				engine->removeSoundSource(sound_source2.get());
		}

		engine = nullptr;
		sound_source1 = nullptr;
		sound_source2 = nullptr;
		chaos::Window::Finalize();
	}

	virtual bool DoTick(float delta_time) override
	{
		for (size_t i = 0; i < playing_sounds.size(); ++i)
		{
			if (playing_sounds[i] == nullptr || playing_sounds[i]->isFinished())
			{
				if (i != playing_sounds.size() - 1)
					playing_sounds[i] = playing_sounds[playing_sounds.size() - 1];
				playing_sounds.pop_back();
				i--;
			}
		}
		return false; // no redraw
	}

	virtual bool OnMouseButtonImpl(int button, int action, int modifier) override
	{
		if (button == 0 && action == GLFW_PRESS)
		{
			irrklang::ISound * sound = engine->play2D(sound_source1.get(), false /* looped */, false /*  start paused */, true /* track */, true /* enable FX */);
			playing_sounds.push_back(sound);
			sound->drop();
			return true;
		}
		else if (button == 1 && action == GLFW_PRESS)
		{
			irrklang::ISound * sound = engine->play2D(sound_source2.get(), false /* looped */, false /*  start paused */, true /* track */, true /* enable FX */);
			playing_sounds.push_back(sound);
			sound->drop();
			return true;
		}
		else if (button == 2 && action == GLFW_PRESS)
		{
			DropAllSounds();
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

		// list of sound devices
		chaos::shared_ptr<irrklang::ISoundDeviceList> deviceList = irrklang::createSoundDeviceList();
		if (deviceList != nullptr)
		{
			for (int i = 0; i < deviceList->getDeviceCount(); ++i)
				printf("DEVICE %d: [%s]  [%s]\n", i, deviceList->getDeviceDescription(i), deviceList->getDeviceID(i));
			deviceList->drop();
			deviceList = nullptr;
		}

		// create the engine
		engine = irrklang::createIrrKlangDevice();
		if (engine == nullptr)
			return false;
		engine->drop(); // XXX : because we add our own reference to an object already having one

		// create the sound
		boost::filesystem::path resources_path = application->GetApplicationPath() / "resources";
		boost::filesystem::path src1_path = resources_path / "70_Stir_HiHatOpen.wav";
		boost::filesystem::path src2_path = resources_path / "70_Stir_Kick.wav";

		sound_source1 = engine->addSoundSourceFromFile(src1_path.string().c_str(), irrklang::ESM_NO_STREAMING, true);
		if (sound_source1 == nullptr)
			return false;
		// XXX :  Not from IRRKLANG   addSoundSourceFromFile(...)
		//
		// DONOT CALL  sound_source1->drop();

		sound_source2 = engine->addSoundSourceFromFile(src2_path.string().c_str(), irrklang::ESM_NO_STREAMING, true);
		if (sound_source2 == nullptr)
			return false;
		// XXX :  Not from IRRKLANG   addSoundSourceFromFile(...)
		//
		// DONOT CALL  sound_source2->drop();

		return true;
	}

protected:

	chaos::shared_ptr<irrklang::ISoundEngine> engine;
	chaos::shared_ptr<irrklang::ISoundSource> sound_source1;
	chaos::shared_ptr<irrklang::ISoundSource> sound_source2;

	std::vector<chaos::shared_ptr<irrklang::ISound>> playing_sounds;
};

int main(int argc, char ** argv, char ** env)
{
	return chaos::RunSimpleWindowApplication<WindowOpenGLTest>(argc, argv, env);
}