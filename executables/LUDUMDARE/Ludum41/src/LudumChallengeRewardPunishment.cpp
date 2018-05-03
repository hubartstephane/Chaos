#pragma once

#include "LudumChallengeRewardPunishment.h"
#include "LudumGame.h"

// =========================================================

bool LudumChallengeRewardPunishment_ExtraBall::IsRewardPunishmentValid(class LudumGame * game, bool reward) const 
{
	assert(game != nullptr);
	return game->IsExtraBallChallengeValid(reward);
}

void LudumChallengeRewardPunishment_ExtraBall::OnRewardPunishment(class LudumGame * game, bool reward)
{
	assert(game != nullptr);
	game->OnExtraBallChallenge(reward);
}

// =========================================================

bool LudumChallengeRewardPunishment_LongBarBall::IsRewardPunishmentValid(class LudumGame * game, bool reward) const 
{
	assert(game != nullptr);
	return game->IsLongBarChallengeValid(reward);
}

void LudumChallengeRewardPunishment_LongBarBall::OnRewardPunishment(class LudumGame * game, bool reward)
{
	assert(game != nullptr);	
	game->OnLongBarChallenge(reward);
}

// =========================================================

bool LudumChallengeRewardPunishment_BrickLife::IsRewardPunishmentValid(class LudumGame * game, bool reward) const 
{
	assert(game != nullptr);
	return game->IsBrickLifeChallengeValid(reward);;
}

void LudumChallengeRewardPunishment_BrickLife::OnRewardPunishment(class LudumGame * game, bool reward)
{
	assert(game != nullptr);	
	game->OnBrickLifeChallenge(reward);
}

// =========================================================

bool LudumChallengeRewardPunishment_SpeedDownBall::IsRewardPunishmentValid(class LudumGame * game, bool reward) const 
{
	assert(game != nullptr);
	return game->IsBallSpeedChallengeValid(reward);
}

void LudumChallengeRewardPunishment_SpeedDownBall::OnRewardPunishment(class LudumGame * game, bool reward)
{
	assert(game != nullptr);	
	game->OnBallSpeedChallenge(reward);
}

// =========================================================

bool LudumChallengeRewardPunishment_SplitBall::IsRewardPunishmentValid(class LudumGame * game, bool reward) const 
{
	assert(game != nullptr);
	return game->IsSplitBallChallengeValid(reward);;
}

void LudumChallengeRewardPunishment_SplitBall::OnRewardPunishment(class LudumGame * game, bool reward)
{
	assert(game != nullptr);
	game->OnSplitBallChallenge(reward);
}







