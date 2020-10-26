#ifdef CHAOS_FORWARD_DECLARATION

namespace chaos
{
	class GPUSurface;

}; // namespace chaos

#else 

namespace chaos
{

	class GPUSurface : public GPUFileResource
	{
	public:

		/** get the description of the surface */
		virtual SurfaceDescription const & GetSurfaceDescription() const = 0;
	};

}; // namespace chaos

#endif // CHAOS_FORWARD_DECLARATION



