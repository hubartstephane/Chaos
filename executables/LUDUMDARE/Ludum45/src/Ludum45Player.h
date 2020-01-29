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

	friend class BonusSpawnerTriggerObject;

	friend class ParticleBonusTrait;
	friend class ParticlePlayerTrait;
	friend class ParticleFireTrait;
	friend class GameHUDUpgradeComponent;
	friend class ParticleEnemyTrait;

	DEATH_GAMEFRAMEWORK_DECLARE_PLAYER(Ludum);

	/** constructor */
	LudumPlayer(death::GameInstance * in_game_instance);

	/** get the player particle */
	ParticlePlayer * GetPlayerParticle();
	ParticlePlayer const * GetPlayerParticle() const;

	/** override */
	virtual void SetPlayerAllocation(chaos::ParticleAllocationBase * in_allocation) override;

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
	virtual death::PlayerCheckpoint * DoCreateCheckpoint() const override;
	/** override */
	virtual bool DoLoadFromCheckpoint(death::PlayerCheckpoint const * checkpoint) override;
	/** override */
	virtual bool DoSaveIntoCheckpoint(death::PlayerCheckpoint * checkpoint) const override;

	/** override */
	virtual void OnLevelChanged(death::GameLevel * new_level, death::GameLevel * old_level, death::GameLevelInstance * new_level_instance) override;
	/** override */
	virtual void TickPlayerDisplacement(double delta_time) override;
	/** override */
	virtual void InternalHandleGamepadInputs(double delta_time, chaos::MyGLFW::GamepadData const * gpd) override;
	/** override */
	virtual void HandleKeyboardInputs(double delta_time) override;


	void UpdatePlayerAcceleration(double delta_time);
	void UpdatePlayerFire(double delta_time);
	void UpdateBrightSideOfLife(double delta_time);
	bool CheckButtonPressed(int const * keyboard_buttons, int gamepad_button);
	void OnDamagedReceived(float damage);

	void FireProjectiles();

	void RegisterUpgrades();

	class PlayerUpgrade * FindPlayerUpgrade(chaos::TagType upgrade_type);

	class PlayerUpgrade const * FindPlayerUpgrade(chaos::TagType upgrade_type) const;

	void OnPlayerUpgrade(chaos::TagType upgrade_type);

	std::string GetPlayerUpgradeString() const;

	void DoUpdateBrightSideOfLife(bool value);

protected:

	float invulnerability_timer = 0.0f;

	bool brightsideoflife = false;

	float brightsideoflife_timer = 0.0f;

	std::vector<chaos::shared_ptr<PlayerUpgrade>> upgrades;

	float fire_timer = 0.0f; 
	float dash_timer = 0.0f; 
	float dash_cooldown = 0.0f; 

	bool  dash_locked = false; // locked until key is released
};
