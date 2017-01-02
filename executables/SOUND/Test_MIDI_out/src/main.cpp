#include <chaos/StandardHeaders.h> 
#include <chaos/FileTools.h> 
#include <chaos/LogTools.h> 
#include <chaos/MyGLFWWindow.h> 
#include <chaos/WinTools.h> 
#include <chaos/Application.h>
#include <chaos/IrrklangTools.h>

#include <chaos/ClockManager.h>
#include <chaos/MIDITools.h>



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

class MyGLFWWindowOpenGLTest1 : public chaos::MyGLFWWindow
{
	friend class MIDIPlaySoundEvent;

protected:

	virtual bool OnDraw(int width, int height) override
	{
		glClearColor(0.0f, 0.0, 0.0, 0.0);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		return true;
	}

	virtual bool Initialize() override
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

		//nMidiPort = 0;
		//nMidiPort = 1;
		//nMidiPort = MIDIMAPPER;
		nMidiPort = 2;

		MMRESULT rv;
		// open the device
		rv = midiOutOpen(&hMidiOutDevice, nMidiPort, 0, 0, 0); // MIDIMAPPER
		if (rv != MMSYSERR_NOERROR)
			return false;

		chaos::ClockEventInfo event_info = chaos::ClockEventInfo(0.0, 0.0, chaos::ClockEventRepetitionInfo::InfiniteRepetition(0.5));
		GetMainClock()->AddPendingEvent(new MIDIPlaySoundEvent(this), event_info, false);

		return true;
	}

	virtual void Finalize() override
	{
		midiOutReset(hMidiOutDevice);
		midiOutClose(hMidiOutDevice);
		hMidiOutDevice = nullptr;
	}

	virtual void TweakSingleWindowApplicationHints(chaos::MyGLFWWindowHints & hints, GLFWmonitor * monitor, bool pseudo_fullscreen) const override
	{
		chaos::MyGLFWWindow::TweakSingleWindowApplicationHints(hints, monitor, pseudo_fullscreen);
		hints.toplevel = 0;
		hints.decorated = 1;
	}

protected:

	HMIDIOUT hMidiOutDevice{ nullptr };
	DWORD nMidiPort{ 0 };
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


