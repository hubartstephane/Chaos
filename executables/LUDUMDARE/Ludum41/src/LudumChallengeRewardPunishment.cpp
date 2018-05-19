#pragma once

#include "LudumChallengeRewardPunishment.h"
#include "LudumGame.h"

// =========================================================

char const * LudumChallengeRewardPunishment_ExtraLife::GetTitleName(class LudumGame * game, bool reward) const 
{
	if (reward)
		return "Extra Life";
	else
		return "Life Lost";
}

bool LudumChallengeRewardPunishment_ExtraLife::IsRewardPunishmentValid(class LudumGame * game, bool reward) const
{
	assert(game != nullptr);
	return game->IsExtraBallChallengeValid(reward);
}

void LudumChallengeRewardPunishment_ExtraLife::OnRewardPunishment(class LudumGame * game, bool reward)
{
	assert(game != nullptr);
	game->OnExtraBallChallenge(reward);
}

// =========================================================

char const * LudumChallengeRewardPunishment_BarSize::GetTitleName(class LudumGame * game, bool reward) const 
{
	if (reward)
		return "Extend Bar";
	else
		return "Reduce Bar";
}

bool LudumChallengeRewardPunishment_BarSize::IsRewardPunishmentValid(class LudumGame * game, bool reward) const 
{
	assert(game != nullptr);
	return game->IsLongBarChallengeValid(reward);
}

void LudumChallengeRewardPunishment_BarSize::OnRewardPunishment(class LudumGame * game, bool reward)
{
	assert(game != nullptr);	
	game->OnLongBarChallenge(reward);
}

// =========================================================

char const * LudumChallengeRewardPunishment_BrickLife::GetTitleName(class LudumGame * game, bool reward) const 
{
	if (reward)
		return "Hurt Bricks";
	else
		return "Heal Bricks";
}

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

char const * LudumChallengeRewardPunishment_SpeedDownBall::GetTitleName(class LudumGame * game, bool reward) const 
{
	if (reward)
		return "Speed Down";
	else
		return "Speed Up";
}

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

char const * LudumChallengeRewardPunishment_SplitBall::GetTitleName(class LudumGame * game, bool reward) const 
{
	if (reward)
		return "Multi Ball";
	return nullptr;
}

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

// =========================================================

char const * LudumChallengeRewardPunishment_BallPower::GetTitleName(class LudumGame * game, bool reward) const
{
	if (reward)
		return "Power Up"; 
	else
		return "Power Down";		
}

bool LudumChallengeRewardPunishment_BallPower::IsRewardPunishmentValid(class LudumGame * game, bool reward) const
{
	assert(game != nullptr);
	return game->IsBallPowerChallengeValid(reward);
}

void LudumChallengeRewardPunishment_BallPower::OnRewardPunishment(class LudumGame * game, bool reward)
{
	assert(game != nullptr);
	game->OnBallPowerChallenge(reward);
}

// =========================================================

char const * LudumChallengeRewardPunishment_BrickOffset::GetTitleName(class LudumGame * game, bool reward) const
{
	if (reward)
		return "Bricks Go Up";
	else
		return "Bricks Go Down";
}

bool LudumChallengeRewardPunishment_BrickOffset::IsRewardPunishmentValid(class LudumGame * game, bool reward) const
{
	assert(game != nullptr);
	return game->IsBrickOffsetChallengeValid(reward);
}

void LudumChallengeRewardPunishment_BrickOffset::OnRewardPunishment(class LudumGame * game, bool reward)
{
	assert(game != nullptr);
	game->OnBrickOffsetChallenge(reward);
}




