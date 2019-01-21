#pragma once

#include <chaos/StandardHeaders.h>
#include <chaos/ReferencedObject.h>
#include <chaos/MyGLFWGamepadManager.h>
#include <chaos/ParticleManager.h>
#include <chaos/Tickable.h>
#include <death/Game.h>

namespace death
{

	// =============================================
	// Player
	// =============================================

	class Player : public chaos::Tickable
	{
		friend class Game;
		friend class GameInstance;
		friend class PlayerGamepadCallbacks;

	public:

		/** constructor */
		Player(class GameInstance * in_game_instance);

		/** returns the game  instance */
		class GameInstance * GetGameInstance() { return game_instance; }
		/** returns the game instance */
		class GameInstance const * GetGameInstance() const { return game_instance; }

		/** returns the game */
		Game * GetGame();
		/** returns the game */
		Game const * GetGame() const;

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

		/** try to give physical device to the player */
		bool CapturePhysicalGamepad(chaos::MyGLFW::PhysicalGamepad * in_physical_gamepad);

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

		/** handle the player input */
		virtual void HandleKeyboardInputs(double delta_time);
		/** handle the player input (top level function) */
		virtual void HandleGamepadInputs(double delta_time);
		/** handle the input for our own purpose (consuming any data) */
		virtual void InternalHandleGamepadInputs(double delta_time, chaos::MyGLFW::GamepadData & gpd);

		/** called whenever player gamepad is disconnected */
		virtual void OnGamepadDisconnected();

		/** reset the cached inputs (exists because we emulate keyboard arrow combinaison as un stick) */
		virtual void ResetCachedInputs();

		/** tick the player displacement */
		virtual void TickPlayerDisplacement(double delta_time);

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

		/** the current stick position */
		glm::vec2 left_stick_position = glm::vec2(0.0f, 0.0f);
		glm::vec2 right_stick_position = glm::vec2(0.0f, 0.0f);

	};


	// =============================================
	// A callback object for gamepad disconnection
	// =============================================

	class PlayerGamepadCallbacks : public chaos::MyGLFW::GamepadCallbacks
	{
		friend class Player;

	public:

		/** constructor */
		PlayerGamepadCallbacks(Player * in_player);
		/** override */
		virtual bool OnGamepadDisconnected(chaos::MyGLFW::Gamepad * in_gamepad) override;

	protected:

		/** player of interrest */
		Player * player = nullptr;
	};


}; // namespace death
