#include <death/GameInstance.h>

namespace death
{
	GameInstance::GameInstance(Game * in_game) :
		game(in_game)
	{
		assert(in_game != nullptr);
	}

	Player * GameInstance::GetPlayer(int player_index)
	{
		size_t count = players.size();
		for (size_t i = 0; i < count; ++i)
			if (players[i]->GetPlayerIndex() == player_index)
				return players[i].get();
		return nullptr;
	}

	Player const * GameInstance::GetPlayer(int player_index) const
	{
		size_t count = players.size();
		for (size_t i = 0; i < count; ++i)
			if (players[i]->GetPlayerIndex() == player_index)
				return players[i].get();
		return nullptr;
	}

	bool GameInstance::OnKeyEvent(int key, int action)
	{
		size_t count = players.size();
		for (size_t i = 0; i < count; ++i)
			if (players[i]->OnKeyEvent(key, action))
				return true;
		return false;
	}

	bool GameInstance::OnCharEvent(unsigned int c)
	{
		size_t count = players.size();
		for (size_t i = 0; i < count; ++i)
			if (players[i]->OnCharEvent(c))
				return true;
		return false;
	}

	bool GameInstance::OnMouseButton(int button, int action, int modifier)
	{
		size_t count = players.size();
		for (size_t i = 0; i < count; ++i)
			if (players[i]->OnMouseButton(button, action, modifier))
				return true;
		return false;
	}

	bool GameInstance::OnMouseMove(double x, double y)
	{
		size_t count = players.size();
		for (size_t i = 0; i < count; ++i)
			if (players[i]->OnMouseMove(x, y))
				return true;
		return false;
	}

	Player * GameInstance::CreatePlayer(chaos::MyGLFW::PhysicalGamepad * in_physical_gamepad)
	{
		// ensure we can cerate a new player
		size_t count = players.size();
		if (count >= GetMaxPlayerCount())
			return nullptr;
		// create the new player
		Player * result = DoCreatePlayer();
		if (result == nullptr)
			return nullptr;
		// insert the player in our list
		players.push_back(result);
		// give the physical device to the player
		result->CapturePhysicalGamepad(in_physical_gamepad);


		return result;
	}

	Player * GameInstance::GivePhysicalGamepadToPlayer(chaos::MyGLFW::PhysicalGamepad * in_physical_gamepad)
	{
		size_t count = players.size();
		for (size_t i = 0; i < count; ++i)
			if (players[i]->CapturePhysicalGamepad(in_physical_gamepad))
				return players[i].get();
		return nullptr;

	}

	bool GameInstance::DoTick(double delta_time)
	{
		size_t count = players.size();
		for (size_t i = 0; i < count; ++i)
			players[i]->Tick(delta_time);
		return true;
	}

	void GameInstance::HandlePlayerGamepadInput(double delta_time, chaos::MyGLFW::GamepadData & gpd)
	{
		// Handle the inputs as we want, modifying the object by consuming data inside
		InternalHandleGamepadInputs(delta_time, gpd);

		// give remaining input back to game instance
		if (game != nullptr && (gpd.IsAnyAction() || gpd.IsAnyAxisAction()))
			game->HandlePlayerGamepadInput(delta_time, gpd);
	}

	void GameInstance::InternalHandleGamepadInputs(double delta_time, chaos::MyGLFW::GamepadData & gpd)
	{

	}

	size_t GameInstance::GetMaxPlayerCount() const
	{
		return 1;
	}

	Player * GameInstance::DoCreatePlayer()
	{
		return new Player(this);
	}

}; // namespace death
