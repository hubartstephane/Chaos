#include <death/GameHUD.h>

namespace death
{
	void PlayingHUD::SetScoreValue(int new_score)
	{
		if (new_score < 0)
		{
			score_allocations = nullptr;
		}
		else if (cached_score_value != new_score)
		{

		}
		cached_score_value = new_score;
	}


}; // namespace death

