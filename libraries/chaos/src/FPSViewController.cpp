#include <chaos/FPSViewController.h>
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
	void FPSViewController::GoBackward(float speed)
	{
		float angle = MathTools::DegreeToRadian(yaw);
		position += speed * glm::vec3(MathTools::Sin(angle), 0.0f, MathTools::Cos(angle));
	}

	void FPSViewController::GoForward(float speed)
	{ 
		GoBackward(-speed); 
	}

	// use :
	//   float angle = MathTools::DegreeToRadian(yaw + 90.0f);
	//   position += speed * glm::vec3(MathTools::Sin(angle), 0.0f, MathTools::Cos(angle));
	// apply :
	//   cos(PI/2 + x) = -sin(x)     sin(PI/2 + x) = cos(x)
	void FPSViewController::StrafeRight(float speed)
	{
		float angle = MathTools::DegreeToRadian(yaw);
		position += speed * glm::vec3(MathTools::Cos(angle), 0.0f, -MathTools::Sin(angle));
	}

	void FPSViewController::StrafeLeft(float speed)
	{ 
		StrafeRight(-speed); 
	}

	void FPSViewController::SetPitch(float value)
	{
		pitch = MathTools::Clamp(value, -90.0f, 90.0f);
	}

	void FPSViewController::SetYaw(float value)
	{
		yaw = MathTools::Fmod(value, 360.0f);
	}

	void FPSViewController::SetRoll(float value)
	{
		roll = MathTools::Fmod(value, 360.0f);
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
