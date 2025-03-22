#include "chaos/ChaosPCH.h"
#include "chaos/ChaosInternals.h"

namespace chaos
{
	GameApplication::GameApplication(
		SubClassOf<Game> in_game_class,
		SubClassOf<GameWindow> in_main_window_class,
		SubClassOf<GameViewportWidget> in_game_viewport_widget_class,
		WindowPlacementInfo const& in_main_window_placement_info, 
		WindowCreateParams const& in_main_window_create_params):
			SimpleWindowApplication(in_main_window_class, in_main_window_placement_info, in_main_window_create_params),
			game_class(in_game_class),
			game_viewport_widget_class(in_game_viewport_widget_class)
	{
		assert(in_game_class.IsValid());
		assert(game_viewport_widget_class.IsValid());
	}

	void GameApplication::FinalizeGPUResources()
	{
		assert(glfwGetCurrentContext() == shared_context);

		SimpleWindowApplication::FinalizeGPUResources();
	}

	bool GameApplication::InitializeGPUResources()
	{
		assert(glfwGetCurrentContext() == shared_context);

		// create the game
		game = game_class.CreateInstance();
		if (game == nullptr)
			return false;
		GiveChildConfiguration(game.get(), "game");
		if (!game->Initialize())
			return false;
		// super method : need to be done game initialization ! (because atlas creation requires the game to have loaded its levels)
		if (!SimpleWindowApplication::InitializeGPUResources())
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
		SimpleWindowApplication::DoTick(delta_time);
		// update the game
		if (game != nullptr)
			if (!IsGameSuspended())
				game->Tick(delta_time);
		return true;
	}

	bool GameApplication::FillAtlasGeneratorInput(BitmapAtlas::AtlasInput& input)
	{
		if (!SimpleWindowApplication::FillAtlasGeneratorInput(input))
			return false;
		if (game != nullptr)
			if (!game->FillAtlasGeneratorInput(input))
				return false;
		return true;
	}

	bool GameApplication::OnMouseMoveImpl(glm::vec2 const& delta)
	{
		if (game != nullptr)
			if (!IsGameSuspended())
				if (game->OnMouseMove(delta))
					return true;
		return SimpleWindowApplication::OnMouseMoveImpl(delta);
	}

	bool GameApplication::OnMouseButtonImpl(int button, int action, int modifier)
	{
		if (game != nullptr)
			if (!IsGameSuspended())
				if (game->OnMouseButton(button, action, modifier))
					return true;
		return SimpleWindowApplication::OnMouseButtonImpl(button, action, modifier);
	}

	bool GameApplication::OnMouseWheelImpl(double scroll_x, double scroll_y)
	{
		if (game != nullptr)
			if (!IsGameSuspended())
				if (game->OnMouseWheel(scroll_x, scroll_y))
					return true;
		return SimpleWindowApplication::OnMouseWheelImpl(scroll_x, scroll_y);
	}

	bool GameApplication::OnKeyEventImpl(KeyEvent const& key_event)
	{
		if (game != nullptr)
			if (!IsGameSuspended())
				if (game->OnKeyEvent(key_event))
					return true;
		return SimpleWindowApplication::OnKeyEventImpl(key_event);
	}

	bool GameApplication::OnCharEventImpl(unsigned int c)
	{
		if (game != nullptr)
			if (!IsGameSuspended())
				if (game->OnCharEvent(c))
					return true;
		return SimpleWindowApplication::OnCharEventImpl(c);
	}

	bool GameApplication::IsGameSuspended() const
	{
		return IsImGuiMenuEnabled();
	}

}; // namespace chaos

