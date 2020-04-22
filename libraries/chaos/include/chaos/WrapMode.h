#pragma once

#include <chaos/StandardHeaders.h>

namespace chaos
{
	enum class WrapMode : int
	{
		none = 0,
		clamp = 1,
		wrap = 2,
		check_ranges = 3
	};

	// apply the wrap mode (return whether this is not an out of bounds)
	template<typename T>
	bool ApplyWrapMode(T src, std::pair<T, T> const& range, WrapMode mode, T & result)
	{
		if (mode == WrapMode::none)
		{
			result = src;
		}
		else if (mode == WrapMode::clamp)
		{
			result = std::clamp(src, range.first, range.second);
		}
		else if (mode == WrapMode::wrap)
		{
			result = range.first + MathTools::Modulo(result, range.second - range.first);
		}
		else if (mode == WrapMode::check_ranges)
		{
			if (src < range.first)
				return false;
			if (src > range.second)
				return false;
			result = src;
		}
		return true; // except for ranges, this function should always be successful 
	}

	template<typename T>
	bool ApplyWrapMode(glm::tvec1<T> src, std::pair<glm::tvec1<T>, glm::tvec1<T>> const& range, WrapMode mode, glm::tvec1<T> & result)
	{
		for (size_t i = 0; i < src.length(); ++i)
			if (!ApplyWrapMode(src[i], std::make_pair(range.first[i], range.second[i]), mode, result))
				return false;
		return true;
	}

	template<typename T>
	bool ApplyWrapMode(glm::tvec2<T> src, std::pair<glm::tvec2<T>, glm::tvec1<T>> const& range, WrapMode mode, glm::tvec2<T>& result)
	{
		for (size_t i = 0; i < src.length(); ++i)
			if (!ApplyWrapMode(src[i], std::make_pair(range.first[i], range.second[i]), mode, result))
				return false;
		return true;
	}

	template<typename T>
	bool ApplyWrapMode(glm::tvec3<T> src, std::pair<glm::tvec3<T>, glm::tvec1<T>> const& range, WrapMode mode, glm::tvec3<T>& result)
	{
		for (size_t i = 0; i < src.length(); ++i)
			if (!ApplyWrapMode(src[i], std::make_pair(range.first[i], range.second[i]), mode, result))
				return false;
		return true;
	}

	template<typename T>
	bool ApplyWrapMode(glm::tvec4<T> src, std::pair<glm::tvec4<T>, glm::tvec4<T>> const& range, WrapMode mode, glm::tvec4<T>& result)
	{
		for (size_t i = 0; i < src.length(); ++i)
			if (!ApplyWrapMode(src[i], std::make_pair(range.first[i], range.second[i]), mode, result))
				return false;
		return true;
	}

	/** JSON loading method */
	bool LoadFromJSON(nlohmann::json const& json_entry, WrapMode& dst);
	/** JSON saving method */
	bool SaveIntoJSON(nlohmann::json& json_entry, WrapMode const& src);

}; // namespace chaos
