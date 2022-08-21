#include "chaos/Chaos.h"

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
	void FPSViewController::GoBackward(float value)
	{
		float angle = MathTools::DegreeToRadian(yaw);
		position += value * glm::vec3(std::sin(angle), 0.0f, std::cos(angle));
	}

	void FPSViewController::GoForward(float value)
	{ 
		GoBackward(-value);
	}

	// use :
	//   float angle = MathTools::DegreeToRadian(yaw + 90.0f);
	//   position += value * glm::vec3(std::sin(angle), 0.0f, std::cos(angle));
	// apply :
	//   cos(PI/2 + x) = -sin(x)     sin(PI/2 + x) = cos(x)
	void FPSViewController::StrafeRight(float value)
	{
		float angle = MathTools::DegreeToRadian(yaw);
		position += value * glm::vec3(std::cos(angle), 0.0f, -std::sin(angle));
	}

	void FPSViewController::StrafeLeft(float value)
	{ 
		StrafeRight(-value);
	}

	void FPSViewController::SetPitch(float value)
	{
		pitch = std::clamp(value, -90.0f, 90.0f);
	}

	void FPSViewController::SetYaw(float value)
	{
		yaw = std::fmod(value, 360.0f);
	}

	void FPSViewController::SetRoll(float value)
	{
		roll = std::fmod(value, 360.0f);
	}

	void FPSViewController::IncrementPitch(float value)
	{
		SetPitch(pitch + value);
	}

	void FPSViewController::IncrementYaw(float value)
	{
		SetYaw(yaw + value);
	}

	void FPSViewController::IncrementRoll(float value)
	{
		SetRoll(roll + value);
	}

	glm::mat4 FPSViewController::GlobalToLocal() const
	{
		return  
			glm::rotate(- MathTools::DegreeToRadian(roll),  glm::vec3(0.0f, 0.0f, 1.0f)) *
			glm::rotate(- MathTools::DegreeToRadian(pitch), glm::vec3(1.0f, 0.0f, 0.0f)) *
			glm::rotate(- MathTools::DegreeToRadian(yaw),   glm::vec3(0.0f, 1.0f, 0.0f)) *
			glm::translate(-position);
	}

	glm::mat4 FPSViewController::LocalToGlobal() const
	{
		return 
			glm::translate(position) *
			glm::yawPitchRoll(MathTools::DegreeToRadian(yaw), MathTools::DegreeToRadian(pitch), MathTools::DegreeToRadian(roll));   
	}

}; // namespace chaos
