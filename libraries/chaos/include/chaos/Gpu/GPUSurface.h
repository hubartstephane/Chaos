namespace chaos
{
#ifdef CHAOS_FORWARD_DECLARATION

	class GPUSurface;

#elif !defined CHAOS_TEMPLATE_IMPLEMENTATION

	class CHAOS_API GPUSurface : public GPUResource
	{
	public:

		/** get the description of the surface */
		virtual SurfaceDescription const& GetSurfaceDescription() const = 0;
	};

#endif

}; // namespace chaos