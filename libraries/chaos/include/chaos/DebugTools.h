#pragma once

#include <chaos/StandardHeaders.h>
#include <chaos/buffer.h>


namespace chaos
{

	/**
	* DebugTools is an utility class for debugging
	*/
	class DebugTools
	{
	public:

		/** display the callstack of the current application */
		static void DisplayCallStack(std::ostream & stream);
	};

}; // namespace chaos

