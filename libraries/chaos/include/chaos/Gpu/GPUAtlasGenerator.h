namespace chaos
{
#ifdef CHAOS_FORWARD_DECLARATION

	class GPUAtlasGenerator;

#elif !defined CHAOS_TEMPLATE_IMPLEMENTATION

	/**
	* GPUAtlasGenerator
	*/

	class CHAOS_API GPUAtlasGenerator
	{

	public:

		/** compute all AtlasBitmapInfo positions */
		GPUAtlas* ComputeResult(BitmapAtlas::AtlasInput const& in_input, BitmapAtlas::AtlasGeneratorParams const& in_params = {});
	};

#endif

}; // namespace chaos