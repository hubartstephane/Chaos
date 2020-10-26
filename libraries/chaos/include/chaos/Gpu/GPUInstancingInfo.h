#ifdef CHAOS_FORWARD_DECLARATION

namespace chaos
{
	class GPUInstancingInfo;

}; // namespace chaos

#else 

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

#endif // CHAOS_FORWARD_DECLARATION

