#pragma once

#include <chaos/StandardHeaders.h> 

#include <death/Game.h>
#include <death/GameInstance.h>
#include <death/GameFramework.h>

// =========================================================
// LudumGameInstance
// =========================================================

class LudumGameInstance : public death::GameInstance
{

public:

	DEATH_GAMEFRAMEWORK_DECLARE_GAMEINSTANCE(Ludum);

	/** constructor */
	LudumGameInstance(death::Game * in_game);

	int GetWakenUpParticleCount() const { return waken_up_particle_count; }

	void NotifyAtomCountChange(int delta);

protected:

	virtual death::Player * DoCreatePlayer() override;

	virtual bool DoCheckGameOverCondition() override;

	virtual void OnLevelChanged(death::GameLevel * new_level, death::GameLevel * old_level, death::GameLevelInstance * new_level_instance) override;

	virtual bool DoTick(double delta_time) override;

	void TickHeartBeat(double delta_time);



protected:

	/** the life first player add in previous frame */
	float previous_frame_life = 0.0f;
	/** number of waken up particle */
	int waken_up_particle_count = 0;
	/** the heart beat time */
	float heart_beat_time = 0.0f;
};
