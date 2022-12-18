#include "chaos/Chaos.h"

// ================================================================


class WindowOpenGLTest : public chaos::Window
{
	friend class MIDICommandEvent;

	CHAOS_DECLARE_OBJECT_CLASS(WindowOpenGLTest, chaos::Window);

protected:

	virtual bool OnDraw(chaos::GPURenderer * renderer, chaos::GPUProgramProviderInterface const * uniform_provider, chaos::WindowDrawParams const& draw_params) override
	{
		glClearColor(0.0f, 0.0, 0.0, 0.0);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		return true;
	}



	virtual bool InitializeFromConfiguration(nlohmann::json const & config) override
	{
		if (!chaos::Window::InitializeFromConfiguration(config))
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
	chaos::WindowCreateParams create_params;
	create_params.monitor = nullptr;
	create_params.width = 500;
	create_params.height = 500;
	create_params.monitor_index = 0;

	return chaos::RunWindowApplication<WindowOpenGLTest>(argc, argv, env, create_params);
}