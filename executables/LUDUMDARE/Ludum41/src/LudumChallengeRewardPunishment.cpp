#pragma once

#include "LudumChallengeRewardPunishment.h"
#include "LudumGame.h"


bool LudumChallengeRewardPunishment_ExtraBall::IsRewardPunishmentValid(class LudumGame * game, bool reward) const 
{
	



	return true;
}

void LudumChallengeRewardPunishment_ExtraBall::OnRewardPunishment(class LudumGame * game, bool reward)
{
	if (game != nullptr)
		game->OnExtraBallChallenge(reward);
}












bool LudumChallengeRewardPunishment_LongBarBall::IsRewardPunishmentValid(class LudumGame * game, bool reward) const 
{







	return true;
}

void LudumChallengeRewardPunishment_LongBarBall::OnRewardPunishment(class LudumGame * game, bool reward)
{
	if (game != nullptr)
		game->OnLongBarChallenge(reward);
}





bool LudumChallengeRewardPunishment_LifeBall::IsRewardPunishmentValid(class LudumGame * game, bool reward) const 
{









	return true;
}

void LudumChallengeRewardPunishment_LifeBall::OnRewardPunishment(class LudumGame * game, bool reward)
{
	if (game != nullptr)
		game->OnLifeChallenge(reward);
}





bool LudumChallengeRewardPunishment_SpeedDownBall::IsRewardPunishmentValid(class LudumGame * game, bool reward) const 
{









	return true;
}

void LudumChallengeRewardPunishment_SpeedDownBall::OnRewardPunishment(class LudumGame * game, bool reward)
{
	if (game != nullptr)
		game->OnBallSpeedChallenge(reward);
}



bool LudumChallengeRewardPunishment_SplitBall::IsRewardPunishmentValid(class LudumGame * game, bool reward) const 
{








	return true;
}

void LudumChallengeRewardPunishment_SplitBall::OnRewardPunishment(class LudumGame * game, bool reward)
{
	if (game != nullptr)
		game->OnSplitBallChallenge(reward);
}







