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
#if 0	
	static void CALLBACK OnMidiOutEvent(HMIDIIN hMidiIn, UINT wMsg, DWORD dwInstance, DWORD dwParam1, DWORD dwParam2)
	{
		MyGLFWWindowOpenGLTest1 * self = (MyGLFWWindowOpenGLTest1 *)dwInstance;
		self->OnMidiOutEventImpl(hMidiIn, wMsg, dwParam1, dwParam2);
	}

	void OnMidiOutEventImpl(HMIDIIN hMidiIn, UINT wMsg, DWORD dwParam1, DWORD dwParam2)
	{
		switch (wMsg) 		
		{
			case MIM_OPEN: // reserved, reserved
			{
				chaos::LogTools::Log("wMsg = MIM_OPEN\n");
				break;
			}
			case MIM_CLOSE: // reserved, reserved
			{
				chaos::LogTools::Log("wMsg = MIM_CLOSE\n");
				break;
			}
			case MIM_ERROR:
			{
				DWORD dwMidiMessage = dwParam1;
				DWORD dwTimestamp = dwParam2;
				chaos::LogTools::Log("wMsg = MIM_ERROR\n");
				break;
			}
			case MIM_DATA:
			{


				DWORD dwMidiMessage = dwParam1;
				DWORD dwTimestamp   = dwParam2; // milliseconds

				MIDICommand command(dwMidiMessage);

				BYTE b1 = (dwParam1 & 0xFF) >> 24;
				BYTE b2 = (dwParam1 & 0xFF) >> 16;
				BYTE b3 = (dwParam1 & 0xFF) >> 8;
				BYTE b4 = (dwParam1 & 0xFF) >> 0; // midi status

				if (b1 != 0 || b2 != 0 || b3 != 0)

					chaos::LogTools::Log("wMsg = MIM_DATA, dwParam1=%08x, dwParam2=%08x\n", dwParam1, dwParam2);
				break;
			}
			case MIM_LONGDATA:
			{
				chaos::LogTools::Log("wMsg = MIM_LONGDATA\n");
				break;
			}
			case MIM_LONGERROR:
			{
				chaos::LogTools::Log("wMsg = MIM_LONGERROR\n");
				break;
			}
			case MIM_MOREDATA:
			{
				chaos::LogTools::Log("wMsg = MIM_MOREDATA\n");
				break;
			}
			default:
			{
				chaos::LogTools::Log("wMsg = unknown\n");
				break;
			}
		}
		return;
	}
#endif

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

		MMRESULT rv;
		// open the device

		rv = midiOutOpen(&hMidiDevice, 2, 0, 0, 0); // MIDIMAPPER

		//rv = midiOutOpen(&hMidiDevice, nMidiPort, (DWORD_PTR)(void*)OnMidiOutEvent, (DWORD_PTR)this, CALLBACK_FUNCTION);
		if (rv != MMSYSERR_NOERROR)
			return false;

		chaos::ClockEventInfo event_info = chaos::ClockEventInfo(0.0, 0.0, chaos::ClockEventRepetitionInfo::InfiniteRepetition(1.0));
		GetMainClock()->AddPendingEvent(new MIDIPlaySoundEvent(this), event_info, false);

		return true;
	}

	virtual void Finalize() override
	{
		midiOutReset(hMidiDevice);
		midiOutClose(hMidiDevice);
		hMidiDevice = nullptr;		
	}

	virtual void TweakSingleWindowApplicationHints(chaos::MyGLFWWindowHints & hints, GLFWmonitor * monitor, bool pseudo_fullscreen) const override
	{
		chaos::MyGLFWWindow::TweakSingleWindowApplicationHints(hints, monitor, pseudo_fullscreen);
		hints.toplevel = 0;
		hints.decorated = 1;
	}

protected:

	HMIDIOUT hMidiDevice{ nullptr };
	DWORD nMidiPort{ 0 };
};

// ================================================================

void PlayNote(HMIDIOUT hMidiDevice, unsigned char note, unsigned char volume, unsigned char channel)
{
	chaos::MIDICommand midi_command(0x90 + channel, note, volume);
	MMRESULT result = midiOutShortMsg(hMidiDevice, midi_command.GetValue());
	if (result != MMSYSERR_NOERROR)
		result = result;
}


chaos::ClockEventTickResult MIDIPlaySoundEvent::Tick(chaos::ClockEventTickData const & tick_data)
{
	int count = GetExecutionCount();

	unsigned char note   = 60 + (count % 16);
	unsigned char volume = 60 + (count % 10) * 10;
	chaos::LogTools::Log("MIDIPlaySoundEvent::Tick note = [%d] volume = [%d]\n", note, volume);

	PlayNote(application->hMidiDevice, note, volume, 0);
	PlayNote(application->hMidiDevice, note + 1, volume, 1);




	
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


