#include "chaos/ChaosPCH.h"
#include "chaos/ChaosInternals.h"

namespace chaos
{
	GameApplication::GameApplication(SubClassOf<Game> in_game_class):
			game_class(in_game_class)
	{
		assert(in_game_class.IsValid());
	}

	void GameApplication::FinalizeGPUResources()
	{
		assert(glfwGetCurrentContext() == shared_context);

		if (game != nullptr)
		{
			game->Finalize();
			game = nullptr;
		}
		WindowApplication::FinalizeGPUResources();
	}

	bool GameApplication::InitializeGPUResources(JSONReadConfiguration config)
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
		if (!WindowApplication::InitializeGPUResources(config))
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
			if (!IsGameSuspended())
				game->Tick(delta_time);
		return true;
	}

	bool GameApplication::FillAtlasGeneratorInput(AtlasInput& input)
	{
		if (!WindowApplication::FillAtlasGeneratorInput(input))
			return false;
		if (game != nullptr)
			if (!game->FillAtlasGeneratorInput(input))
				return false;
		return true;
	}

	bool GameApplication::TraverseInputEventReceiverHierarchy(LightweightFunction<bool(InputEventReceiverInterface*)> event_func)
	{
		if (game != nullptr)
			if (!IsGameSuspended())
				if (game->TraverseInputEventReceiverHierarchy(event_func))
					return true;
		return WindowApplication::TraverseInputEventReceiverHierarchy(event_func);
	}

	bool GameApplication::IsGameSuspended() const
	{
		return IsImGuiMenuEnabled();
	}

}; // namespace chaos

