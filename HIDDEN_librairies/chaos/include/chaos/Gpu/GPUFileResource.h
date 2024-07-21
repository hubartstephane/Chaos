namespace chaos
{
#ifdef CHAOS_FORWARD_DECLARATION

	class GPUFileResource;

#elif !defined CHAOS_TEMPLATE_IMPLEMENTATION

	class CHAOS_API GPUFileResource : public GPUResource, public NamedInterface, public FileResource
	{
	};

#endif

}; // namespace chaos