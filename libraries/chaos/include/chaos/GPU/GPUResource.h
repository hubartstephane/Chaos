namespace chaos
{
#ifdef CHAOS_FORWARD_DECLARATION

	class GPUResource;

#elif !defined CHAOS_TEMPLATE_IMPLEMENTATION

	// ==============================================================
	// GPUResource
	// ==============================================================

	class CHAOS_API GPUResource : public Object, public IdentifiableInterface
	{
	public:

		/** destructor */
		virtual ~GPUResource() = default;
		/** cleaning the resource */
		virtual void Release();
	};

#endif

}; // namespace chaos