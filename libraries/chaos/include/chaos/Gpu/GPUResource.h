#ifdef CHAOS_FORWARD_DECLARATION

namespace chaos
{
	class GPUResource;

}; // namespace chaos

#elif defined CHAOS_TEMPLATE_IMPLEMENTATION


#else 

namespace chaos
{
	// ==============================================================
	// GPUResource
	// ==============================================================

	class GPUResource : public Object
	{
	public:

		/** destructor */
		virtual ~GPUResource();
		/** cleaning the resource */
		virtual void Release();
	};

}; // namespace chaos

#endif // CHAOS_FORWARD_DECLARATION



