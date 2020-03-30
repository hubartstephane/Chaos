#include <death/GameWindow.h>
#include <chaos/WinTools.h>
#include <chaos/Application.h>
#include <chaos/InputMode.h>
#include <chaos/GeometryFramework.h>

#include <chaos/GPURenderParams.h>

namespace death
{
	void GameWindow::OnInputModeChanged(chaos::InputMode new_mode, chaos::InputMode old_mode)
	{
		if (game != nullptr)
			game->OnInputModeChanged(new_mode, old_mode);
	}

	bool GameWindow::OnCharEventImpl(unsigned int c)
	{
		// give inputs to the game
		if (game != nullptr)
			if (game->OnCharEvent(c))
				return true;
		return chaos::MyGLFW::Window::OnCharEventImpl(c);
	}

	bool GameWindow::OnKeyEventImpl(chaos::KeyEvent const& event)
	{
		if (chaos::MyGLFW::Window::OnKeyEventImpl(event))
			return true;




		// give inputs to the game
		if (game != nullptr)
			if (game->OnKeyEvent(event))
				return true;
		// super method

		return false;
		 

		return chaos::MyGLFW::Window::OnKeyEventImpl(event);
	}

	bool GameWindow::OnMouseButtonImpl(int button, int action, int modifier)
	{
		if (game != nullptr)
			if (game->OnMouseButton(button, action, modifier))
				return true;
		return chaos::MyGLFW::Window::OnMouseButtonImpl(button, action, modifier);
	}

	bool GameWindow::OnMouseMoveImpl(double x, double y)
	{
		if (game != nullptr)
			if (game->OnMouseMove(x, y))
				return true;
		return chaos::MyGLFW::Window::OnMouseMoveImpl(x, y);
	}

	chaos::box2 GameWindow::GetRequiredViewport(glm::ivec2 const & size) const
	{
		if (game != nullptr)
			return game->GetRequiredViewport(size);
		return chaos::MyGLFW::Window::GetRequiredViewport(size);
	}

	bool GameWindow::OnDraw(chaos::GPURenderer * renderer, chaos::box2 const & viewport, glm::ivec2 window_size)
	{
		if (game != nullptr)
		{
			chaos::GPURenderParams render_params;
			render_params.viewport = viewport;
			render_params.screen_size = window_size;
			game->Display(renderer, nullptr, render_params);
		}
		return true;
	}

	void GameWindow::Finalize()
	{
		game = nullptr;
	}

	Game * GameWindow::CreateGame()
	{
		return nullptr;
	}

	bool GameWindow::InitializeFromConfiguration(nlohmann::json const & config, boost::filesystem::path const & config_path)
	{
		if (!chaos::MyGLFW::Window::InitializeFromConfiguration(config, config_path))
			return false;
		// create the game
		game = CreateGame();
		if (game == nullptr)
			return false;
		// initialize the game
		nlohmann::json const * game_config = chaos::JSONTools::GetStructure(config, "game");
		if (game_config != nullptr)
			if (!game->InitializeFromConfiguration(*game_config, config_path))
				return false;

		return true;
	}

	void GameWindow::TweakHints(chaos::MyGLFW::WindowHints & hints, GLFWmonitor * monitor, bool pseudo_fullscreen) const
	{
		chaos::MyGLFW::Window::TweakHints(hints, monitor, pseudo_fullscreen);
#if !_DEBUG
		if (chaos::Application::HasApplicationCommandLineFlag("-UnlimitedFPS")) // CMDLINE
			hints.unlimited_fps = true;
#else 
		hints.unlimited_fps = true;
#endif

	}

	bool GameWindow::Tick(float delta_time)
	{
		if (game != nullptr)
			game->Tick(delta_time);
		return true; // refresh
	}

	void GameWindow::OnIconifiedStateChange(bool iconified)
	{
		// do not execute following code in debug because it does not fit well with debugger
#if !_DEBUG
		if (iconified)
			game->RequirePauseGame();
#endif
	}

	void GameWindow::OnFocusStateChange(bool gain_focus)
	{
		// do not execute following code in debug because it does not fit well with debugger
#if !_DEBUG
		if (!gain_focus)
			game->RequirePauseGame();
#endif
	}

}; // namespace death

