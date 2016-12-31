#include <chaos/StandardHeaders.h> 
#include <chaos/FileTools.h> 
#include <chaos/LogTools.h> 
#include <chaos/MyGLFWWindow.h> 
#include <chaos/WinTools.h> 
#include <chaos/Application.h>
#include <chaos/IrrklangTools.h>


//#pragma comment(lib, "winmm.lib")

class MyGLFWWindowOpenGLTest1 : public chaos::MyGLFWWindow
{

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
		switch (wMsg) {
			case MIM_OPEN:
				chaos::LogTools::Log("wMsg = MIM_OPEN\n");
				break;
			case MIM_CLOSE:
				chaos::LogTools::Log("wMsg = MIM_CLOSE\n");
				break;
			case MIM_DATA:
				chaos::LogTools::Log("wMsg = MIM_DATA, dwParam1=%08x, dwParam2=%08x\n", dwParam1, dwParam2);
				break;
			case MIM_LONGDATA:
				chaos::LogTools::Log("wMsg = MIM_LONGDATA\n");
				break;
			case MIM_ERROR:
				chaos::LogTools::Log("wMsg = MIM_ERROR\n");
				break;
			case MIM_LONGERROR:
				chaos::LogTools::Log("wMsg = MIM_LONGERROR\n");
				break;
			case MIM_MOREDATA:
				chaos::LogTools::Log("wMsg = MIM_MOREDATA\n");
				break;
			default:
				chaos::LogTools::Log("wMsg = unknown\n");
				break;
		}
		return;
	}

	virtual bool Initialize() override
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

		// open the device
		MMRESULT rv = midiInOpen(&hMidiDevice, nMidiPort, (DWORD_PTR)(void*)OnMidiInEvent, (DWORD_PTR)this, CALLBACK_FUNCTION);
		if (rv != MMSYSERR_NOERROR)
			return false;

		// start device usage
		midiInStart(hMidiDevice);

		return true;
	}

	virtual void Finalize() override
	{
		midiInStop(hMidiDevice);
		midiInClose(hMidiDevice);
		hMidiDevice = nullptr;
	}

	virtual void TweakSingleWindowApplicationHints(chaos::MyGLFWWindowHints & hints, GLFWmonitor * monitor, bool pseudo_fullscreen) const override
	{
		chaos::MyGLFWWindow::TweakSingleWindowApplicationHints(hints, monitor, pseudo_fullscreen);
		hints.toplevel = 0;
		hints.decorated = 1;
	}

protected:

	HMIDIIN hMidiDevice{ nullptr };
	DWORD nMidiPort{ 0 };
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


