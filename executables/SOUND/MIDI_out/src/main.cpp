#include "chaos/Chaos.h"

// ================================================================

class MIDIPlaySoundEvent : public chaos::ClockEvent
{
public:

	MIDIPlaySoundEvent(class WindowOpenGLTest * in_application) : application(in_application) {}

	virtual chaos::ClockEventTickResult Tick(chaos::ClockEventTickData const & tick_data) override;

protected:

	class WindowOpenGLTest * application;
};

// ================================================================

class WindowOpenGLTest : public chaos::Window
{
	friend class MIDIPlaySoundEvent;

	CHAOS_DECLARE_OBJECT_CLASS(WindowOpenGLTest, chaos::Window);

protected:

	virtual bool OnDraw(chaos::GPURenderer * renderer, chaos::GPUProgramProviderInterface const * uniform_provider, chaos::WindowDrawParams const& draw_params) override
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

			chaos::Log::Message("Midi OUT Device [%d] : name         = %s", i, caps.szPname);
			chaos::Log::Message("                     : support      = %d", caps.dwSupport);
			chaos::Log::Message("                     : driver       = %d", caps.vDriverVersion);
			chaos::Log::Message("                     : mid          = %d", caps.wMid);
			chaos::Log::Message("                     : pid          = %d", caps.wPid);
			chaos::Log::Message("                     : notes        = %d", caps.wNotes);
			chaos::Log::Message("                     : channel mask = %d", caps.wChannelMask);
			chaos::Log::Message("                     : technology   = %d", caps.wTechnology);
			chaos::Log::Message("                     : voices       = %d", caps.wVoices);
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

		chaos::Clock * clock = chaos::WindowApplication::GetMainClockInstance();
		if (clock != nullptr)
			clock->AddPendingEvent(new MIDIPlaySoundEvent(this), event_info, false);

		return true;
	}

	virtual bool InitializeFromConfiguration(nlohmann::json const * config) override
	{
		if (!chaos::Window::InitializeFromConfiguration(config))
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
		chaos::Window::Finalize();
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
	chaos::Log::Message("MIDIPlaySoundEvent::Tick note = [%d] volume = [%d]\n", note, volume);

	PlayNote(application->hMidiOutDevice, note, volume, 0);
	PlayNote(application->hMidiOutDevice, note + 1, volume, 1);

	return ContinueExecution();
}

// ================================================================

int main(int argc, char ** argv, char ** env)
{
	chaos::WindowCreateParams create_params;
	create_params.monitor = nullptr;
	create_params.width = 500;
	create_params.height = 500;
	create_params.monitor_index = 0;

	return chaos::RunWindowApplication<WindowOpenGLTest>(argc, argv, env, create_params);
}