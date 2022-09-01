namespace chaos
{
#ifdef CHAOS_FORWARD_DECLARATION

	enum class WrapMode;

#elif !defined CHAOS_TEMPLATE_IMPLEMENTATION

	enum class CHAOS_API WrapMode : int
	{
		NONE = 0,
		CLAMP = 1,
		WRAP = 2,
		CHECK_RANGES = 3
	};

	// apply the wrap mode (return whether this is not an out of bounds)
	template<typename T>
	CHAOS_API bool ApplyWrapMode(T src, T min_value, T max_value, WrapMode mode, T& result)
	{
		if (mode == WrapMode::NONE)
		{
			result = src;
		}
		else if (mode == WrapMode::CLAMP)
		{
			result = std::clamp(src, min_value, max_value);
		}
		else if (mode == WrapMode::WRAP)
		{
			if (std::is_integral_v<T>)
			{
				result = min_value + MathTools::Modulo(src - min_value, max_value - min_value + 1); // +1 because 'max_value' is inside accepted range
			}
			else
			{
				result = min_value + MathTools::Modulo(src - min_value, max_value - min_value); // XXX : beware wrap mode for floating values does not work identicaly than for integers
			}
		}
		else if (mode == WrapMode::CHECK_RANGES)
		{
			if (src < min_value)
				return false;
			if (src > max_value)
				return false;
			result = src;
		}
		return true; // except for ranges, this function should always be successful
	}

	template<typename T>
	CHAOS_API bool ApplyWrapMode(glm::tvec1<T> const& src, glm::tvec1<T> const& min_value, glm::tvec1<T> const& max_value, WrapMode mode, glm::tvec1<T>& result)
	{
		for (size_t i = 0; i < src.length(); ++i)
			if (!ApplyWrapMode(src[i], min_value[i], max_value[i], mode, result[i]))
				return false;
		return true;
	}

	template<typename T>
	CHAOS_API bool ApplyWrapMode(glm::tvec2<T> const& src, glm::tvec2<T> const& min_value, glm::tvec2<T> const& max_value, WrapMode mode, glm::tvec2<T>& result)
	{
		for (size_t i = 0; i < src.length(); ++i)
			if (!ApplyWrapMode(src[i], min_value[i], max_value[i], mode, result[i]))
				return false;
		return true;
	}

	template<typename T>
	CHAOS_API bool ApplyWrapMode(glm::tvec3<T> const& src, glm::tvec3<T> const& min_value, glm::tvec3<T> const& max_value, WrapMode mode, glm::tvec3<T>& result)
	{
		for (size_t i = 0; i < src.length(); ++i)
			if (!ApplyWrapMode(src[i], min_value[i], max_value[i], mode, result[i]))
				return false;
		return true;
	}

	template<typename T>
	CHAOS_API bool ApplyWrapMode(glm::tvec4<T> src, glm::tvec4<T> const& min_value, glm::tvec4<T> const& max_value, WrapMode mode, glm::tvec4<T>& result)
	{
		for (size_t i = 0; i < src.length(); ++i)
			if (!ApplyWrapMode(src[i], min_value[i], max_value[i], mode, result[i]))
				return false;
		return true;
	}

	CHAOS_DECLARE_ENUM_METHOD(WrapMode);

#endif

}; // namespace chaos