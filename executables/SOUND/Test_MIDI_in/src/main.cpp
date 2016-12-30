#include <chaos/StandardHeaders.h> 
#include <chaos/FileTools.h> 
#include <chaos/LogTools.h> 
#include <chaos/MyGLFWWindow.h> 
#include <chaos/WinTools.h> 
#include <chaos/Application.h>
#include <chaos/IrrklangTools.h>

class MyGLFWWindowOpenGLTest1 : public chaos::MyGLFWWindow
{

protected:

	virtual bool OnDraw(int width, int height) override
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
		engine = nullptr;
		sound_source1 = nullptr;
		sound_source2 = nullptr;
	}

	// XXX : when we call addSoundSourceFromFile(...), no reference is add to the ISoundSource
	// 
	//   => calling both 
	//           sound_source1->drop();   
	//        and 
	//           engine->drop();   
	//
	// provoke a double-deletion => crash
	//


	virtual bool Tick(double delta_time) override
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

	virtual void OnMouseButton(int button, int action, int modifier) override
	{
		if (button == 0 && action == GLFW_PRESS)
		{
			irrklang::ISound * sound = engine->play2D(sound_source1.get(), false /* looped */, false /*  start paused */, true /* track */, true /* enable FX */);
			playing_sounds.push_back(sound);
		}
		else if (button == 1 && action == GLFW_PRESS)
		{
			irrklang::ISound * sound = engine->play2D(sound_source2.get(), false /* looped */, false /*  start paused */, true /* track */, true /* enable FX */);
			playing_sounds.push_back(sound);
		}
		else if (button == 2 && action == GLFW_PRESS)
			DropAllSounds();
	}

	virtual bool Initialize() override
	{


		/*
		MMRESULT midiInOpen(
			LPHMIDIIN lphMidiIn,
			UINT      uDeviceID,
			DWORD_PTR dwCallback,
			DWORD_PTR dwCallbackInstance,
			DWORD     dwFlags
		);
		*/




		chaos::Application * application = chaos::Application::GetInstance();
		if (application == nullptr)
			return false;

		// list of sound devices
		boost::intrusive_ptr<irrklang::ISoundDeviceList> deviceList = irrklang::createSoundDeviceList();
		if (deviceList != nullptr)
		{
			for (int i = 0; i < deviceList->getDeviceCount(); ++i)
				printf("DEVICE %d: [%s]  [%s]\n", i, deviceList->getDeviceDescription(i), deviceList->getDeviceID(i));
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

		sound_source2 = engine->addSoundSourceFromFile(src2_path.string().c_str(), irrklang::ESM_NO_STREAMING, true);
		if (sound_source2 == nullptr)
			return false;

		return true;
	}

	virtual void TweakSingleWindowApplicationHints(chaos::MyGLFWWindowHints & hints, GLFWmonitor * monitor, bool pseudo_fullscreen) const override
	{
		chaos::MyGLFWWindow::TweakSingleWindowApplicationHints(hints, monitor, pseudo_fullscreen);
		hints.toplevel = 0;
		hints.decorated = 1;
	}

protected:

	boost::intrusive_ptr<irrklang::ISoundEngine> engine;
	boost::intrusive_ptr<irrklang::ISoundSource> sound_source1;
	boost::intrusive_ptr<irrklang::ISoundSource> sound_source2;

	std::vector<boost::intrusive_ptr<irrklang::ISound>> playing_sounds;
};

int _tmain(int argc, char ** argv, char ** env)
{
	chaos::Application::Initialize<chaos::Application>(argc, argv, env);

	chaos::WinTools::AllocConsoleAndRedirectStdOutput();

	chaos::MyGLFWSingleWindowApplicationParams params;
	params.monitor = nullptr;
	params.width = 500;
	params.height = 500;
	params.monitor_index = 0;
	chaos::MyGLFWWindow::RunSingleWindowApplication<MyGLFWWindowOpenGLTest1>(params);

	return 0;
}


