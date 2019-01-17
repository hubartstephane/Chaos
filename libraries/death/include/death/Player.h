#pragma once

#include <chaos/StandardHeaders.h>
#include <chaos/ReferencedObject.h>
#include <chaos/MyGLFWGamepadManager.h>
#include <chaos/ParticleManager.h>


namespace death
{

	// =============================================
	// Player
	// =============================================

	class Player : public chaos::ReferencedObject
	{
		friend class Game;
		friend class GameInstance;

	public:

		/** constructor */
		Player(class GameInstance * in_game_instance);

		/** get the player allocation */
		chaos::ParticleAllocation * GetPlayerAllocation() { return player_allocations.get(); }
		/** get the player allocation */
		chaos::ParticleAllocation const * GetPlayerAllocation() const { return player_allocations.get(); }
		/** set the player allocation */
		void SetPlayerAllocation(chaos::ParticleAllocation * in_allocation);

		
		/** get the index of the player */
		int GetPlayerIndex() const { return player_index; }

		/** get the life count */
		int GetLifeCount() const { return life_count; }
		/** get the current score */
		int GetScore() const { return score; }
		/** returns whether the player is dead or not */
		bool IsDead() const { return life_count == 0; }

	protected:

		/** handle keyboard input */
		virtual bool OnKeyEvent(int key, int action);
		/** handle keyboard input */
		virtual bool OnCharEvent(unsigned int c);
		/** handle mouse input */
		virtual bool OnMouseButton(int button, int action, int modifier);
		/** handle mouse movement */
		virtual bool OnMouseMove(double x, double y);

	protected:

		/** the game instance owning the player */
		class GameInstance * game_instance = nullptr;
		/** a gamepad that can be given to the player */
		chaos::shared_ptr<chaos::MyGLFW::Gamepad> gamepad;

		/** the allocation for the player */
		chaos::shared_ptr<chaos::ParticleAllocation> player_allocations;

		/** the index of the player */
		int player_index = 0;

		/** the life for the player */
		int life_count = 3;
		/** the score for the player */
		int score = 0;
	};

}; // namespace death
