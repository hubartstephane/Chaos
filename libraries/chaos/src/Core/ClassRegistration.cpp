#include "chaos/ChaosPCH.h"
#include "chaos/ChaosInternals.h"

namespace chaos
{
	ClassRegistration & ClassRegistration::ShortName(std::string in_short_name)
	{
		class_ptr->SetShortName(std::move(in_short_name));
		return *this;
	}

}; // namespace chaos
