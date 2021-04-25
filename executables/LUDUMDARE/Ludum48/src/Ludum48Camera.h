#pragma once

#include <chaos/Chaos.h>


class MyAutoRecenterCameraComponent : public chaos::AutoRecenterToPlayerCameraComponent
{
	CHAOS_DECLARE_OBJECT_CLASS2(MyAutoRecenterCameraComponent, chaos::AutoRecenterToPlayerCameraComponent);

public:


	MyAutoRecenterCameraComponent(size_t in_player_index = 0);


	virtual chaos::box2 GetTargetBox() const override;

};