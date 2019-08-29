#pragma once

#include <chaos/StandardHeaders.h>
#include <chaos/GPUClasses.h>

namespace chaos
{
	/**
	* GPUInstancingInfo
	*/

	class GPUInstancingInfo
	{
	public:

		/** constructor */
		GPUInstancingInfo(int in_instance_count = 0, int in_base_instance = 0):
			instance_count(in_instance_count),
			base_instance(in_base_instance)
		{}

		/** number of instance */
		int instance_count = 0;
		/** the starting index of instance */
		int base_instance = 0;
	};

}; // namespace chaos

