#pragma once

#include <chaos/StandardHeaders.h>
#include <chaos/Renderable.h>

#include <death/GameFramework.h>

namespace death
{
	class GameHUDComponent : public chaos::Renderable
	{
		DEATH_GAMEFRAMEWORK_ALLFRIENDS()

		friend class GameHUD;

	public:

		/** getters on game */
		Game * GetGame();
		/** getters on game */
		Game const * GetGame() const;

		/** getters on game instance */
		GameInstance * GetGameInstance();
		/** getters on game instance */
		GameInstance const * GetGameInstance() const;

		/** get currently played level */
		GameLevel * GetLevel();
		/** get currently played level */
		GameLevel const * GetLevel() const;

		/** get currently played level */
		GameLevelInstance * GetLevelInstance();
		/** get currently played level */
		GameLevelInstance const * GetLevelInstance() const;

		/** get the player */
		Player * GetPlayer(int player_index);
		/** get the player */
		Player const * GetPlayer(int player_index) const;

	protected:

		/** MAYBE (!!!) called whenever the hud is beeing inserted into the hud (the HUD::RegisterComponent is template function. the function below is not necessaraly been called) */
		virtual void OnInsertedInHUD();
		/** called whenever the hud is beeing removed into the hud */
		virtual void OnRemovedFromHUD();

	protected:

		/** the current hud */
		GameHUD * hud = nullptr;
	};









	class GameHUDTitleComponent : public GameHUDComponent
	{
		friend class GameHUD;

	protected:

		/** override */
		virtual void OnInsertedInHUD(int a, int b, float c);
		/** override */
		virtual void OnRemovedFromHUD() override;
	};

}; // namespace death
