#include <chaos/StandardHeaders.h> 
#include <chaos/FileTools.h> 
#include <chaos/LogTools.h> 
#include <chaos/MyGLFWSingleWindowApplication.h> 
#include <chaos/MyGLFWWindow.h> 
#include <chaos/WinTools.h> 
#include <chaos/Application.h>
#include <chaos/IrrklangTools.h>
#include <chaos/MIDICommand.h>


// ================================================================

class MIDITimeoutEvent : public chaos::ClockEvent
{
public:

	MIDITimeoutEvent(class MyGLFWWindowOpenGLTest1 * in_application, int in_value) : application(in_application), value(in_value){}

	virtual ~MIDITimeoutEvent();

	virtual void OnEventRemovedFromClock() override;

	virtual chaos::ClockEventTickResult Tick(chaos::ClockEventTickData const & tick_data) override;

protected:

	class MyGLFWWindowOpenGLTest1 * application;

	int value;
};

// ================================================================

class MIDICommandEvent : public chaos::ClockEvent
{
public:

	MIDICommandEvent(class MyGLFWWindowOpenGLTest1 * in_application, chaos::MIDICommand const & in_command) : application(in_application) , command(in_command){}

	virtual ~MIDICommandEvent();

	virtual void OnEventRemovedFromClock() override;

	virtual chaos::ClockEventTickResult Tick(chaos::ClockEventTickData const & tick_data) override;

protected:

	class MyGLFWWindowOpenGLTest1 * application;

	chaos::MIDICommand command;
};


// ================================================================


class MyGLFWWindowOpenGLTest1 : public chaos::MyGLFW::Window
{
	friend class MIDICommandEvent;
	friend class MIDITimeoutEvent;

	int const STATE_NONE = 0;
	int const STATE_PLAYING = 1;
	int const STATE_RECORDING = 2;

	int const SILENCE_TIMER = 0;
	int const END_OF_TRACK_TIMER = 1;

	double const SILENCE_DURATION{ 1.0 };

protected:

	virtual bool OnDraw(glm::ivec2 size) override
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

	void DoRecordCommand(chaos::MIDICommand const & command)
	{
		chaos::ClockEventInfo event_info = chaos::ClockEventInfo::SingleTickEvent(0.0, chaos::ClockEventRepetitionInfo::NoRepetition());
		track_clock->AddPendingEvent(new MIDICommandEvent(this, command), event_info, true);
	}

	void AddTimeoutEvent(chaos::Clock * clock, int value, double delta_time, bool relative_time)
	{
		chaos::ClockEventInfo event_info = chaos::ClockEventInfo::SingleTickEvent(delta_time, chaos::ClockEventRepetitionInfo::NoRepetition());
		clock->AddPendingEvent(new MIDITimeoutEvent(this, value), event_info, relative_time);
	}

	void OnTimeOut(int value)
	{
		if (current_state == STATE_RECORDING)
		{
			if (value == SILENCE_TIMER)
			{
				management_clock->Reset();
				management_clock->RemoveAllPendingEvents();
				AddTimeoutEvent(track_clock.get(), END_OF_TRACK_TIMER, 0.0, true);
				track_clock->Reset();
				track_clock->EnableTickEvents(true);
				current_state = STATE_PLAYING;
				chaos::LogTools::Log("[STATE_PLAYING] : %lf", track_clock->GetClockTime());
			}
		}
		else if (current_state == STATE_PLAYING)
		{
			if (value == END_OF_TRACK_TIMER)
			{
				track_clock->Reset();
				track_clock->RemoveAllPendingEvents();
				management_clock->Reset();
				management_clock->RemoveAllPendingEvents();
				current_state = STATE_NONE;
				chaos::LogTools::Log("[STATE_NONE] : %ld" , track_clock->GetClockTime());
			}
		}
	}

	void OnCommandReceived(chaos::MIDICommand const & command)
	{
		if (command.IsSystemMessage())
			return;

		//if (!command.IsNoteOnMessage() && !command.IsNoteOffMessage())
		//		return;

		// stop the current replay
		if (current_state == STATE_PLAYING)
		{			
			track_clock->Reset();
			track_clock->RemoveAllPendingEvents();
		}
		// handle the new MIDI event
		management_clock->Reset();
		management_clock->RemoveAllPendingEvents();
		AddTimeoutEvent(management_clock.get(), SILENCE_TIMER, SILENCE_DURATION, false); 		// displace the timeout
		// record the event
		DoRecordCommand(command);
		track_clock->EnableTickEvents(false);
		current_state = STATE_RECORDING; 

		chaos::LogTools::Log("[STATE_RECORDING] : %lf", track_clock->GetClockTime());
	}

	void OnMidiInEventImpl(HMIDIIN hMidiIn, UINT wMsg, DWORD dwParam1, DWORD dwParam2)
	{
		if (wMsg == MIM_DATA)
		{
			DWORD dwMidiMessage = dwParam1;
			DWORD dwTimestamp = dwParam2; // milliseconds
			chaos::MIDICommand command((uint32_t)dwMidiMessage);			
			OnCommandReceived(command);
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

	virtual bool InitializeFromConfiguration(nlohmann::json const & config, boost::filesystem::path const & config_path) override
	{	
		chaos::Clock * clock = chaos::MyGLFW::SingleWindowApplication::GetMainClockInstance();
		if (clock == nullptr)
      return false;

		// create child clocks (BEFORE !!)
		track_clock = clock->CreateChildClock(0);
		if (track_clock == nullptr)
			return false;
		management_clock = clock->CreateChildClock(0);
		if (management_clock == nullptr)
			return false;
		// initialize MIDI's
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

		track_clock = nullptr;
		management_clock = nullptr;
	}


	virtual void TweakHints(chaos::MyGLFW::WindowHints & hints, GLFWmonitor * monitor, bool pseudo_fullscreen) const override
	{
		chaos::MyGLFW::Window::TweakHints(hints, monitor, pseudo_fullscreen);
		hints.toplevel = 0;
		hints.decorated = 1;
	}

protected:

	HMIDIOUT hMidiOutDevice{ nullptr };
	DWORD nMidiInPort{ 0 };
	HMIDIIN hMidiInDevice{ nullptr };
	DWORD nMidiOutPort{ 0 };

	int current_state{ STATE_NONE };

	boost::intrusive_ptr<chaos::Clock> track_clock;
	boost::intrusive_ptr<chaos::Clock> management_clock;
};

// ================================================================

MIDITimeoutEvent::~MIDITimeoutEvent()
{
//	chaos::LogTools::Log("~MIDICommandEvent() [%08x]\n", this);
}

void MIDITimeoutEvent::OnEventRemovedFromClock()
{
//	chaos::LogTools::Log("MIDITimeoutEvent() [%08x]\n", this);
}

chaos::ClockEventTickResult MIDITimeoutEvent::Tick(chaos::ClockEventTickData const & tick_data)
{
	application->OnTimeOut(value);
	return CompleteExecution();
}

// ================================================================

MIDICommandEvent::~MIDICommandEvent()
{
//	chaos::LogTools::Log("~MIDICommandEvent() [%08x]\n", this);
}

void MIDICommandEvent::OnEventRemovedFromClock()
{
//	chaos::LogTools::Log("OnEventRemovedFromClock() [%08x]\n", this);
}

chaos::ClockEventTickResult MIDICommandEvent::Tick(chaos::ClockEventTickData const & tick_data)
{
//	if (command.IsNoteOnMessage())
	{

		//chaos::LogTools::Log("MIDIPlaySoundEvent::Tick command = [%02x] channel = [%02x] param1 = [%02x] param2 = [%02x] param3 = [%02x]\n", command.GetCommand(), command.GetChannel(), command.param1, command.param2, command.param3);

		MMRESULT result = midiOutShortMsg(application->hMidiOutDevice, command.GetValue());
		if (result != MMSYSERR_NOERROR)
			result = result;
	}
	return ContinueExecution();
}

// ================================================================


int _tmain(int argc, char ** argv, char ** env)
{
  chaos::MyGLFW::SingleWindowApplicationParams params;
  params.monitor = nullptr;
  params.width = 500;
  params.height = 500;
  params.monitor_index = 0;
  chaos::MyGLFW::RunWindowApplication<MyGLFWWindowOpenGLTest1>(argc, argv, env, params);

	return 0;
}


