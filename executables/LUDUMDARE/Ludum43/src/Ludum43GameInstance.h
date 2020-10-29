#pragma once

#include <chaos/Chaos.h>

// =========================================================
// LudumGameInstance
// =========================================================

class LudumGameInstance : public chaos::GameInstance
{

	DEATH_GAMEFRAMEWORK_DECLARE_FRIENDSHIPS(Ludum);

	CHAOS_OBJECT_DECLARE_CLASS2(LudumGameInstance, chaos::GameInstance);

public:

	LudumGameInstance();

	int GetWakenUpParticleCount() const { return waken_up_particle_count; }

	void NotifyAtomCountChange(int delta);

protected:

	virtual void OnLevelChanged(chaos::Level * new_level, chaos::Level * old_level, chaos::LevelInstance * new_level_instance) override;

	virtual bool DoTick(float delta_time) override;

	void TickHeartBeat(float delta_time);



protected:

	/** the life first player add in previous frame */
	float previous_frame_life = 0.0f;
	/** number of waken up particle */
	int waken_up_particle_count = 0;
	/** the heart beat time */
	float heart_beat_time = 0.0f;
};
