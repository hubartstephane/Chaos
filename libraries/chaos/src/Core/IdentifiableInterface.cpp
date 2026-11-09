#include "chaos/ChaosPCH.h"
#include "chaos/ChaosInternals.h"

namespace chaos
{
	IdentifiableInterface::IdentifiableInterface()
	{
		static std::atomic<uint64_t> next_id = 0;
		unique_id = ++next_id;
	}

}; // namespace chaos
