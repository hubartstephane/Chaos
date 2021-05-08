namespace chaos
{
#ifdef CHAOS_FORWARD_DECLARATION

	class GPUFileResource;

#elif !defined CHAOS_TEMPLATE_IMPLEMENTATION

	class GPUFileResource : public GPUResource, public NamedObject, public FileResource
	{
	};

#endif

}; // namespace chaos