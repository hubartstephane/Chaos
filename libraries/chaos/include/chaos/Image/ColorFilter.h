namespace chaos
{
#ifdef CHAOS_FORWARD_DECLARATION

	class ColorFilter;

#elif !defined CHAOS_TEMPLATE_IMPLEMENTATION

	/**
	* ColorFilter : filter out some pixel
	*/

	class ColorFilter
	{

	public:

		/** filter the incomming color */
		bool Filter(glm::vec4 const& color) const;

		/** filter the incomming color */
		template<typename T>
		bool Filter(T const& color) const
		{
			PixelRGBAFloat c;
			PixelConverter::Convert(c, color);
			return Filter((glm::vec4)c);
		}

	public:

		/** distance to reference color */
		float distance = 0.0f;
		/** the operator to apply to the distance */
		ComparisonOperator distance_operator = ComparisonOperator::GREATER;
		/** a reference color to check distance with */
		glm::vec4 color_reference = { 0.0f, 0.0f, 0.0f, 0.0f };
		/** the mask to apply to incomming color before checking for distance */
		glm::vec4 color_mask = { 0.0f, 0.0f, 0.0f, 1.0f };
	};

	bool SaveIntoJSON(nlohmann::json& json_entry, ColorFilter const& src);

	bool LoadFromJSON(nlohmann::json const& json_entry, ColorFilter& dst);

#endif

}; // namespace chaos