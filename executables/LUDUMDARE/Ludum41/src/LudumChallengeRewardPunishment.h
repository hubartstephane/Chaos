#pragma once

#include <chaos/StandardHeaders.h> 
#include <chaos/ReferencedObject.h>

class LudumChallengeRewardPunishment : public chaos::ReferencedObject
{
public:

	/** called whenever the challenge is completed */
	virtual void OnRewardPunishment(class LudumGame * game, bool reward) {}
};

class LudumChallengeRewardPunishment_ExtraBall : public LudumChallengeRewardPunishment
{
public:

	/** called whenever the challenge is completed */
	virtual void OnRewardPunishment(class LudumGame * game, bool reward) override;
};

class LudumChallengeRewardPunishment_LongBarBall : public LudumChallengeRewardPunishment
{
public:

	/** called whenever the challenge is completed */
	virtual void OnRewardPunishment(class LudumGame * game, bool reward) override;
};


class LudumChallengeRewardPunishment_LifeBall : public LudumChallengeRewardPunishment
{
public:

	/** called whenever the challenge is completed */
	virtual void OnRewardPunishment(class LudumGame * game, bool reward) override;
};

class LudumChallengeRewardPunishment_SpeedDownBall : public LudumChallengeRewardPunishment
{
public:

	/** called whenever the challenge is completed */
	virtual void OnRewardPunishment(class LudumGame * game, bool reward) override;
};


class LudumChallengeRewardPunishment_SplitBall : public LudumChallengeRewardPunishment
{
public:

	/** called whenever the challenge is completed */
	virtual void OnRewardPunishment(class LudumGame * game, bool reward) override;
};
