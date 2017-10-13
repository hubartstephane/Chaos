#include <chaos/StandardHeaders.h> 
#include <chaos/FileTools.h> 
#include <chaos/LogTools.h> 
#include <chaos/MyGLFWWindow.h> 
#include <chaos/WinTools.h> 
#include <chaos/Application.h>
#include <chaos/IrrklangTools.h>
#include <chaos/MathTools.h>

class SoundVolumeObject : public chaos::ReferencedObject
{
public:

	/** get the own object volume */
	float GetVolume() const;
	/** get the final volume for the object (category and blendings taken into account) */
	virtual float GetEffectiveVolume() const;
	/** tick the object */
	virtual void Tick(float delta_time);
	/** whether the object is finished */
	virtual bool IsFinished() const;

	/** require the stop (destroy the object) with or without a blend time */
	void StopAndKill(float in_blendout_time);

protected:

	/** the volume */
	float volume = 1.0f;
	/** the blend out factor (whether the object is blending out) */
	float blendout_factor = 1.0f;
	float blendout_time   = 0.0f;
	/** whether the object is to be killed */
	bool should_kill = false;

};

float SoundVolumeObject::GetVolume() const
{
	return volume;
}

float SoundVolumeObject::GetEffectiveVolume() const
{
	float result = volume;
	if (blendout_time >= 0.0f && blendout_factor >= 0.0f)
		result *= blendout_factor;
	return result;
}

bool SoundVolumeObject::IsFinished() const
{
	if (blendout_time >= 0.0f && blendout_factor <= 0.0f) // the sound is being blending out
		return true;
	return false;
}

void SoundVolumeObject::Tick(float delta_time)
{
	if (blendout_time >= 0.0f)
		blendout_factor = max(0.0f, blendout_factor - delta_time / blendout_time); // blend out the object
}

void SoundVolumeObject::StopAndKill(float in_blendout_time) // the object will only be destroyed after the tick
{
	blendout_time = in_blendout_time;
	if (blendout_time <= 0.0f) // if this is the first call
	{
		blendout_factor = 1.0f;
		should_kill = true;	
	}
}









class SoundCategory : public SoundVolumeObject
{
public:



protected:
	
	/** the name of the category */
	std::string name;
};





class Sound : public SoundVolumeObject
{
public:

	/** tick the sounds */
	void Tick(float delta_time);

	/** returns true if the sound is finished */
	bool IsFinished() const;
	/** returns true whether the sound should be removed from the list at the end */
	bool ShouldKillAtEnd() const;
	/** get the final volume for the sound (category and blendings taken into account) */
	virtual float GetEffectiveVolume() const override;

	/** stop the sound */
	void Stop(double in_blendout_factor);
	/** pause the sound */


	bool IsLooping() const;
	

protected:

	/* the name */
	std::string name;
	/** the category */
	SoundCategory * category = nullptr;

	/** the looping information */
	float loop_start = -1.0f;
	float loop_end   = -1.0f;
	float loop_blend_time = 0.5f;
	/** the sound */
	boost::intrusive_ptr<irrklang::ISound> irrklang_sound;
	boost::intrusive_ptr<irrklang::ISound> irrklang_loop_sound;
};

float Sound::GetEffectiveVolume() const
{
	float result = SoundVolumeObject::GetEffectiveVolume();
	if (category != nullptr)
		result *= category->GetEffectiveVolume();
	return result;
}

bool Sound::IsLooping() const
{
	// irrklang native looping
	if (irrklang_sound->isLooped()) 
		return true;
	// our manual looping
	if (loop_start >= 0.0f && loop_start < loop_end)
	{
	
	
	}
	return false;
}

bool Sound::IsFinished() const
{
	if (SoundVolumeObject::IsFinished())
		return true;
	if (irrklang_sound->isFinished() && !IsLooping())
		return true;
	return false;
}

bool Sound::ShouldKillAtEnd() const
{
	return false;
}

void Sound::Tick(float delta_time)
{
	SoundVolumeObject::Tick(delta_time);
	if (IsFinished())
		return;

	if ()

	float current_time = irrklang_sound->getPlayPosition();

}




class SoundManager
{
public:

	bool StartManager();

	bool StopManager();

	void Tick(double delta_time);

	void StopCategory(char const * selector, double blend_time);


	void StopSound(char const * selector, double fadeout_time = 0.0);


protected:

	/** all detected devices */
	boost::intrusive_ptr<irrklang::ISoundDeviceList> irrklang_devices;
	/** the irrklank engine */
	boost::intrusive_ptr<irrklang::ISoundEngine> irrklang_engine;

	/** the categories */
	std::vector<boost::intrusive_ptr<SoundCategory>> categories;
	/** the sounds */
	std::vector<boost::intrusive_ptr<Sound>> sounds;



};

void SoundManager::Tick(double delta_time)
{
	// tick all categories


	// tick all sounds
	for (int i = sounds.size() - 1 ; i >= 0 ; --i)
	{
		Sound * sound = sounds[i].get();
		if (sound == nullptr)
			continue;
		sound->Tick(delta_time);
		if (sound->IsFinished() && sound->ShouldKillAtEnd())
		{
			if (i != sounds.size() - 1)
				sounds[i] = sounds[sounds.size() - 1]; // destroy sound that is to be destroyed
			sounds.pop_back();
		}	
	}


}

bool SoundManager::StartManager()
{
	StopManager(); // destroy previous references

				   // get the list of all devices
	irrklang_devices = irrklang::createSoundDeviceList();
	if (irrklang_devices == nullptr)
		return false;
	irrklang_devices->drop();

	// create the engine
	irrklang_engine = irrklang::createIrrKlangDevice();
	if (irrklang_engine == nullptr)
		return false;
	irrklang_engine->drop(); // suppress the extra reference


	return true;
}

bool SoundManager::StopManager()
{
	irrklang_devices = nullptr;
	irrklang_engine  = nullptr;

	return true;
}



class MyGLFWWindowOpenGLTest1 : public chaos::MyGLFW::Window
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

	virtual void TweakSingleWindowApplicationHints(chaos::MyGLFW::WindowHints & hints, GLFWmonitor * monitor, bool pseudo_fullscreen) const override
	{
		chaos::MyGLFW::Window::TweakSingleWindowApplicationHints(hints, monitor, pseudo_fullscreen);
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

	chaos::MyGLFW::SingleWindowApplicationParams params;
	params.monitor = nullptr;
	params.width = 500;
	params.height = 500;
	params.monitor_index = 0;
	chaos::MyGLFW::Window::RunSingleWindowApplication<MyGLFWWindowOpenGLTest1>(params);

	return 0;
}


