#include <chaos/Chaos.h> 

// ================================================================

class MIDIPlaySoundEvent : public chaos::ClockEvent
{
public:

	MIDIPlaySoundEvent(class MyGLFWWindowOpenGLTest1 * in_application) : application(in_application) {}

	virtual chaos::ClockEventTickResult Tick(chaos::ClockEventTickData const & tick_data) override;

protected:

	class MyGLFWWindowOpenGLTest1 * application;
};

// ================================================================

class MyGLFWWindowOpenGLTest1 : public chaos::MyGLFW::Window
{
	friend class MIDIPlaySoundEvent;

protected:

	virtual bool OnDraw(chaos::GPURenderer * renderer, chaos::box2 const & viewport, glm::ivec2 window_size) override
	{
		glClearColor(0.0f, 0.0, 0.0, 0.0);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		return true;
	}

	bool InitializeMIDIOut()
	{
		// enumerate the midi IN devices
		UINT midi_device_count = midiOutGetNumDevs();
		for (UINT i = 0; i < midi_device_count; ++i)
		{
			MIDIOUTCAPS caps;
			midiOutGetDevCaps(i, &caps, sizeof(MIDIINCAPS));

			chaos::LogTools::Log("Midi OUT Device [%d] : name         = %s", i, caps.szPname);
			chaos::LogTools::Log("                     : support      = %d", caps.dwSupport);
			chaos::LogTools::Log("                     : driver       = %d", caps.vDriverVersion);
			chaos::LogTools::Log("                     : mid          = %d", caps.wMid);
			chaos::LogTools::Log("                     : pid          = %d", caps.wPid);
			chaos::LogTools::Log("                     : notes        = %d", caps.wNotes);
			chaos::LogTools::Log("                     : channel mask = %d", caps.wChannelMask);
			chaos::LogTools::Log("                     : technology   = %d", caps.wTechnology);
			chaos::LogTools::Log("                     : voices       = %d", caps.wVoices);
		}

		//nMidiOutPort = 0;
		//nMidiOutPort = 1;
		//nMidiOutPort = MIDIMAPPER;
		nMidiOutPort = 2;

		MMRESULT rv;
		// open the device
		rv = midiOutOpen(&hMidiOutDevice, nMidiOutPort, 0, 0, 0); // MIDIMAPPER
		if (rv != MMSYSERR_NOERROR)
			return false;

		chaos::ClockEventInfo event_info = chaos::ClockEventInfo(0.0, 0.0, chaos::ClockEventRepetitionInfo::InfiniteRepetition(0.5));

		chaos::Clock * clock = chaos::MyGLFW::SingleWindowApplication::GetMainClockInstance();
		if (clock != nullptr)
			clock->AddPendingEvent(new MIDIPlaySoundEvent(this), event_info, false);

		return true;
	}

	virtual bool InitializeFromConfiguration(nlohmann::json const & config, boost::filesystem::path const & config_path) override
	{
		if (!chaos::MyGLFW::Window::InitializeFromConfiguration(config, config_path))
			return false;

		if (!InitializeMIDIOut())
			return false;
		return true;
	}

	void FinalizeMIDIOut()
	{
		midiOutReset(hMidiOutDevice);
		midiOutClose(hMidiOutDevice);
		hMidiOutDevice = nullptr;
	}

	virtual void Finalize() override
	{
		FinalizeMIDIOut();
		chaos::MyGLFW::Window::Finalize();
	}

	virtual void TweakHints(chaos::MyGLFW::WindowHints & hints, GLFWmonitor * monitor, bool pseudo_fullscreen) const override
	{
		chaos::MyGLFW::Window::TweakHints(hints, monitor, pseudo_fullscreen);
		hints.toplevel = 0;
		hints.decorated = 1;
	}

protected:

	HMIDIOUT hMidiOutDevice = nullptr;
	DWORD nMidiOutPort = 0;
};

// ================================================================

void PlayNote(HMIDIOUT hMidiOutDevice, unsigned char note, unsigned char volume, unsigned char channel)
{
	chaos::MIDICommand midi_command(0x90 + channel, note, volume);
	MMRESULT result = midiOutShortMsg(hMidiOutDevice, midi_command.GetValue());
	if (result != MMSYSERR_NOERROR)
		result = result;
}


chaos::ClockEventTickResult MIDIPlaySoundEvent::Tick(chaos::ClockEventTickData const & tick_data)
{
	int count = GetExecutionCount();

	unsigned char note   = 60 + (count % 16);
	unsigned char volume = 60 + (count % 10) * 10;
	chaos::LogTools::Log("MIDIPlaySoundEvent::Tick note = [%d] volume = [%d]\n", note, volume);

	PlayNote(application->hMidiOutDevice, note, volume, 0);
	PlayNote(application->hMidiOutDevice, note + 1, volume, 1);

	return ContinueExecution(); 
}

// ================================================================

int CHAOS_MAIN(int argc, char ** argv, char ** env)
{
	chaos::MyGLFW::WindowParams params;
	params.monitor = nullptr;
	params.width = 500;
	params.height = 500;
	params.monitor_index = 0;

	chaos::MyGLFW::WindowHints hints;

	return chaos::MyGLFW::RunWindowApplication<MyGLFWWindowOpenGLTest1>(argc, argv, env, params, hints);
}


