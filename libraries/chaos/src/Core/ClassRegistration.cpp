#include "chaos/ChaosPCH.h"
#pragma once

#include <chaos/chaos.h>

namespace chaos
{
	ClassRegistration & ClassRegistration::operator()(std::string in_short_name)
	{
		class_ptr->SetShortName(std::move(in_short_name));
		return *this;
	}

}; // namespace chaos
