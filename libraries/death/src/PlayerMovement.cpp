#include <death/PlayerMovement.h>

#include <chaos/MathTools.h>

namespace death
{

	bool PlayerMovement::IsJumping() const
	{
		if (IsTouchingGround())
			return false;
		if (velocity.y > 0.0f) // as soon as a roof is touched, velocity becomes 0.0, IsJumping(..) becomes false
			return true;
		return false;
	}

	bool PlayerMovement::IsFalling() const
	{
		if (IsTouchingGround())
			return false;
		if (velocity.y < 0.0f)
			return true;
		return false;
	}

	bool PlayerMovement::IsTouchingGround() const
	{
		return touching_ground;
	}

	void PlayerMovement::ConditionnalThrowJumpStoppedEvent(bool & jump_stopped_thrown)
	{
		if (jump_stopped_thrown)
			return;
		jump_stopped_thrown = true;
		OnJumpStopped();
	}

	void PlayerMovement::ConditionnalThrowRunStoppedEvent(bool & run_stopped_thrown)
	{
		if (run_stopped_thrown)
			return;
		run_stopped_thrown = true;
		OnRunStopped();
	}

	void PlayerMovement::Tick(float delta_time)
	{
		// compute the vertical velocity
		bool jump_stopped_thrown = false;
		UpdateVerticalVelocity(delta_time, jump_stopped_thrown);
		// compute the horizontal velocity
		bool run_stopped_thrown = false;
		UpdateHorizontalVelocity(delta_time, run_stopped_thrown);
		// move the pawn
		DisplacePawn(delta_time);
		// search for collisions 
		glm::vec2 old_position = position;
		ComputePawnCollision(delta_time);
		// apply collisions
		ApplyVerticalCollisions(old_position, jump_stopped_thrown);
		ApplyHorizontalCollisions(old_position, run_stopped_thrown);
	}

	void PlayerMovement::UpdateVerticalVelocity(float delta_time, bool & jump_stopped_thrown)
	{
		float impulse = UpdateVerticalImpulse(delta_time, jump_stopped_thrown);
		// no vertical impulse, let gravity do its action
		if (impulse == 0.0f)
			velocity.y -= gravity * delta_time; // apply accelerate
		else
			velocity.y = impulse;
		// clamp the vertical velocity
		if (max_velocity.y >= 0.0f)
			velocity.y = std::clamp(velocity.y, -max_velocity.y, +max_velocity.y);
	}

	float PlayerMovement::UpdateVerticalImpulse(float delta_time, bool & jump_stopped_thrown)
	{
		// get jump button state button state 
		bool jump_pressed = IsJumpKeyPressed();

		// button is released, we can reset the forbidden flag
		if (!jump_pressed)
		{
			bool old_jumping = jumping;
			jumping = false;
			wait_jump_button_released = false;
			if (old_jumping)
				ConditionnalThrowJumpStoppedEvent(jump_stopped_thrown);		
			return 0.0f;
		}
		assert(jump_pressed);

		// want to jump or continue jumping, but action is forbidden until button released
		if (wait_jump_button_released)
			return 0.0f;

		// begin jumping
		if (!jumping)
		{
			if (current_jump_count == max_jump_count) // cannot jump anymore
				return 0.0f;
			++current_jump_count;
			jumping = true;
			current_impulse_time = 0.0f;
			current_impulse_height = 0.0f;
			OnJumpStarted();
			return jump_impulse;
		}
		assert(jumping);

		// continue jumping (time consideration)
		if (max_impulse_time > 0.0f)
		{
			current_impulse_time = current_impulse_time + delta_time;
			if (current_impulse_time >= max_impulse_time)
			{
				current_impulse_time = max_impulse_time; // clamp
				wait_jump_button_released = true;
				ConditionnalThrowJumpStoppedEvent(jump_stopped_thrown);
				return 0.0f;
			}
			return jump_impulse;
		}
		// continue jumping (height consideration)
		if (max_impulse_height > 0.0f)
		{
			current_impulse_height = current_impulse_height + delta_time * jump_impulse; // integral over velocity
			if (current_impulse_height >= max_impulse_height)
			{
				current_impulse_height = max_impulse_height; // clamp
				wait_jump_button_released = true;
				ConditionnalThrowJumpStoppedEvent(jump_stopped_thrown);
				return 0.0f;
			}
			return jump_impulse;
		}
		// no jump, no impulse
		return 0.0f;
	}

	void PlayerMovement::UpdateHorizontalVelocity(float delta_time, bool & run_stopped_thrown)
	{


	}



	void PlayerMovement::DisplacePawn(float delta_time)
	{
		position += velocity * delta_time;
	}

	void PlayerMovement::ComputePawnCollision(float delta_time)
	{

	}

	void PlayerMovement::ApplyVerticalCollisions(glm::vec2 const & old_position, bool & jump_stopped_thrown)
	{
		// search whether the player touched a GROUND
		bool old_touching_ground = touching_ground;
		touching_ground = false;
		// collision bring the pawn higher => this is a collision with the GROUND
		if (old_position.y < position.y)
		{
			velocity.y = 0.0f; // reset jump data
			current_jump_count = 0;
			jumping = false;
			touching_ground = true;
			if (!old_touching_ground) // if we were already touching the ground, we are walking => do not trigger the callback
				OnGroundTouched();
		}
		// collision bring the pawn lower => this is a collision with the ROOF
		else if (old_position.y > position.y)
		{
			velocity.y = 0.0f;
			jumping = false;
			wait_jump_button_released = true;
			ConditionnalThrowJumpStoppedEvent(jump_stopped_thrown);
			OnRoofTouched();
		}
	}

	void PlayerMovement::ApplyHorizontalCollisions(glm::vec2 const & old_position, bool & run_stopped_thrown)
	{

	}

	bool PlayerMovement::IsJumpKeyPressed() const
	{
		return false;
	}





	void PlayerMovement::OnJumpStarted()
	{
	}

	void PlayerMovement::OnJumpStopped()
	{
	}

	void PlayerMovement::OnRunStarted()
	{
	}

	void PlayerMovement::OnRunStopped()
	{
	}

	void PlayerMovement::OnGroundTouched()
	{
	}

	void PlayerMovement::OnRoofTouched()
	{
	}

	void PlayerMovement::OnWallTouched()
	{
	}

}; // namespace death