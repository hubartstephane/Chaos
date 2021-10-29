#include <chaos/Chaos.h>

#include "Ludum40SpriteManager.h"
#include "Ludum40Game.h"

// ======================================================================================

class WindowOpenGLTest : public chaos::Window
{
	CHAOS_DECLARE_OBJECT_CLASS(WindowOpenGLTest, chaos::Window);

	static constexpr float VIEWPORT_WANTED_ASPECT = (16.0f / 9.0f);

protected:

	virtual bool OnKeyEventImpl(chaos::KeyEvent const & event) override
	{
		if (game != nullptr)
			if (game->OnKeyEvent(event.key, event.action))
				return true;
		return chaos::Window::OnKeyEventImpl(event);
	}

	virtual bool OnDraw(chaos::GPURenderer * renderer, chaos::box2 const & viewport, glm::ivec2 window_size) override
	{
		glm::vec4 clear_color(0.0f, 0.0f, 0.0f, 0.0f);
		glClearBufferfv(GL_COLOR, 0, (GLfloat*)&clear_color);

		float far_plane = 1000.0f;
		glClearBufferfi(GL_DEPTH_STENCIL, 0, far_plane, 0);

		glEnable(GL_DEPTH_TEST);
		glDisable(GL_CULL_FACE);   // when viewer is inside the cube

		float world_x = 1000.0f;
		float world_y = world_x / VIEWPORT_WANTED_ASPECT;

		if (game != nullptr)
			game->Display(renderer, glm::vec2(world_x, world_y));

		return true;
	}

	virtual void Finalize() override
	{
		game = nullptr;
		chaos::Window::Finalize();
	}

	virtual bool InitializeFromConfiguration(nlohmann::json const & config) override
	{   
		if (!chaos::Window::InitializeFromConfiguration(config))
			return false;

		// open user temp directory and dump the config file
		chaos::Application * application = chaos::Application::GetInstance();
		if (application != nullptr)
		{
			boost::filesystem::path user_temp = application->CreateUserLocalTempDirectory();
#if _DEBUG
			chaos::WinTools::ShowFile(user_temp);
#endif
		}

		game = new Game;
		if (game == nullptr)
			return false;

		float WORLD_X = 1000.0f;
		glm::vec2 world_size = glm::vec2(WORLD_X, WORLD_X / VIEWPORT_WANTED_ASPECT);
		if (!game->Initialize(this, chaos::JSONTools::GetStructure(config, "game"), world_size))
			return false;

		return true;
	}

	virtual void TweakHints(chaos::WindowHints & hints, GLFWmonitor * monitor, bool pseudo_fullscreen) const override
	{
		chaos::Window::TweakHints(hints, monitor, pseudo_fullscreen);

#if !_DEBUG
		hints.toplevel  = 1;
#endif
		hints.decorated = 1;
	}

	virtual bool Tick(float delta_time) override
	{
		game->Tick(delta_time);

		return true; // refresh
	}

protected:

	chaos::shared_ptr<Game> game;
};

int CHAOS_MAIN(int argc, char ** argv, char ** env)
{
	chaos::WindowParams params;
	params.monitor = nullptr;
	params.width = 500;
	params.height = 500;
	params.monitor_index = 0;

	chaos::WindowHints hints;

	return chaos::RunWindowApplication<WindowOpenGLTest>(argc, argv, env, params, hints);
}


