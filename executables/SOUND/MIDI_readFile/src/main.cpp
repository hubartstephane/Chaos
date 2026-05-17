#include "chaos/Chaos.h"

// ================================================================


class MyWindow : public chaos::Window
{
	friend class MIDICommandEvent;

	CHAOS_DECLARE_OBJECT_CLASS(MyWindow, chaos::Window);

protected:

	virtual bool OnDraw(chaos::GPURenderContext * render_context, chaos::GPUProgramProviderInterface const * uniform_provider, chaos::WindowDrawParams const& draw_params) override
	{
		glClearColor(0.0f, 0.0, 0.0, 0.0);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		return true;
	}



	virtual bool OnInitialize(chaos::JSONReadConfiguration config) override
	{
		if (!chaos::Window::OnInitialize(config))
			return false;

		chaos::Application * application = chaos::Application::GetInstance();
		if (application == nullptr)
			return false;

		boost::filesystem::path const & resource_path = application->GetResourcesPath();

		chaos::Buffer<char> buffer = chaos::FileTools::LoadFile(resource_path / "Blues Breaker - 8 Bars" / "BluesBreaker_8Bars_01.mid");
		if (buffer == nullptr)
			return false;

		chaos::MidiLoader loader;
		return loader.LoadBuffer(buffer);
	}
};

// ================================================================


int main(int argc, char ** argv, char ** env)
{
	chaos::WindowApplicationData window_application_data;
	window_application_data.main_window_class = MyWindow::GetStaticClass();
	return chaos::RunApplication<chaos::WindowApplication>(argc, argv, env, &window_application_data);
}