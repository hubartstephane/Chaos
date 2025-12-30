#pragma once

#include "Ludum41JujuAndBarbPCH.h"
#include "Ludum41JujuAndBarbChallengeRewardPunishment.h"
#include "Ludum41JujuAndBarbGame.h"
#include "Ludum41JujuAndBarbGameInstance.h"

// =========================================================

char const * LudumChallengeRewardPunishment_ExtraLife::GetTitleName(LudumGameInstance * game_instance, bool reward) const
{
	if (reward)
		return "Extra Life";
	else
		return "Life Lost";
}

bool LudumChallengeRewardPunishment_ExtraLife::IsRewardPunishmentValid(LudumGameInstance * game_instance, bool reward) const
{
	assert(game_instance != nullptr);
	return game_instance->IsExtraBallChallengeValid(reward);
}

void LudumChallengeRewardPunishment_ExtraLife::OnRewardPunishment(LudumGameInstance * game_instance, bool reward)
{
	assert(game_instance != nullptr);
	game_instance->OnExtraBallChallenge(reward);
}

// =========================================================

char const * LudumChallengeRewardPunishment_BarSize::GetTitleName(LudumGameInstance * game_instance, bool reward) const
{
	if (reward)
		return "Extend Bar";
	else
		return "Reduce Bar";
}

bool LudumChallengeRewardPunishment_BarSize::IsRewardPunishmentValid(LudumGameInstance * game_instance, bool reward) const
{
	assert(game_instance != nullptr);
	return game_instance->IsLongBarChallengeValid(reward);
}

void LudumChallengeRewardPunishment_BarSize::OnRewardPunishment(LudumGameInstance * game_instance, bool reward)
{
	assert(game_instance != nullptr);
	game_instance->OnLongBarChallenge(reward);
}

// =========================================================

char const * LudumChallengeRewardPunishment_BrickLife::GetTitleName(LudumGameInstance * game_instance, bool reward) const
{
	if (reward)
		return "Hurt Bricks";
	else
		return "Heal Bricks";
}

bool LudumChallengeRewardPunishment_BrickLife::IsRewardPunishmentValid(LudumGameInstance * game_instance, bool reward) const
{
	assert(game_instance != nullptr);
	return game_instance->IsBrickLifeChallengeValid(reward);;
}

void LudumChallengeRewardPunishment_BrickLife::OnRewardPunishment(LudumGameInstance * game_instance, bool reward)
{
	assert(game_instance != nullptr);
	game_instance->OnBrickLifeChallenge(reward);
}

// =========================================================

char const * LudumChallengeRewardPunishment_SpeedDownBall::GetTitleName(LudumGameInstance * game_instance, bool reward) const
{
	if (reward)
		return "Speed Down";
	else
		return "Speed Up";
}

bool LudumChallengeRewardPunishment_SpeedDownBall::IsRewardPunishmentValid(LudumGameInstance * game_instance, bool reward) const
{
	assert(game_instance != nullptr);
	return game_instance->IsBallSpeedChallengeValid(reward);
}

void LudumChallengeRewardPunishment_SpeedDownBall::OnRewardPunishment(LudumGameInstance * game_instance, bool reward)
{
	assert(game_instance != nullptr);
	game_instance->OnBallSpeedChallenge(reward);
}

// =========================================================

char const * LudumChallengeRewardPunishment_SplitBall::GetTitleName(LudumGameInstance * game_instance, bool reward) const
{
	if (reward)
		return "Multi Ball";
	return nullptr;
}

bool LudumChallengeRewardPunishment_SplitBall::IsRewardPunishmentValid(LudumGameInstance * game_instance, bool reward) const
{
	assert(game_instance != nullptr);
	return game_instance->IsSplitBallChallengeValid(reward);;
}

void LudumChallengeRewardPunishment_SplitBall::OnRewardPunishment(LudumGameInstance * game_instance, bool reward)
{
	assert(game_instance != nullptr);
	game_instance->OnSplitBallChallenge(reward);
}

// =========================================================

char const * LudumChallengeRewardPunishment_BallPower::GetTitleName(LudumGameInstance * game_instance, bool reward) const
{
	if (reward)
		return "Power Up";
	else
		return "Power Down";
}

bool LudumChallengeRewardPunishment_BallPower::IsRewardPunishmentValid(LudumGameInstance * game_instance, bool reward) const
{
	assert(game_instance != nullptr);
	return game_instance->IsBallPowerChallengeValid(reward);
}

void LudumChallengeRewardPunishment_BallPower::OnRewardPunishment(LudumGameInstance * game_instance, bool reward)
{
	assert(game_instance != nullptr);
	game_instance->OnBallPowerChallenge(reward);
}

// =========================================================

char const * LudumChallengeRewardPunishment_BrickOffset::GetTitleName(LudumGameInstance * game_instance, bool reward) const
{
	if (reward)
		return "Bricks Go Up";
	else
		return "Bricks Go Down";
}

bool LudumChallengeRewardPunishment_BrickOffset::IsRewardPunishmentValid(LudumGameInstance * game_instance, bool reward) const
{
	assert(game_instance != nullptr);
	return game_instance->IsBrickOffsetChallengeValid(reward);
}

void LudumChallengeRewardPunishment_BrickOffset::OnRewardPunishment(LudumGameInstance * game_instance, bool reward)
{
	assert(game_instance != nullptr);
	game_instance->OnBrickOffsetChallenge(reward);
}




