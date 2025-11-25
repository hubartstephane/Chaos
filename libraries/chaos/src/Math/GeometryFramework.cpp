#include "chaos/ChaosPCH.h"
#include "chaos/ChaosInternals.h"

namespace chaos
{
	glm::vec4 EncodeBoxToVector(box2 const & src)
	{
		glm::vec4 result;
		result.x = src.position.x;
		result.y = src.position.y;
		result.z = src.half_size.x;
		result.w = src.half_size.y;
		return result;
	}

	glm::mat4x4 GetRotationMatrix(glm::quat const & rotation)
	{
		return glm::toMat4(rotation);
	}
	/** rotator to matrix */
	glm::dmat4x4 GetRotationMatrix(glm::dquat const & rotation)
	{
		return glm::toMat4(rotation);
	}
	/** rotator to matrix */
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
	/** rotator to matrix */
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
