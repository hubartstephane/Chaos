namespace chaos
{
	/** MathTools is namespace for methods to handle mathematical functions */
	namespace MathTools
	{
#ifdef CHAOS_FORWARD_DECLARATION

		template<typename T, typename U>
		class RangeMapper;

#elif !defined CHAOS_TEMPLATE_IMPLEMENTATION

		/** returns the mantissa of a float */
		unsigned int GetMantissa(float f);

		/** returns the exponent of a float */
		int GetExponent(float f);

		/** returns the sign of a float */
		unsigned int GetSign(float f);

		/** a functor used to repeatly map a range to another */
		template<typename T, typename U>
		class RangeMapper
		{
		public:

			/** constructor */
			RangeMapper(std::pair<T, T> const& src_range, std::pair<U, U> const& dst_range)
			{
				factor = (dst_range.second - dst_range.first) / (src_range.second - src_range.first);
				origin = dst_range.first - src_range.first * factor;
			}

			/** constructor */
			RangeMapper(T const& src_range_min, T const& src_range_max, U const& dst_range_min, U const& dst_range_max)
			{
				factor = (dst_range_max - dst_range_min) / (src_range_max - src_range_min);
				origin = dst_range_min - src_range_min * factor;
			}

			/** the functor **/
			U operator()(T src) const
			{
				return origin + src * factor;
			}

		protected:

			/** internal */
			U origin;
			/** internal */
			U factor;
		};

		// A signed modulo version
		//
		// XXX : the '%' operator is the reminder of the integer divide ... not really modulo when it comes with negative values
		//
		template<typename T>
		T Modulo(T a, T b)
		{
			assert(b > 0);

			// property :    
			//
			//    a ^ b == (a + k.b) ^ b
			//
			// i want to use % operator, so i need to have a 'a + k.b' positive value
			//
			//    a + k.b >= 0     =>  k = -(a/b) + 1
			if constexpr (std::is_integral_v<T>)
			{
				if (a < 0)
				{
					int k = -(a / b) + 1;
					a = a + (k * b);
				}
				return a % b;
			}
			else if constexpr (std::is_same_v<T, float> || std::is_same_v<T, double>)
			{
				if (a < 0)
				{
					int64_t k = -static_cast<int64_t>(a / b) + static_cast<int64_t>(1);
					a = a + (static_cast<T>(k) * b);
				}
				return std::fmod(a, b);
			}
		}

		/** gets the value to add to src in order to have dst angle (with the lowest possible rotation) */
		template<typename T>
		T GetShortestRotation(T src, T dst)
		{
			T const pi = T(M_PI);
			// set the two parameters into [0 .. 2PI] (the choosen range is not important while this is for difference computation)
			src = Modulo(src, pi + pi);
			dst = Modulo(dst, pi + pi);
			// compute the best path
			T diff = dst - src;
			if (std::abs(diff) <= pi)
			{
				return diff;
			}
			else
			{
				if (src > dst)
					return (pi + pi - src) + dst;
				else
					return  -(pi + pi - dst) - src;
			}
		}


		/** Try to make src be equal to dst. Use shortest path and clamp possible rotation */
		template<typename T>
		T UpdateRotationForTargetAngle(T src, T dst, T max_rotation)
		{
			assert(max_rotation >= 0);

			T diff = GetShortestRotation(src, dst);

			return src + std::clamp(diff, -max_rotation, max_rotation);
		}

		/** remap a range to another */
		template<typename T, typename U>
		U RemapRanges(T const& src_range_min, T const& src_range_max, U const& dst_range_min, U const& dst_range_max, T src)
		{
			return dst_range_min + (src - src_range_min) * (dst_range_max - dst_range_min) / (src_range_max - src_range_min);
		}

		/** create a range remapper functor */
		template<typename T, typename U>
		RangeMapper<T, U> MakeRangeRemapper(T const& src_range_min, T const& src_range_max, U const& dst_range_min, U const& dst_range_max)
		{
			return RangeMapper<T, U>(src_range_min, src_range_max, dst_range_min, dst_range_max);
		}

		/** create a range remapper functor */
		template<typename T, typename U>
		RangeMapper<T, U> MakeRangeRemapper(std::pair<T, T> const& src_range, std::pair<U, U> const& dst_range)
		{
			return RangeMapper<T, U>(src_range, dst_range);
		}

		/** a function to multiply to values and convert */
		template<typename T, typename T1, typename T2>
		T CastAndMul(T1 src1, T2 src2)
		{
			return static_cast<T>(
				static_cast<float>(src1) * static_cast<float>(src2)
				);
		}

		template<typename T, typename T1, typename T2>
		T CastAndDiv(T1 src1, T2 src2)
		{
			return static_cast<T>(
				static_cast<float>(src1) / static_cast<float>(src2)
				);
		}

		/** convert an angle into Radian (for float or double) */
		template<typename T>
		T DegreeToRadian(T degree)
		{
			return degree * static_cast<T>(M_PI / 180.0);
		}

		/** convert an angle into Degree (for float or double) */
		template<typename T>
		T RadianToDegree(T radian)
		{
			return radian * static_cast<T>(180.0 / M_PI);
		}

		/** return the square of a value */
		template<typename T>
		T Square(T value)
		{
			return value * value;
		}

		/** Saturate a value to 1 */
		template<typename T>
		T Saturate(T x)
		{
			return std::min(x, static_cast<T>(1));
		}

		/** compute a vector from polar coordinates */
		template<typename T>
		glm::tvec3<T> PolarCoordToVector(T alpha, T beta)
		{
			T c = std::cos(beta);

			return glm::tvec3<T>(std::cos(alpha) * c, std::sin(beta), std::sin(alpha) * c);
		}

		/** returns the linear interpolation between 2 values */
		template<typename T>
		T Lerp(T t, T a, T b)
		{
			return ((static_cast<T>(1) - t) * a) + (t * b);
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

		/** an Ease-In, Ease-out function */
		template<typename T>
		T Ease(T x)
		{
			T x2 = x * x;
			return static_cast<T>(3) * x2 - static_cast<T>(2) * x2 * x;
		}

		/** a linear function that is y(x) = a.x + b, but with a ease In (the derivative of    y(x) = x * x    as a value of 1 when x = 0.5) */
		template<typename T>
		T EaseInIdentity(T x)
		{
			return static_cast<T>(x) - (static_cast<T>(1.0 - 0.5 * 0.5));
		}

		/** transform range [-1 .. +1] into 3 integer values -1, 0, +1 */
		template<typename T>
		T AnalogicToDiscret(T value)
		{
			if (value == 0)
				return 0;
			return (value > 0) ? (T)+1 : (T)-1;
		}

		/** the structure for polynomial solutions */
		template<typename T>
		struct Polynome2Solution
		{
			/** the number of solutions (0, 1 or 2) */
			size_t solution_count;
			/** the solutions */
			T      solutions[2];
		};

		/** solve the 2 degree polynome ax2 + bx + c */
		template<typename T>
		Polynome2Solution<T> SolvePolynome2(T a, T b, T c)
		{
			Polynome2Solution<T> result;

			T delta = (b * b) - (static_cast<T>(4) * a * c); // b2 - 4ac

			if (delta < 0)
			{
				result.solution_count = 0;
			}
			else if (delta == 0)
			{
				T solution = -b / (a + a);
				result.solutions[0] = result.solutions[1] = solution;
				result.solution_count = 1;
			}
			else
			{
				T sqrtdelta = Sqrt(delta);
				T _2a = a + a;
				result.solutions[0] = (-b - sqrtdelta) / _2a;
				result.solutions[1] = (-b + sqrtdelta) / _2a;
				result.solution_count = 2;
			}

			return result;
		}

		/** target a value */
		template<typename T>
		T TargetValue(T src, T target_value, T increase_value, T decrease_value)
		{
			if (src < target_value)
				src = std::min(src + increase_value, target_value);
			else if (src > target_value)
				src = std::max(src - decrease_value, target_value);
			return src;
		}

		/** a function to reset rand() function */
		void ResetRandSeed();

		/** returns a random float between [0..1] */
		float RandFloat();

		/** returns a random float between X and Y */
		float RandFloat(float min_value, float max_value);

		/** a function to detect if an entry is power of 2 */
		template<typename T>
		bool IsPowerOf2(T src)
		{
			return ((src != 0) && (src & (src - 1)) == 0);
		}

		/** a function to find the smallest power of 2 greater or equal than src */
		template<typename T>
		T GetNearestPowerOf2(T src)
		{
			if (src == 0)
				return 1;
			if (IsPowerOf2(src))
				return src;
			return 1 << (BitTools::bsr(src) + 1);
		}

		template<typename T>
		bool IsInRange(T value, T min_value, T max_value, bool open_range = true)
		{
			if (open_range)
			{
				if (value <= min_value || value >= max_value)
					return false;
			}
			else
			{
				if (value < min_value || value > max_value)
					return false;
			}
			return true;
		}

#endif

	}; // namespace MathTools

}; // namespace chaos