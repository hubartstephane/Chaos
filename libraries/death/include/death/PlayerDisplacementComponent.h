#pragma once

#include <chaos/StandardHeaders.h>
#include <chaos/Object.h>
#include <chaos/Tickable.h>

#include <death/GameFramework.h>
#include <death/GameGettersDecl.h>

namespace death
{
	class PlayerDisplacementComponent : public chaos::Tickable
	{
		DEATH_GAMEFRAMEWORK_ALLFRIENDS()

	public:

		DEATH_GAMEGETTERS_DECLARE();

		/** Initialization method */
		bool Initialize(Player* in_player);

		/** returns the player the component belongs to */
		chaos::AutoCastable<Player> GetPlayer() { return player; }
		/** returns the player the component belongs to */
		chaos::AutoConstCastable<Player> GetPlayer() const { return player; }

	protected:

		/** the player that owns this component */
		Player* player = nullptr;

	};



#if 0

	class PlayerMovement : public chaos::Object
	{
	public:

		/** the tick entry point */
		void Tick(float delta_time);

		/** whether the pawn is jumping */
		bool IsJumping() const;
		/** whether the pawn is falling */
		bool IsFalling() const;
		/** whether the pawn is touching the ground */
		bool IsTouchingGround() const;

	protected:

		/** apply acceleration/impulsion to the pawn */
		void DisplacePawn(float delta_time);
		/** compute the pawn collision */
		void ComputePawnCollision(float delta_time);

		/** apply collisions with roof/ground */
		void ApplyVerticalCollisions(glm::vec2 const & old_position, bool & jump_stopped_thrown);
		/** apply collisions with walls */
		void ApplyHorizontalCollisions(glm::vec2 const & old_position, bool & run_stopped_thrown);

		/** update the vertical velocity */
		void  UpdateVerticalVelocity(float delta_time, bool & jump_stopped_thrown);
		/** update the vertical impulse */
		float UpdateVerticalImpulse(float delta_time, bool & jump_stopped_thrown);
		/** call a single time per frame the OnJumpStopped callbacks */
		void ConditionnalThrowJumpStoppedEvent(bool & jump_stopped_thrown);

		/** update the horizontal velocity */
		void UpdateHorizontalVelocity(float delta_time, bool & run_stopped_thrown);

		/** call a single time per frame the OnRunStopped callbacks */
		void ConditionnalThrowRunStoppedEvent(bool & run_stopped_thrown);




		/** test whether the jump button is pressed */
		bool IsJumpKeyPressed() const;

		/** called whenever the jump is successfully started */
		virtual void OnJumpStarted();
		/** called whenever the jump button is released (or an obstacle is hit that stops the jump) */
		virtual void OnJumpStopped();
		/** called whenever the pawn start running */
		virtual void OnRunStarted();
		/** called whenever the pawn stop running */
		virtual void OnRunStopped();

		/** called whenever the pawn touch the ground */
		virtual void OnGroundTouched();
		/** called whenever the pawn hit a roof */
		virtual void OnRoofTouched();
		/** called whenever the pawn touch a wall */
		virtual void OnWallTouched();

	protected:

		/** the current position of the pawn */
		glm::vec2 position = glm::vec2(0.0f, 0.0f);
		/** the current velocity of the pawn */
		glm::vec2 velocity = glm::vec2(0.0f, 0.0f);

		/** the gravity of the game */
		float gravity = 10.0f;
		/** the jump velocity */
		float jump_impulse = 10.0f;
		/** the max speed of the player in Horizontal and Vertical direction */
		glm::vec2 max_velocity = glm::vec2(10.0f, 10.0f);

		/** the max time during which the player can receive a vertical impulse */
		float max_impulse_time = 0.2f;
		/** the max height displacement during which the player can be impulsing  */
		float max_impulse_height = 20.0f;
		/** the number of time the player can jump before touching the ground */
		int   max_jump_count = 1;

		/** the current impulse time */
		float current_impulse_time = 0.0f;
		/** the current impulse height */
		float current_impulse_height = 0.0f;
		/** the current jump count */
		int   current_jump_count = 0;

		/** whether the jump button must be ignored until it is released (initialize to true so that player does not jump when the game begins) */
		bool wait_jump_button_released = true;
		/** whether the player is jumping */
		bool jumping = false;
		/** whether to pawn is touching te ground */
		bool touching_ground = false;
	};

#endif

}; // namespace death
