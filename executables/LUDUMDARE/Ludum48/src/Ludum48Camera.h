#pragma once

#include "Ludum48.h"

class LudumAutoRecenterToPlayerCameraComponent : public AutoRecenterToPlayerCameraComponent
{
	CHAOS_DECLARE_OBJECT_CLASS2(LudumAutoRecenterToPlayerCameraComponent, AutoRecenterToPlayerCameraComponent);

public:


	LudumAutoRecenterToPlayerCameraComponent(size_t in_player_index = 0);


	virtual box2 GetTargetBox() const override;



	mutable box2 previous_frame_box;

};