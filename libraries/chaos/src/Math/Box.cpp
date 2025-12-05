#include "chaos/ChaosPCH.h"
#include "chaos/ChaosInternals.h"

namespace chaos
{
	glm::vec4 EncodeBoxToVector(box_base2 const & src)
	{
		glm::vec4 result;
		result.x = src.position.x;
		result.y = src.position.y;
		result.z = src.half_size.x;
		result.w = src.half_size.y;
		return result;
	}

}; // namespace chaos
