#ifdef CHAOS_FORWARD_DECLARATION

namespace chaos
{
	class GPUResource;

}; // namespace chaos

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



