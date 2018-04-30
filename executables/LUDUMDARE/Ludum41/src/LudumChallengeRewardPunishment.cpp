#pragma once

#include "LudumChallengeRewardPunishment.h"
#include "LudumGame.h"

void LudumChallengeRewardPunishment_ExtraBall::OnRewardPunishment(class LudumGame * game, bool reward)
{
	if (game != nullptr)
		game->OnExtraBallChallenge(reward);
}

void LudumChallengeRewardPunishment_LongBarBall::OnRewardPunishment(class LudumGame * game, bool reward)
{
	if (game != nullptr)
		game->OnLongBarChallenge(reward);
}

void LudumChallengeRewardPunishment_LifeBall::OnRewardPunishment(class LudumGame * game, bool reward)
{
	if (game != nullptr)
		game->OnLifeChallenge(reward);
}

void LudumChallengeRewardPunishment_SpeedDownBall::OnRewardPunishment(class LudumGame * game, bool reward)
{
	if (game != nullptr)
		game->OnBallSpeedChallenge(reward);
}

void LudumChallengeRewardPunishment_SplitBall::OnRewardPunishment(class LudumGame * game, bool reward)
{
	if (game != nullptr)
		game->OnSplitBallChallenge(reward);
}







