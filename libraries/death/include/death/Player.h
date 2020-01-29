#pragma once

#include <chaos/StandardHeaders.h>
#include <chaos/ReferencedObject.h>
#include <chaos/MyGLFWGamepadManager.h>
#include <chaos/ParticleManager.h>
#include <chaos/Tickable.h>
#include <chaos/InputEventReceiver.h>

#include <death/GameInstanceEntity.h>
#include <death/GameFramework.h>
#include <death/Game.h>

namespace death
{

	// =============================================
	// Player
	// =============================================

	class Player : public chaos::Tickable, public chaos::InputEventReceiver, public GameInstanceEntity, public CheckpointObject<PlayerCheckpoint>
	{
		DEATH_GAMEFRAMEWORK_ALLFRIENDS()
		friend class PlayerGamepadCallbacks;

	public:

		/** constructor */
		Player(GameInstance * in_game_instance);
		/** destructor */
		virtual ~Player();

		// XXX : player allocation is not necessarly in one of the game particle_manager's layer
		//       it can be set from a level instance's particle_manager

		/** get the player allocation */
		chaos::ParticleAllocationBase * GetPlayerAllocation() { return player_allocations.get(); }
		/** get the player allocation */
		chaos::ParticleAllocationBase const * GetPlayerAllocation() const { return player_allocations.get(); }
		/** set the player allocation */
		virtual void SetPlayerAllocation(chaos::ParticleAllocationBase * in_allocation);

		/** get the index of the player */
		int GetPlayerIndex() const { return player_index; }

		/** get the current score */
		int GetScore() const { return score; }
		/** get the life count */
		int GetLifeCount() const { return life_count; }
        /** get the current health value */
        float GetCurrentHealth() const { return current_health; }
        /** get the maximum health value health value */
        float GetCurrentMaxHealth() const { return current_max_health; }

		/** returns whether the player is dead or not */
		bool IsDead() const { return life_count == 0; }

		/** update player score */
		void SetScore(int in_score, bool increment = false);
		/** update player life */
		void SetLifeCount(int in_life, bool increment = false);

		/** try to give physical device to the player */
		bool CapturePhysicalGamepad(chaos::MyGLFW::PhysicalGamepad * in_physical_gamepad);

		/** get player particle */
		chaos::ParticleDefault::Particle * GetPlayerParticle();
		chaos::ParticleDefault::Particle const * GetPlayerParticle() const;

		/** get player position */
		glm::vec2 GetPlayerPosition() const;
		/** get player box */
		chaos::box2 GetPlayerBox() const;
		/** set the player position */
		bool SetPlayerPosition(glm::vec2 const & position);
		/** set the player box */
		bool SetPlayerBox(chaos::box2 const & box);

		/** gets the left stick position */
		glm::vec2 GetLeftStickPosition() const { return left_stick_position; }
		/** gets the right stick position */
		glm::vec2 GeRightStickPosition() const { return right_stick_position; }

		/** gets the left trigger */
		float GetLeftTrigger() const { return left_trigger; }
		/** gets the right trigger */
		float GetRightTrigger() const { return right_trigger; }

	protected:

		/** initialize the game instance */
		virtual bool Initialize(death::GameInstance * in_game_instance) override;

		/** called whenever the level is being changed */
		virtual void OnLevelChanged(death::GameLevel * new_level, death::GameLevel * old_level, death::GameLevelInstance * new_level_instance);

		/** override */
		virtual bool DoTick(double delta_time) override;
		/** handle keyboard input */
		virtual bool OnKeyEvent(int key, int scan_code, int action, int modifier) override;
		/** handle keyboard input */
		virtual bool OnCharEvent(unsigned int c) override;
		/** handle mouse input */
		virtual bool OnMouseButton(int button, int action, int modifier) override;
		/** handle mouse movement */
		virtual bool OnMouseMove(double x, double y) override;

		/** extract stick values from keyboard state */
		virtual void CacheKeyboardPlayerDisplacementInputs();
		/** extract stick values from gamepad state */
		virtual void CacheGamepadPlayerDisplacementInputs();
		/** handle the player input */
		virtual void HandleKeyboardInputs(double delta_time);
		/** handle the player input (top level function) */
		virtual void HandleGamepadInputs(double delta_time);
		/** handle the input for our own purpose (consuming any data) */
		virtual void InternalHandleGamepadInputs(double delta_time, chaos::MyGLFW::GamepadData const * gamepad_data);
		/** reset the cached inputs (exists because we emulate keyboard arrow combinaison as un stick) */
		virtual void ResetCachedInputs();

		/** called whenever player gamepad is disconnected */
		virtual void OnGamepadDisconnected();

		/** tick the player displacement */
		virtual void TickPlayerDisplacement(double delta_time);

		/** called whenever a life is lost */
		virtual void OnLifeLost();
		/** override */
		virtual bool DoSaveIntoCheckpoint(PlayerCheckpoint * checkpoint) const override;
		/** override */
		virtual bool DoLoadFromCheckpoint(PlayerCheckpoint const * checkpoint) override;

	protected:

		/** a gamepad that can be given to the player */
		chaos::shared_ptr<chaos::MyGLFW::Gamepad> gamepad;

		/** the allocation for the player */
		chaos::shared_ptr<chaos::ParticleAllocationBase> player_allocations;

		/** the index of the player */
		int player_index = 0;
		/** the life for the player */
		int life_count = 3;
        /** the current health of the player */
        float current_health = 1.0f;
        /** the maximum health of the player */
        float current_max_health = 1.0f;
		/** the score for the player */
		int score = 0;

		/** the current stick position */
		glm::vec2 left_stick_position = glm::vec2(0.0f, 0.0f);
		glm::vec2 right_stick_position = glm::vec2(0.0f, 0.0f);
		float left_trigger = 0.0f;
		float right_trigger = 0.0f;
	};

	// =============================================
	// A callback object for gamepad disconnection
	// =============================================

	class PlayerGamepadCallbacks : public chaos::MyGLFW::GamepadCallbacks
	{

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
