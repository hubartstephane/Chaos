#include "chaos/ChaosPCH.h"
#include "chaos/ChaosInternals.h"

//class SoundCategory;
//#include "chaos/SoundCore/PlaySoundDesc.h"

namespace chaos
{
	bool PlaySoundDesc::IsSound3D() const
	{
		return is_3D_sound;
	}

	void PlaySoundDesc::Enable3D(bool enable)
	{
		is_3D_sound = enable;
	}

	void PlaySoundDesc::SetPosition(glm::vec3 const & in_position, bool update_3D_sound)
	{
		position = in_position;
		if (update_3D_sound)
			is_3D_sound = true;
	}

	void PlaySoundDesc::SetVelocity(glm::vec3 const & in_velocity, bool update_3D_sound)
	{
		velocity = in_velocity;
		if (update_3D_sound)
			is_3D_sound = true;
	}

}; // namespace chaos