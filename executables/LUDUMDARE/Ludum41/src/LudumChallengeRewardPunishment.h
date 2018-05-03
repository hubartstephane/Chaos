#pragma once

#include <chaos/StandardHeaders.h> 
#include <chaos/ReferencedObject.h>

class LudumChallengeRewardPunishment : public chaos::ReferencedObject
{
public:

	/** called to know whether the reward / punishment is valid */
	virtual bool IsRewardPunishmentValid(class LudumGame * game, bool reward) const { return true; }
	/** called to know whether the reward punishment must wait for an in game condition */
	virtual bool CheckRewardPunishmentCondition(class LudumGame * game, bool reward) const { return true; }
	/** called whenever the challenge is completed */
	virtual void OnRewardPunishment(class LudumGame * game, bool reward) {}
};

class LudumChallengeRewardPunishment_ExtraBall : public LudumChallengeRewardPunishment
{
public:

	/** called to know whether the reward / punishment is valid */
	virtual bool IsRewardPunishmentValid(class LudumGame * game, bool reward) const override;
	/** called whenever the challenge is completed */
	virtual void OnRewardPunishment(class LudumGame * game, bool reward) override;
};




class LudumChallengeRewardPunishment_LongBarBall : public LudumChallengeRewardPunishment
{
public:

	/** called to know whether the reward / punishment is valid */
	virtual bool IsRewardPunishmentValid(class LudumGame * game, bool reward) const override;
	/** called whenever the challenge is completed */
	virtual void OnRewardPunishment(class LudumGame * game, bool reward) override;
};





class LudumChallengeRewardPunishment_BrickLife : public LudumChallengeRewardPunishment
{
public:

	/** called to know whether the reward / punishment is valid */
	virtual bool IsRewardPunishmentValid(class LudumGame * game, bool reward) const override;
	/** called whenever the challenge is completed */
	virtual void OnRewardPunishment(class LudumGame * game, bool reward) override;
};





class LudumChallengeRewardPunishment_SpeedDownBall : public LudumChallengeRewardPunishment
{
public:

	/** called to know whether the reward / punishment is valid */
	virtual bool IsRewardPunishmentValid(class LudumGame * game, bool reward) const override;
	/** called whenever the challenge is completed */
	virtual void OnRewardPunishment(class LudumGame * game, bool reward) override;
};





class LudumChallengeRewardPunishment_SplitBall : public LudumChallengeRewardPunishment
{
public:

	/** called to know whether the reward / punishment is valid */
	virtual bool IsRewardPunishmentValid(class LudumGame * game, bool reward) const override;
	/** called whenever the challenge is completed */
	virtual void OnRewardPunishment(class LudumGame * game, bool reward) override;
};
