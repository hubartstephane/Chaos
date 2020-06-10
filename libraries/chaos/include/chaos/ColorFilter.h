#pragma once

#include <chaos/StandardHeaders.h>
#include <chaos/GeometryFramework.h>
#include <chaos/PixelTypes.h>
#include <chaos/JSONTools.h>

namespace chaos
{

	/**
	* ColorFilter : filter out some pixel 
	*/

	class ColorFilter
	{

	public:

		template<typename T>
		bool Filter(T const& color) const
		{
			// check R     => range
			// check G     => range
			// check B     => range
			// check A     => range

			// check distance color-color


			if constexpr (std::is_same_v<T, PixelBGRA>)
				return (color.A == 0);

			if constexpr (std::is_same_v<T, PixelBGRA>)
				return (color.R + color.G + color.B < 100);

			return false;
		}


	public:

		/** whether to reverse the color range check */
		bool reverse_range_check = false;
		/** the minimum range per component */
		glm::vec4 min_color_range = glm::vec4(0.0f, 0.0f, 0.0f, 0.0f);
		/** the maximum range per component */
		glm::vec4 max_color_range = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);

		/** whether to reverse the color distance check */
		bool reverse_distance_check = false;
		/** a reference color to check distance with */
		glm::vec4 reference_color = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);
		/** distance to reference color */
		float reference_distance = 1.0f;
	};

	bool SaveIntoJSON(nlohmann::json& json_entry, ColorFilter const& src);

	bool LoadFromJSON(nlohmann::json const& json_entry, ColorFilter& dst);

}; // namespace chaos
