#pragma once

#include <chaos/StandardHeaders.h>
#include <chaos/ReferencedObject.h>
#include <chaos/Tickable.h>
#include <death/Player.h>
#include <death/Game.h>

namespace death
{

	// =============================================
	// GameInstance
	// =============================================

	class GameInstance : public chaos::Tickable
	{
		friend class Game;
		friend class Player;

	public:

		/** constructor */
		GameInstance(Game * in_game);

		/** get the player by its index */
		Player * GetPlayer(int player_index);
		/** get the player by its index */
		Player const * GetPlayer(int player_index) const;

		/** returns the game */
		Game * GetGame() { return game; }
		/** returns the game */
		Game const * GetGame() const { return game; }

		/** create one player and give it the gamepad provided if any */
		Player * CreatePlayer(chaos::MyGLFW::PhysicalGamepad * in_physical_gamepad);

		/** try to give a physical to any player (returns the player) */
		Player * GivePhysicalGamepadToPlayer(chaos::MyGLFW::PhysicalGamepad * in_physical_gamepad);


	protected:

		/** override */
		virtual bool DoTick(double delta_time) override;

		/** handle keyboard input */
		virtual bool OnKeyEvent(int key, int action);
		/** handle keyboard input */
		virtual bool OnCharEvent(unsigned int c);
		/** handle mouse input */
		virtual bool OnMouseButton(int button, int action, int modifier);
		/** handle mouse movement */
		virtual bool OnMouseMove(double x, double y);

		/** returns the maximum number of player */
		size_t GetMaxPlayerCount() const;

		/** return a new player */
		virtual Player * DoCreatePlayer();

	protected:

		/** the game */
		Game * game = nullptr;

		/** all the players present in the game */
		std::vector<chaos::shared_ptr<Player>> players;

	};

}; // namespace death
