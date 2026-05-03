namespace chaos
{
#ifdef CHAOS_FORWARD_DECLARATION

	class GPUInstancingInfo;

#elif !defined CHAOS_TEMPLATE_IMPLEMENTATION

	/**
	* GPUInstancingInfo
	*/

	class CHAOS_API GPUInstancingInfo
	{
	public:

		/** constructor */
		GPUInstancingInfo(int in_instance_count = 0, int in_base_instance = 0) :
			instance_count(in_instance_count),
			base_instance(in_base_instance)
		{}

		/** number of instance */
		int instance_count = 0;
		/** the starting index of instance */
		int base_instance = 0;
	};

#endif

}; // namespace chaos