class DisplacementManager
{
public:

  
  /** the tick entry point */
  void Tick(double delta_time);
  
  /** whether the pawn is jumping */
  bool IsJumping() const;
  /** whether the pawn is falling */
  bool IsFalling() const;
  /** whether the pawn is touching the ground */
  bool IsTouchingGround() const;
  
protected:
        
  /** update the vertical velocity */
  void  UpdateVerticalVelocity(double delta_time);                  
  /** update the vertical impulse */
  float UpdateVerticalImpulse(double delta_time);  
  /** test whether the jump button is pressed */
  bool IsJumpKeyPressed() const;
                               
                               
  /** called whenever the jump button is successfully pressed (with an effective jump start) */
  virtual void OnJumpStarted();
  /** called whenever the jump button is released */
  virtual void OnJumpStopped();
  /** called whenever the pawn touch the ground */
  virtual void OnGroundTouched();
  /** called whenever the pawn hit a roof */
  virtual void OnRoofTouched();

    
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
  int   max_jump_count = 1:
  
  /** the current jump count */
  int   current_jump_count = 0;
  /** the current impulse time */
  float current_impulse_time = 0.0f;
  /** the current impulse height */
  float current_impulse_height = 0.0f;
  
  /** whether the jump button is pressed */
  bool jump_button_pressed = false;
  /** whether to pawn is touching te ground */
  bool touching_floor = false;
};

bool DisplacementManager::IsJumping() const
{
  if (IsTouchingGround())
    return false;
  if (velocity.y > 0.0f)
    return true;
  return false;
}
  
bool DisplacementManager::IsFalling() const
{
  if (IsTouchingGround())
    return false;
  if (velocity.y < 0.0f)
    return true;
  return false;    
}

bool DisplacementManager::IsTouchingGround() const
{
  return touching_ground;
}
  
void DisplacementManager::OnJumpStarted()
{
}
  
void DisplacementManager::OnJumpEnded()
{
}

void DisplacementManager::OnGroundTouched()
{
}

void DisplacementManager::OnRoofTouched()
{
}

void DisplacementManager::Tick(double delta_time)
{
  // compute the vertical velocity
  UpdateVerticalVelocity(delta_time);
  // move the pawn
  DisplacePawn(delta_time);
  // search for collisions 
  glm::vec2 old_position = position;
  ComputePawnCollision(delta_time);
  // search whether the player touched a GROUND
  bool old_touching_ground = touching_ground; 
  touching_ground = false;
  // collision bring the pawn higher => this is a collision with the GROUND
  if (old_position.y < position.y)
  {
    current_jump_count = 0;
    current_impulse_time = 0.0f;
    current_impulse_height = 0.0f; // reset jump data
    touching_ground = true;
    if (!old_touching_ground) // if we were already touching the ground, we are walking => do not trigger the callback
      OnGroundTouched();    
  }                                                       
  // collision bring the pawn lower => this is a collision with the ROOF
  else if (old_position.y > position.y)
  {
    current_impulse_time = max_impulse_time; // top of the jump curve
    current_impulse_height = max_impulse_height;
    OnRoofTouched();
  }                                                       
}

void DisplacementManager::DisplacePawn(double delta_time)
{
  position += velocity * (float)delta_time;
}

void DisplacementManager::UpdateVerticalVelocity(double delta_time)
{
  float impulse = UpdateVerticalImpulse(delta_time);
  // no vertical impulse, let gravity do its action
  if (impulse != 0)
    velocity.y = impulse;
  else
    velocity.y -= gravity * (float)delta_time;
  // clamp the vertical velocity
  velocity.y = MathTools::Clamp(velocity.y, -max_velocity.y, +max_velocity.y);
}

float DisplacementManager::UpdateVerticalImpulse(double delta_time)
{
  // update the pressed button state 
  bool old_jump_pressed = jump_pressed;
  bool new_jump_pressed = IsJumpKeyPressed();
  jump_pressed = new_jump_pressed;
  // compute vertical impulse  
  if (new_jump_pressed)
  {                                           
    // begin jumping
    if (!old_jump_pressed) 
    {
      if (current_jump_count == max_jump_count) // cannot jump anymore
        return 0.0f;
      ++current_jump_count;
      OnJumpStarted();
    }
    // continue jumping
    else
    {
      // jump limit reached. player has to release the jump button before doing another jump
      if (max_impulse_time > 0 && current_impulse_time >= max_impulse_time)
      {
        OnJumpStopped();
        return 0.0f;
      }
      if (max_impulse_height > 0 && current_impulse_height >= max_impulse_height)
      {
        OnJumpStopped();
        return 0.0f;   
      }           
    }
    // update impulse time
    if (max_impulse_time > 0.0f)
      current_impulse_time = MathTools::Min(current_impulse_time + (float)delta_time, max_impulse_time);
    // update impulse height
    if (max_impulse_height > 0.0f)
      current_impulse_height = MathTools::Min(current_impulse_height + (float)delta_time * jump_impulse, max_impulse_height);
    // we are impulsing
    return jump_impulse;
  }
  else
  {
    if (old_jump_pressed)
      OnJumpStopped();
    current_impulse_time = 0.0f;
    current_impulse_height = 0.0f;
    return 0.0f;
  } 
}

bool DisplacementManager::IsJumpKeyPressed() const
{
  return false;
}
