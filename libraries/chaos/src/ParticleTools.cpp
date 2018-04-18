#include <chaos/ParticleTools.h>
#include <chaos/Hotpoint.h>

namespace chaos
{
	ParticleCorners ParticleTools::GetParticleCorners(glm::vec2 const & position, glm::vec2 const & size, int hotpoint_type)
	{
		ParticleCorners result;
		result.bottomleft_position = Hotpoint::ConvertToBottomLeft(position, size, hotpoint_type);
		result.topright_position = result.bottomleft_position + size;
		return result;
	}



}; // namespace chaos

