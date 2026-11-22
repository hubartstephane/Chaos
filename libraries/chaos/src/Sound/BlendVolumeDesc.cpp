#include "chaos/ChaosPCH.h"
#include "chaos/ChaosInternals.h"

namespace chaos
{
	BlendVolumeDesc BlendVolumeDesc::BlendIn(float blend_time)
	{
		BlendVolumeDesc result;
		result.blend_type = SoundBlendType::BlendIn;
		result.blend_time = blend_time;
		return result;
	}

	BlendVolumeDesc BlendVolumeDesc::BlendOut(float blend_time, bool pause_at_end, bool kill_at_end, bool kill_when_paused)
	{
		BlendVolumeDesc result;
		result.blend_type = SoundBlendType::BlendOut;
		result.blend_time = blend_time;
		result.pause_at_end = pause_at_end;
		result.kill_at_end = kill_at_end;
		result.kill_when_paused = kill_when_paused;
		return result;
	}

}; // namespace chaos