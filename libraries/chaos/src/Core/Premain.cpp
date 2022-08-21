
#include "chaos/Chaos.h"

namespace chaos
{
	// use a function instead of a static data to ensure the vector is properly constructed before accessing it
	static std::vector<std::pair<int, std::function<void()>>> & GetPendingPremainCalls()
	{
		static std::vector<std::pair<int, std::function<void()>>> result;
		return result;	
	}
	
	bool RegisterPremainCode(int order, std::function<void()> func)
	{
		auto & pending_premain_calls = GetPendingPremainCalls();
		pending_premain_calls.push_back(std::make_pair(order, func));
		return true;
	}

	void FlushPremainCalls()
	{
		auto& pending_premain_calls = GetPendingPremainCalls();

		// order the elements by increasing orders
		std::sort(pending_premain_calls.begin(), pending_premain_calls.end(), [](auto src1, auto src2)
		{
			return src1.first < src2.first;
		});
		// make the calls
		for (auto& element : pending_premain_calls)
			element.second();
		// clear the list
		pending_premain_calls.clear();
	}

}; // namespace chaos
