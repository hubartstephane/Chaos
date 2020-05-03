#include <chaos/GeometryFramework.h>
#include <chaos/MathTools.h>

namespace chaos
{
	glm::vec4 EncodeBoxToVector(chaos::box2 const & src)
	{
		glm::vec4 result;
		result.x = src.position.x;
		result.y = src.position.y;
		result.z = src.half_size.x;
		result.w = src.half_size.y;		
		return result;
	}

	glm::mat4x4 GetRotatorMatrix(glm::quat const & rotator)
	{
		return glm::toMat4(rotator);
	}
	/** rotator to matrix */
	glm::dmat4x4 GetRotatorMatrix(glm::dquat const & rotator)
	{
		return glm::toMat4(rotator);
	}
	/** rotator to matrix */
	glm::mat4x4 GetRotatorMatrix(float rotator)
	{
		// hand made rotation for 2D to avoid the usage of a quaternion
		glm::mat4x4 result = glm::scale(glm::vec3(1.0f, 1.0f, 1.0f));

		auto c = std::cos(rotator);
		auto s = std::sin(rotator);
		result[0][0] =  c;
		result[0][1] =  s;
		result[1][0] = -s;
		result[1][1] =  c;
		return result;
//		return glm::rotate(rotator, glm::vec3(0.0f, 0.0f, 1.0f));
	}
	/** rotator to matrix */
	glm::dmat4x4 GetRotatorMatrix(double rotator)
	{
		// hand made rotation for 2D to avoid the usage of a quaternion
		glm::dmat4x4 result = glm::scale(glm::dvec3(1.0, 1.0, 1.0));

		auto c = std::cos(rotator);
		auto s = std::sin(rotator);
		result[0][0] =  c;
		result[0][1] =  s;
		result[1][0] = -s;
		result[1][1] =  c;
		return result;
		// return glm::rotate(rotator, glm::dvec3(0.0f, 0.0f, 1.0));
	}

	glm::vec2 GetPlaneNormal(glm::vec3  const & p)
	{
		return auto_cast_vector(p);
	}

	glm::vec3 GetPlaneNormal(glm::vec4  const & p)
	{
		return auto_cast_vector(p);
	}

	glm::dvec2 GetPlaneNormal(glm::dvec3 const & p)
	{
		return auto_cast_vector(p);
	}

	glm::dvec3 GetPlaneNormal(glm::dvec4 const & p)
	{
		return auto_cast_vector(p);
	}

	float GetPlaneOffset(glm::vec3  const & p)
	{
		return p.z;	
	}

	float GetPlaneOffset(glm::vec4  const & p)
	{
		return p.w;	
	}

	double GetPlaneOffset(glm::dvec3 const & p)
	{
		return p.z;	
	}

	double GetPlaneOffset(glm::dvec4 const & p)
	{
		return p.w;	
	}

}; // namespace chaos
