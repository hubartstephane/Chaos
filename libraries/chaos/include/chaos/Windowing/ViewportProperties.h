namespace chaos
{
#ifdef CHAOS_FORWARD_DECLARATION

	class ViewportProperties;

#elif !defined CHAOS_TEMPLATE_IMPLEMENTATION

	/**
	 * Some geometric restriction for a viewport
	 */
	class CHAOS_API ViewportProperties
	{
	public:

		/** the padding of the viewport */
		Padding padding;
		/** whether the aspect_ratio of the viewport has to be forced */
		std::optional<float> aspect_ratio;
	};

#endif

}; // namespace chaos