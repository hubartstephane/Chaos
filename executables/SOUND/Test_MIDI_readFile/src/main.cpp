#include <chaos/StandardHeaders.h> 
#include <chaos/FileTools.h> 
#include <chaos/LogTools.h> 
#include <chaos/MyGLFWWindow.h> 
#include <chaos/WinTools.h> 
#include <chaos/Application.h>
#include <chaos/IrrklangTools.h>
#include <chaos/MIDITools.h>

// ================================================================

class MIDICommandEvent : public chaos::ClockEvent
{
public:

	MIDICommandEvent(class MyGLFWWindowOpenGLTest1 * in_application, chaos::MIDICommand const & in_command) : application(in_application) , command(in_command){}

	virtual chaos::ClockEventTickResult Tick(chaos::ClockEventTickData const & tick_data) override;

protected:

	class MyGLFWWindowOpenGLTest1 * application;

	chaos::MIDICommand command;
};


// ================================================================


class MyGLFWWindowOpenGLTest1 : public chaos::MyGLFWWindow
{
	friend class MIDICommandEvent;

protected:

	virtual bool OnDraw(int width, int height) override
	{
		glClearColor(0.0f, 0.0, 0.0, 0.0);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		return true;
	}
	
	static void CALLBACK OnMidiInEvent(HMIDIIN hMidiIn, UINT wMsg, DWORD dwInstance, DWORD dwParam1, DWORD dwParam2)
	{
		MyGLFWWindowOpenGLTest1 * self = (MyGLFWWindowOpenGLTest1 *)dwInstance;
		self->OnMidiInEventImpl(hMidiIn, wMsg, dwParam1, dwParam2);
	}

	void OnMidiInEventImpl(HMIDIIN hMidiIn, UINT wMsg, DWORD dwParam1, DWORD dwParam2)
	{
		if (wMsg == MIM_DATA)
		{
			DWORD dwMidiMessage = dwParam1;
			DWORD dwTimestamp = dwParam2; // milliseconds

			chaos::MIDICommand command((uint32_t)dwMidiMessage);

			if (!command.IsSystemMessage())
			{
				chaos::ClockEventInfo event_info = chaos::ClockEventInfo::SingleTickEvent(1.0, chaos::ClockEventRepetitionInfo::NoRepetition());
				GetMainClock()->AddPendingEvent(new MIDICommandEvent(this, command), event_info, true);
			}
		}
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

		return true;
	}

	bool InitializeMIDIIn()
	{
		// enumerate the midi IN devices
		UINT midi_device_count = midiInGetNumDevs();
		for (UINT i = 0; i < midi_device_count; ++i)
		{
			MIDIINCAPS caps;
			midiInGetDevCaps(i, &caps, sizeof(MIDIINCAPS));

			chaos::LogTools::Log("Midi IN Device [%d] : name    = %s", i, caps.szPname);
			chaos::LogTools::Log("                    : support = %d", caps.dwSupport);
			chaos::LogTools::Log("                    : driver  = %d", caps.vDriverVersion);
			chaos::LogTools::Log("                    : mid     = %d", caps.wMid);
			chaos::LogTools::Log("                    : pid     = %d", caps.wPid);
		}

		MMRESULT rv;
		// open the device
		rv = midiInOpen(&hMidiInDevice, nMidiInPort, (DWORD_PTR)(void*)OnMidiInEvent, (DWORD_PTR)this, CALLBACK_FUNCTION);
		if (rv != MMSYSERR_NOERROR)
			return false;

		// start device usage
		midiInStart(hMidiInDevice);

		return true;
	}

	virtual bool Initialize() override
	{
		if (!InitializeMIDIIn())
			return false;
		if (!InitializeMIDIOut())
			return false;
		return true;		
	}

	void FinalizeMIDIIn()
	{
		midiInStop(hMidiInDevice);
		midiInClose(hMidiInDevice);
		hMidiInDevice = nullptr;
	}

	void FinalizeMIDIOut()
	{
		midiOutReset(hMidiOutDevice);
		midiOutClose(hMidiOutDevice);
		hMidiOutDevice = nullptr;
	}

	virtual void Finalize() override
	{
		FinalizeMIDIIn();
		FinalizeMIDIOut();
	}


	virtual void TweakSingleWindowApplicationHints(chaos::MyGLFWWindowHints & hints, GLFWmonitor * monitor, bool pseudo_fullscreen) const override
	{
		chaos::MyGLFWWindow::TweakSingleWindowApplicationHints(hints, monitor, pseudo_fullscreen);
		hints.toplevel = 0;
		hints.decorated = 1;
	}

protected:

	HMIDIOUT hMidiOutDevice{ nullptr };
	DWORD nMidiInPort{ 0 };
	HMIDIIN hMidiInDevice{ nullptr };
	DWORD nMidiOutPort{ 0 };
};

// ================================================================

chaos::ClockEventTickResult MIDICommandEvent::Tick(chaos::ClockEventTickData const & tick_data)
{

	chaos::LogTools::Log("MIDIPlaySoundEvent::Tick command = [%02x] channel = [%02x] param1 = [%02x] param2 = [%02x] param3 = [%02x]\n", command.GetCommand(), command.GetChannel(), command.param1, command.param2, command.param3);

	MMRESULT result = midiOutShortMsg(application->hMidiOutDevice, command.GetValue());
	if (result != MMSYSERR_NOERROR)
		result = result;

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


