#pragma once

#include <chaos/Chaos.h>


class LudumAutoRecenterToPlayerCameraComponent : public chaos::AutoRecenterToPlayerCameraComponent
{
	CHAOS_DECLARE_OBJECT_CLASS2(LudumAutoRecenterToPlayerCameraComponent, chaos::AutoRecenterToPlayerCameraComponent);

public:


	LudumAutoRecenterToPlayerCameraComponent(size_t in_player_index = 0);


	virtual chaos::box2 GetTargetBox() const override;



	mutable chaos::box2 previous_frame_box;

};