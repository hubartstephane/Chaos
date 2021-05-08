namespace chaos
{
#ifdef CHAOS_FORWARD_DECLARATION

	class GPUSurface;

#elif !defined CHAOS_TEMPLATE_IMPLEMENTATION

	class GPUSurface : public GPUFileResource
	{
	public:

		/** get the description of the surface */
		virtual SurfaceDescription const& GetSurfaceDescription() const = 0;
	};

#endif

}; // namespace chaos