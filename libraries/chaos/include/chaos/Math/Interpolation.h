namespace chaos
{
#if defined CHAOS_FORWARD_DECLARATION 

	enum class InterpolationType;

#elif !defined CHAOS_TEMPLATE_IMPLEMENTATION

	/**
	 * InterpolationType: indicate which kind of interpolation to use
	 */

	enum class InterpolationType : int
	{
		LINEAR,
		EASE,
		EASIER,
		POW_2,
		POW_3,
		POW_5,
		SIN,
		EXP
	};

	CHAOS_DECLARE_ENUM_METHOD(InterpolationType, CHAOS_API);

	/** returns the linear interpolation between 2 values */
	template<typename T, typename SRC_TYPE>
	T Lerp(T t, SRC_TYPE a, SRC_TYPE b)
	{
		return ((static_cast<T>(1) - t) * a) + (t * b);
	}

	/** an Ease-In, Ease-out function */
	template<typename T>
	T Ease(T x)
	{
		T x2 = x * x;
		return static_cast<T>(3) * x2 - static_cast<T>(2) * x2 * x;
	}

	/** another Ease-In, Ease-out function */
	template<typename T>
	T Easier(T x)
	{
		T x3 = x * x * x;
		T x4 = x3 * x;
		T x5 = x4 * x;
		return
			static_cast<T>(6) * x5
			- static_cast<T>(15) * x4
			+ static_cast<T>(10) * x3;
	}

	/** some interpolation based on sin */
	template<typename T>
	T SinInterpolation(T x)
	{
		return
			std::sin(static_cast<T>(M_PI * 0.5) * x) /
			std::sin(static_cast<T>(M_PI * 0.5));
	}

	/** some interpolation based on exp */
	template<typename T>
	T ExpInterpolation(T x, T k = 1, T power = 3)
	{
		return
			(std::pow(static_cast<T>(M_E), k * std::pow(x, power)) - static_cast<T>(1)) /
			(std::pow(static_cast<T>(M_E), k) - static_cast<T>(1));
	}

	/** a generic interpolation factor getter */
	template<typename T>
	T InterpolationFactor(InterpolationType interpolation_type, T x)
	{
		switch (interpolation_type)
		{
		case InterpolationType::LINEAR:
			return x;
		case InterpolationType::EASE:
			return Ease(x);
		case InterpolationType::EASIER:
			return Easier(x);
		case InterpolationType::POW_2:
			return x * x;
		case InterpolationType::POW_3:
			return x * x * x;
		case InterpolationType::POW_5:
			return x * x * x * x * x;
		case InterpolationType::SIN:
			return SinInterpolation(x);
		case InterpolationType::EXP:
			return ExpInterpolation(x);
		default:
			assert(0);
			return {};
		}
	}

	/** a generic interpolation function */
	template<typename T, typename SRC_TYPE>
	T Interpolate(InterpolationType interpolation_type, T t, SRC_TYPE a, SRC_TYPE b)
	{
		return Lerp(InterpolationFactor(interpolation_type, t), a, b);
	}

	/** returns the cos interpolation between 2 values */
	template<typename T>
	T Coserp(T t, T a, T b)
	{
		T f = (static_cast<T>(1) - std::cos(t * static_cast<T>(M_PI)) / static_cast<T>(2));

		//
		//               t   -->  [ 0 ..  1]
		//         (PI * t)  -->  [ 0 .. PI]
		//     cos (PI * t)  -->  [-1 ..  1]
		// 1 - cos (PI * t)  -->  [ 0 ..  2]
		//
		// ----> f --> [0..1]
		//

		return Lerp(f, a, b);
	}

	/** a linear function that is y(x) = a.x + b, but with a ease In (the derivative of    y(x) = x * x    as a value of 1 when x = 0.5) */
	template<typename T>
	T EaseInIdentity(T x)
	{
		return static_cast<T>(x) - (static_cast<T>(1.0 - 0.5 * 0.5));
	}

#endif

}; // namespace chaos