#pragma once

#include <chaos/StandardHeaders.h>
#include <chaos/ReferencedObject.h>
#include <death/Player.h>


namespace death
{

	// =============================================
	// GameInstance
	// =============================================

	class GameInstance : public chaos::ReferencedObject
	{
		friend class Game;
		friend class Player;

	public:

		/** get the player by its index */
		Player * GetPlayer(int player_index);
		/** get the player by its index */
		Player const * GetPlayer(int player_index) const;


	protected:

		/** all the players present in the game */
		std::vector<chaos::shared_ptr<Player>> players;

	};

}; // namespace death
