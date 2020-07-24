#pragma once

#include <chaos/StandardHeaders.h>
#include <chaos/buffer.h>


namespace chaos
{
	namespace DebugTools
	{
		/** display the callstack of the current application */
		void DisplayCallStack(std::ostream & stream);

	}; // namespace DebugTools

}; // namespace chaos

