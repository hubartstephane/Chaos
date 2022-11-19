#include "chaos/ChaosPCH.h"
#include "chaos/ChaosInternals.h"

namespace chaos
{
	GameApplication::GameApplication(SubClassOf<Game> in_game_class, SubClassOf<ClientServerWindow> in_main_window_class, SubClassOf<GameWindowClient> in_window_client_class, WindowCreateParams const& in_window_create_params):
		WindowApplication(in_main_window_class, in_window_create_params),
		game_class(in_game_class),
		window_client_class(in_window_client_class)
	{
		assert(in_game_class.IsValid());
		assert(in_window_client_class.IsValid());
	}

	Window* GameApplication::CreateMainWindow()
	{
		Window* result = WindowApplication::CreateMainWindow();
		if (result != nullptr)
		{
			if (ClientServerWindow* client_server_window = auto_cast(result))
			{
				if (GameWindowClient* window_client = window_client_class.CreateInstance())
				{
					window_client->SetGame(game.get());
					client_server_window->SetClient(window_client);
				}
			}
		}
		return result;
	}

	bool GameApplication::PreMessageLoop()
	{
		assert(glfwGetCurrentContext() == shared_context);
		 
		// create the game
		game = game_class.CreateInstance();
		if (game == nullptr)
			return false;
		game->configuration_path = configuration_path;

		// initialize the game from configuration
		nlohmann::json const* game_config = JSONTools::GetStructure(configuration, "game");
		if (game_config != nullptr)
			if (!game->InitializeFromConfiguration(*game_config))
				return false;
		// super method : need to be done game initialization ! (because atlas creation requires the game to have loaded its levels)
		if (!WindowApplication::PreMessageLoop())
			return false;
		// create now some game resources that need application resources to be initialized
		if (!game->CreateGPUResources())
			return false;
		return true;
	}

	bool GameApplication::DoTick(float delta_time)
	{
		assert(glfwGetCurrentContext() == shared_context);
		// super
		WindowApplication::DoTick(delta_time);
		// update the game
		if (game != nullptr)
			game->Tick(delta_time);
		return true;
	}

	bool GameApplication::FillAtlasGeneratorInput(BitmapAtlas::AtlasInput& input)
	{
		if (!WindowApplication::FillAtlasGeneratorInput(input))
			return false;
		if (game != nullptr)
			if (!game->FillAtlasGeneratorInput(input))
				return false;
		return true;
	}

}; // namespace chaos

