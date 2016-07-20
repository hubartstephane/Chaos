#pragma once

#include <chaos/StandardHeaders.h>
#include <chaos/GeometryFramework.h>
#include <chaos/MathTools.h>

//
// In OpenGL, the camera system is oriented as follow
//
//     Y
//      ^
//      |
//      |
//      +-------> X
//     / 
//    / 
//   L Z
//
// The eye is located along +Z and looking to the -Z direction
// We choose to set the YAW has the angle with the -Z axis, so that if YAW is 0 than that correspond to projection matrix we are using


namespace chaos
{
/** 
 * FPSViewController : this is a class to help handling FPS system with simple actions like go forward or strafe
 */

class FPSViewController
{

public:

  /** the position of the camera in its parent system */
  glm::vec3 position;
  /** Yaw (expressed in degree) */
  float yaw;
  /** Pitch (expressed in degree) */
  float pitch;
  /** Roll (expressed in degree) */
  float roll;

  /** constructor */
  FPSViewController() : position(0.0f), yaw(0.0f), pitch(0.0f), roll(0.0f) {}

  /** advance the camera */
  void GoBackward(float speed)
  {
    float angle = MathTools::DegreeToRadian(yaw);
    position += speed * glm::vec3(MathTools::Sin(angle), 0.0f, MathTools::Cos(angle));
  }

  /** advance the camera */
  void GoForward(float speed){ GoBackward(-speed); }

  /** strafe the camera */
  void StrafeRight(float speed)
  {
    // use :
    //   float angle = MathTools::DegreeToRadian(yaw + 90.0f);
    //   position += speed * glm::vec3(MathTools::Sin(angle), 0.0f, MathTools::Cos(angle));
    // apply :
    //   cos(PI/2 + x) = -sin(x)     sin(PI/2 + x) = cos(x)
    
    float angle = MathTools::DegreeToRadian(yaw);
    position += speed * glm::vec3(MathTools::Cos(angle), 0.0f, -MathTools::Sin(angle));
  }

  /** strafe the camera (X axis, positive values on the right) */
  void StrafeLeft(float speed){ StrafeRight(-speed); }

  /** change the angles of the camera */
  inline void SetPitch(float value)
  {
    pitch = MathTools::Clamp(value, -90.0f, 90.0f);
  }

  /** change the angles of the camera */
  void SetYaw(float value)
  {
    yaw = MathTools::Fmod(value, 360.0f);
  }

  /** change the angles of the camera */
  void SetRoll(float value)
  {
    roll = MathTools::Fmod(value, 360.0f);
  }

  /** increment angle of the camera */
  inline void IncrementPitch(float value)
  {
    SetPitch(pitch + value);
  }

  /** increment angle of the camera */
  inline void IncrementYaw(float value)
  {
    SetYaw(yaw + value);
  }

  /** increment angle of the camera */
  inline void IncrementRoll(float value)
  {
    SetRoll(roll + value);
  }

  /** get transformation matrix */
  glm::mat4 GlobalToLocal() const
  {
    return  
      glm::rotate(- MathTools::DegreeToRadian(roll),  glm::vec3(0.0f, 0.0f, 1.0f)) *
      glm::rotate(- MathTools::DegreeToRadian(pitch), glm::vec3(1.0f, 0.0f, 0.0f)) *
      glm::rotate(- MathTools::DegreeToRadian(yaw),   glm::vec3(0.0f, 1.0f, 0.0f)) *
      glm::translate(-position);
  }

  /** get transformation matrix */
  glm::mat4 LocalToGlobal() const
  {
    return 
      glm::translate(position) *
      glm::yawPitchRoll(MathTools::DegreeToRadian(yaw), MathTools::DegreeToRadian(pitch), MathTools::DegreeToRadian(roll));   
  }
};

}; // namespace chaos
