#include "chaos/ChaosPCH.h"
#include "chaos/ChaosInternals.h"

namespace chaos
{
	GameApplication::GameApplication(SubClassOf<Game> in_game_class, SubClassOf<Window> in_main_window_class, SubClassOf<GameViewportWidget> in_game_viewport_widget_class, WindowCreateParams const& in_window_create_params):
		WindowApplication(in_main_window_class, in_window_create_params),
		game_class(in_game_class),
		game_viewport_widget_class(in_game_viewport_widget_class)
	{
		assert(in_game_class.IsValid());
		assert(game_viewport_widget_class.IsValid());
	}

	Window* GameApplication::CreateMainWindow()
	{
		Window* result = WindowApplication::CreateMainWindow();
		if (result != nullptr)
		{
			if (WindowRootWidget* root_widget = result->GetRootWidget())
			{
				if (GameViewportWidget* game_viewport_widget = game_viewport_widget_class.CreateInstance())
				{
					WidgetLayout layout;
					layout.aspect_ratio = 15.0f / 9.0f;
					game_viewport_widget->SetLayout(layout);
					game_viewport_widget->SetGame(game.get());

					root_widget->AddChildWidget(game_viewport_widget);
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

	bool GameApplication::OnMouseMoveImpl(glm::vec2 const& delta)
	{
		if (game != nullptr)
			if (game->OnMouseMove(delta))
				return true;
		return Application::OnMouseMoveImpl(delta);
	}

	bool GameApplication::OnMouseButtonImpl(int button, int action, int modifier)
	{
		if (game != nullptr)
			if (game->OnMouseButton(button, action, modifier))
				return true;
		return Application::OnMouseButtonImpl(button, action, modifier);
	}

	bool GameApplication::OnMouseWheelImpl(double scroll_x, double scroll_y)
	{
		if (game != nullptr)
			if (game->OnMouseWheel(scroll_x, scroll_y))
				return true;
		return Application::OnMouseWheelImpl(scroll_x, scroll_y);
	}

	bool GameApplication::OnKeyEventImpl(KeyEvent const& event)
	{
		if (game != nullptr)
			if (game->OnKeyEvent(event))
				return true;
		return Application::OnKeyEventImpl(event);
	}

	bool GameApplication::OnCharEventImpl(unsigned int c)
	{
		if (game != nullptr)
			if (game->OnCharEvent(c))
				return true;
		return Application::OnCharEventImpl(c);
	}

}; // namespace chaos

