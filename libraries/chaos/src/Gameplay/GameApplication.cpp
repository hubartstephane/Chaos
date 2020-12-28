#include <chaos/Chaos.h>

namespace chaos
{
	GameApplication::GameApplication(SubClassOf<Game> in_game_class, SubClassOf<Window> in_main_window_class, WindowParams const& in_window_params, WindowHints const & in_window_hints) :
		game_class(in_game_class),
		WindowApplication(in_main_window_class, in_window_params, in_window_hints)
	{
		assert(game_class.IsValid());
	}

	void GameApplication::OnWindowCreated(Window* window)
	{
		// super
		WindowApplication::OnWindowCreated(window);
		// set the game
		GameWindow* game_window = auto_cast(window);
		if (game_window != nullptr)
			game_window->SetGame(game.get());
	}

	bool GameApplication::PreMessageLoop()
	{
		assert(glfwGetCurrentContext() == shared_context);

		if (!WindowApplication::PreMessageLoop())
			return false;
		// create the game
		game = game_class.CreateInstance();
		if (game == nullptr)
			return false;
		// initialize the game from configuration
		nlohmann::json const* game_config = JSONTools::GetStructure(configuration, "game");
		if (game_config != nullptr)
			if (!game->InitializeFromConfiguration(*game_config, configuration_path))
				return false;
		// update all windows and give them the game
		for (auto& window : windows)
		{
			GameWindow* game_window = auto_cast(window.get());
			if (game_window != nullptr)
				game_window->SetGame(game.get());
		}




#if 0

		GLuint vertex_array = 0;
		GLuint query = 0;
		GLuint renderbuffer = 0;
		GLuint framebuffer = 0;
		
		WithGLContext<void>(shared_context, [&vertex_array , &query, &renderbuffer, &framebuffer]()
		{
			glCreateVertexArrays(1, &vertex_array);
			glCreateQueries(GL_SAMPLES_PASSED, 1, &query);
			glCreateRenderbuffers(1, &renderbuffer);
			glCreateFramebuffers(1, &framebuffer);
		});

		Window * w = CreateTypedWindow(main_window_class, window_params, window_hints);
		
		WithGLContext<void>(w->GetGLFWHandler(), [vertex_array , &query, &renderbuffer, &framebuffer]()
		{
			bool b0 = glIsVertexArray(vertex_array);  // false
			bool b1 = glIsQuery(query);               // false
			bool b2 = glIsRenderbuffer(renderbuffer); // true
			bool b3 = glIsFramebuffer(framebuffer);   // true
		
			query = query;
		});

#endif



		return true;
	}

	void GameApplication::Tick(float delta_time)
	{
		assert(glfwGetCurrentContext() == shared_context);
		// super
		WindowApplication::Tick(delta_time);
		// update the game
		if (game != nullptr)
			game->Tick(delta_time);
	}

}; // namespace chaos

