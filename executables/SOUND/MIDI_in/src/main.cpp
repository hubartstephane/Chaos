#include <chaos/Chaos.h> 

class WindowOpenGLTest : public chaos::Window
{

protected:

	virtual bool OnDraw(chaos::GPURenderer * renderer, chaos::box2 const & viewport, glm::ivec2 window_size) override
	{
		glClearColor(0.0f, 0.0, 0.0, 0.0);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		return true;
	}
	
	static void CALLBACK OnMidiInEvent(HMIDIIN hMidiIn, UINT wMsg, DWORD dwInstance, DWORD dwParam1, DWORD dwParam2)
	{
		WindowOpenGLTest * self = (WindowOpenGLTest *)dwInstance;
		self->OnMidiInEventImpl(hMidiIn, wMsg, dwParam1, dwParam2);
	}

	void OnMidiInEventImpl(HMIDIIN hMidiIn, UINT wMsg, DWORD dwParam1, DWORD dwParam2)
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

				chaos::MIDICommand command((uint32_t)dwMidiMessage);

				if (command.IsSystemMessage())
					break;


				chaos::LogTools::Log("status [%02x] command [%02x] channel [%02x] param1 [%02x] param2 [%02x] param3 [%02x]\n", command.status, command.GetCommand(), command.GetChannel(), command.params[0], command.params[1], command.params[2]);
				break;
			}
			case MIM_LONGDATA:
			{
				MIDIHDR * hdr = (MIDIHDR*)dwParam1;
				DWORD dwTimestamp = dwParam2;

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
		if (!chaos::Window::InitializeFromConfiguration(config, config_path))
			return false;

		if (!InitializeMIDIIn())
			return false;
		return true;
	}

	void FinalizeMIDIIn()
	{
		midiInStop(hMidiInDevice);
		midiInClose(hMidiInDevice);
		hMidiInDevice = nullptr;
	}

	virtual void Finalize() override
	{
		FinalizeMIDIIn();
		chaos::Window::Finalize();
	}

	virtual void TweakHints(chaos::WindowHints & hints, GLFWmonitor * monitor, bool pseudo_fullscreen) const override
	{
		chaos::Window::TweakHints(hints, monitor, pseudo_fullscreen);
		hints.toplevel = 0;
		hints.decorated = 1;
	}

protected:

	HMIDIIN hMidiInDevice = nullptr;
	DWORD nMidiInPort = 0;
};

int CHAOS_MAIN(int argc, char ** argv, char ** env)
{
	int a = 83;
	int b = 0;

	int c = ((a & 127) << 7) + b;



	return 0;

	chaos::WindowParams params;
	params.monitor = nullptr;
	params.width = 500;
	params.height = 500;
	params.monitor_index = 0;

	chaos::WindowHints hints;

	return chaos::RunWindowApplication<WindowOpenGLTest>(argc, argv, env, params, hints);
}


