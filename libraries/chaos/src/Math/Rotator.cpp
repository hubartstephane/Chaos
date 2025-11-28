#include "chaos/ChaosPCH.h"
#include "chaos/ChaosInternals.h"

namespace chaos
{
	glm::mat4x4 GetRotationMatrix(glm::quat const & rotation)
	{
		return glm::toMat4(rotation);
	}

	glm::dmat4x4 GetRotationMatrix(glm::dquat const & rotation)
	{
		return glm::toMat4(rotation);
	}

	glm::mat4x4 GetRotationMatrix(float rotation)
	{
		// hand made rotation for 2D to avoid the usage of a quaternion
		glm::mat4x4 result = glm::scale(glm::vec3(1.0f, 1.0f, 1.0f));

		auto c = std::cos(rotation);
		auto s = std::sin(rotation);
		result[0][0] =  c;
		result[0][1] =  s;
		result[1][0] = -s;
		result[1][1] =  c;
		return result;
	}

	glm::dmat4x4 GetRotationMatrix(double rotation)
	{
		// hand made rotation for 2D to avoid the usage of a quaternion
		glm::dmat4x4 result = glm::scale(glm::dvec3(1.0, 1.0, 1.0));

		auto c = std::cos(rotation);
		auto s = std::sin(rotation);
		result[0][0] =  c;
		result[0][1] =  s;
		result[1][0] = -s;
		result[1][1] =  c;
		return result;
	}

}; // namespace chaos
