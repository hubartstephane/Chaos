#pragma once

#include "Ludum41IsolationPCH.h"

class LudumChallengeRewardPunishment : public chaos::Object
{
public:

	/** get the name of the reward/punishment */
	virtual char const * GetTitleName(class LudumGameInstance * game_instance, bool reward) const { return nullptr;}
	/** called to know whether the reward / punishment is valid */
	virtual bool IsRewardPunishmentValid(class LudumGameInstance * game_instance, bool reward) const { return true; }
	/** called to know whether the reward punishment must wait for an in game condition */
	virtual bool CheckRewardPunishmentCondition(class LudumGameInstance * game_instance, bool reward) const { return true; }
	/** called whenever the challenge is completed */
	virtual void OnRewardPunishment(class LudumGameInstance * game_instance, bool reward) {}
};

class LudumChallengeRewardPunishment_ExtraLife : public LudumChallengeRewardPunishment
{
public:

	/** override */
	virtual char const * GetTitleName(class LudumGameInstance * game_instance, bool reward) const override;
	/** override */
	virtual bool IsRewardPunishmentValid(class LudumGameInstance * game_instance, bool reward) const override;
	/** override */
	virtual void OnRewardPunishment(class LudumGameInstance * game_instance, bool reward) override;
};




class LudumChallengeRewardPunishment_BarSize : public LudumChallengeRewardPunishment
{
public:

	/** override */
	virtual char const * GetTitleName(class LudumGameInstance * game_instance, bool reward) const override;
	/** override */
	virtual bool IsRewardPunishmentValid(class LudumGameInstance * game_instance, bool reward) const override;
	/** override */
	virtual void OnRewardPunishment(class LudumGameInstance * game_instance, bool reward) override;
};





class LudumChallengeRewardPunishment_BrickLife : public LudumChallengeRewardPunishment
{
public:

	/** override */
	virtual char const * GetTitleName(class LudumGameInstance * game_instance, bool reward) const override;
	/** override */
	virtual bool IsRewardPunishmentValid(class LudumGameInstance * game_instance, bool reward) const override;
	/** override */
	virtual void OnRewardPunishment(class LudumGameInstance * game_instance, bool reward) override;
};





class LudumChallengeRewardPunishment_SpeedDownBall : public LudumChallengeRewardPunishment
{
public:

	/** override */
	virtual char const * GetTitleName(class LudumGameInstance * game_instance, bool reward) const override;
	/** override */
	virtual bool IsRewardPunishmentValid(class LudumGameInstance * game_instance, bool reward) const override;
	/** override */
	virtual void OnRewardPunishment(class LudumGameInstance * game_instance, bool reward) override;
};





class LudumChallengeRewardPunishment_SplitBall : public LudumChallengeRewardPunishment
{
public:

	/** override */
	virtual char const * GetTitleName(class LudumGameInstance * game_instance, bool reward) const override;
	/** override */
	virtual bool IsRewardPunishmentValid(class LudumGameInstance * game_instance, bool reward) const override;
	/** override */
	virtual void OnRewardPunishment(class LudumGameInstance * game_instance, bool reward) override;
};


class LudumChallengeRewardPunishment_BallPower : public LudumChallengeRewardPunishment
{
public:

	/** override */
	virtual char const * GetTitleName(class LudumGameInstance * game_instance, bool reward) const override;
	/** override */
	virtual bool IsRewardPunishmentValid(class LudumGameInstance * game_instance, bool reward) const override;
	/** override */
	virtual void OnRewardPunishment(class LudumGameInstance * game_instance, bool reward) override;
};


class LudumChallengeRewardPunishment_BrickOffset : public LudumChallengeRewardPunishment
{
public:

	/** override */
	virtual char const * GetTitleName(class LudumGameInstance * game_instance, bool reward) const override;
	/** override */
	virtual bool IsRewardPunishmentValid(class LudumGameInstance * game_instance, bool reward) const override;
	/** override */
	virtual void OnRewardPunishment(class LudumGameInstance * game_instance, bool reward) override;
};