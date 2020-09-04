#pragma once

#include <chaos/StandardHeaders.h> 
#include <chaos/BitmapAtlas.h>

#include <death/Game.h>
#include <death/GameInstance.h>
#include <death/Player.h>
#include <death/GameFramework.h>


#include "Ludum45Particles.h"
#include "Ludum45Upgrades.h"

// =========================================================
// LudumPlayer
// =========================================================

class LudumPlayer : public death::Player
{

public:

	friend class BonusSpawnerTrigger;

	friend class ParticleBonusTrait;
	friend class ParticlePlayerLayerTrait;
	friend class ParticleFireLayerTrait;
	friend class GameHUDUpgradeComponent;
	friend class ParticleEnemyTrait;

	DEATH_GAMEFRAMEWORK_DECLARE_FRIENDSHIPS(Ludum);

	CHAOS_OBJECT_DECLARE_CLASS2(LudumPlayer, death::Player);

	size_t GetUpgradeLevel(chaos::TagType upgrade_type) const;

	size_t GetSpeedLevel() const;
	size_t GetDamageLevel() const;
	size_t GetDashLevel() const;
	size_t GetGhostLevel() const;
	size_t GetPowerRateLevel() const;
	size_t GetPowerSpreadLevel() const;
	size_t GetSpecialPowerLevel() const;

	float GetCurrentSpeedValue() const; 
	float GetCurrentDamageValue() const; 
	float GetCurrentDashValue() const; 
	bool  GetCurrentGhostValue() const; 
	float GetCurrentPowerRateValue() const; 
	int   GetCurrentPowerSpreadValue() const; 
	float GetCurrentSpecialPowerValue() const; 
	
	template<typename T>
	T GetPlayerUpgradedValue(chaos::TagType upgrade_type, std::vector<T> const & values) const;

protected:

	/** override */
	virtual bool Initialize(death::GameInstance * in_game_instance);

	/** override */
	virtual void OnLevelChanged(death::Level * new_level, death::Level * old_level, death::LevelInstance * new_level_instance) override;

	/** override */
	virtual void TickInternal(float delta_time) override;
	/** override */
	virtual void TickPlayerDisplacement(float delta_time) override;


	void UpdatePlayerAcceleration(float delta_time);
	void UpdatePlayerFire(float delta_time);
	void UpdateBrightSideOfLife(float delta_time);

	virtual void OnHealthChanged(float old_health, float new_health, bool invulnerable) override;

	void FireProjectiles();

	void RegisterUpgrades();

	class PlayerUpgrade * FindPlayerUpgrade(chaos::TagType upgrade_type);

	class PlayerUpgrade const * FindPlayerUpgrade(chaos::TagType upgrade_type) const;

	void OnPlayerUpgrade(chaos::TagType upgrade_type);

	std::string GetPlayerUpgradeString() const;

	void DoUpdateBrightSideOfLife(bool value);

protected:

	bool brightsideoflife = false;

	float brightsideoflife_timer = 0.0f;

	std::vector<chaos::shared_ptr<PlayerUpgrade>> upgrades;

	float fire_timer = 0.0f; 
	float dash_timer = 0.0f; 
	float dash_cooldown = 0.0f; 

	bool  dash_locked = false; // locked until key is released
};
