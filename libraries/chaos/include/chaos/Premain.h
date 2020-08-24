#pragma once

#include <chaos/StandardHeaders.h>


#define CHAOS_PREMAIN static bool const BOOST_PP_CAT(premain_, __LINE__) = chaos::RegisterPremainCode

namespace chaos
{
	/** register a Premain code */
	bool RegisterPremainCode(int order, std::function<void()> func);
	/** call code for all code in premain */
	void FlushPremainCalls();

}; // namespace chaos

