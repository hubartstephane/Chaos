#ifdef CHAOS_FORWARD_DECLARATION

namespace chaos
{
	class GPUFileResource;

}; // namespace chaos

#else 

namespace chaos
{
	class GPUFileResource : public GPUResource, public NamedObject, public FileResource
	{
	};

}; // namespace chaos

#endif // CHAOS_FORWARD_DECLARATION

