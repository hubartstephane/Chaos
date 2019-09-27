#pragma once

#include <chaos/StandardHeaders.h>

namespace chaos
{

	/**
	* MathTools is namespace-class for methods to handle mathematical functions
	*/

	class MathTools
	{
	public:

		/** structure used to decompose floating point in bit fields */
		typedef union tagFloatingPointStruct {

			float value;
			struct {
				unsigned int mantissa  : 23;  // to get exponent, we decompose value to the form 1.xxx E exponent    
				unsigned int exponent  : 8;   // beware : should add 127 to have the correct value
				unsigned int sign      : 1;    
			};

		} FloatingPointStruct;

		/** returns the mantissa of a float */
		static unsigned int GetMantissa(float f)
		{
			FloatingPointStruct r;
			r.value = f;
			return r.mantissa;
		}

		/** returns the exponent of a float */
		static int GetExponent(float f)
		{
			FloatingPointStruct r;
			r.value = f;
			return r.exponent;  
		}

		/** returns the sign of a float */
		static unsigned int GetSign(float f)
		{
			FloatingPointStruct r;
			r.value = f;
			return r.sign;
		}


		/** a functor used to repeatly map a range to another */
		template<typename T, typename U>
		class RangeMapper
		{
		public:

			/** constructor */
			RangeMapper(std::pair<T, T> const & src_range, std::pair<U, U> const & dst_range)
			{
				factor = (dst_range.second - dst_range.first) / (src_range.second - src_range.first);
				origin = dst_range.first - src_range.first * factor;
			}

			/** constructor */
			RangeMapper(T const & src_range_min, T const & src_range_max, U const & dst_range_min, U const & dst_range_max)
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

		/** remap a range to another */
		template<typename T, typename U>
		static U RemapRanges(T const & src_range_min, T const & src_range_max, U const & dst_range_min, U const & dst_range_max, T src)
		{
			return dst_range_min + (src - src_range_min) * (dst_range_max - dst_range_min) / (src_range_max - src_range_min);
		}

		/** create a range remapper functor */
		template<typename T, typename U>
		static RangeMapper<T, U> MakeRangeRemapper(T const & src_range_min, T const & src_range_max, U const & dst_range_min, U const & dst_range_max)
		{
			return RangeMapper<T, U>(src_range_min, src_range_max, dst_range_min, dst_range_max);
		}

		/** create a range remapper functor */
		template<typename T, typename U>
		static RangeMapper<T, U> MakeRangeRemapper(std::pair<T, T> const & src_range, std::pair<U, U> const & dst_range)
		{
			return RangeMapper<T, U>(src_range, dst_range);
		}

		/** a function to multiply to values and convert */
		template<typename T, typename T1, typename T2>
		static T CastAndMul(T1 src1, T2 src2)
		{
			return static_cast<T>(
				static_cast<float>(src1) * static_cast<float>(src2)
				);
		}

		template<typename T, typename T1, typename T2>
		static T CastAndDiv(T1 src1, T2 src2)
		{
			return static_cast<T>(
				static_cast<float>(src1) / static_cast<float>(src2)
				);
		}

		/** convert an angle into Radian (for float or double) */
		template<typename T>
		static T DegreeToRadian(T degree)
		{
			return degree * static_cast<T>(M_PI / 180.0);
		}

		/** convert an angle into Degree (for float or double) */
		template<typename T>
		static T RadianToDegree(T radian)
		{
			return radian * static_cast<T>(180.0 / M_PI);
		}

		/** return the square of a value */
		template<typename T>
		static T Square(T value)
		{
			return value * value;
		}

		/** returns the minimum of 2 values */
		template<typename T> 
		static T Minimum(T x,T y)
		{
			return (x < y)? x : y;
		}

		/** returns the maximum of 2 values */
		template<typename T> 
		static T Maximum(T x,T y)
		{
			return (x > y)? x : y;
		}

		/** clamp a value between 2 bounds */
		template<typename T>
		static T Clamp(T value, T minvalue = 0, T maxvalue = static_cast<T>(1))
		{
			return Maximum(Minimum(value, maxvalue), minvalue);
		}

		/** Saturate a value to 1 */
		template<typename T>
		static T Saturate(T x)
		{
			return Minimum(x, static_cast<T>(1));
		}

		/** compute a vector from polar coordinates */
		template<typename T> 
		static glm::tvec3<T> PolarCoordToVector(T alpha, T beta)
		{
			T c = Cos(beta);

			return glm::tvec3<T>(Cos(alpha) * c, Sin(beta), Sin(alpha) * c);
		}

		/** mathematic function wrapper for meta programming for Cos */
		static float Cos(float f){ return cosf(f); }
		/** mathematic function wrapper for meta programming for Cos */
		static double Cos(double f){ return cos(f); }
		/** mathematic function wrapper for meta programming for Sin */
		static float Sin(float f){ return sinf(f); }
		/** mathematic function wrapper for meta programming for Sin */
		static double Sin(double f){ return sin(f); }
		/** mathematic function wrapper for meta programming for SquareRoot */
		static float Sqrt(float f){ return sqrtf(f); }
		/** mathematic function wrapper for meta programming for SquareRoot */
		static double Sqrt(double f){ return sqrt(f); }
		/** mathematic function wrapper for meta programming for fmod */
		static float Fmod(float numer, float denom){ return fmodf(numer, denom); }
		/** mathematic function wrapper for meta programming for fmod */
		static double Fmod(double  numer, double  denom){ return fmod(numer, denom); }
		/** mathematic function wrapper for meta programming for ceil */
		static float Ceil(float f){ return ceilf(f); }
		/** mathematic function wrapper for meta programming for ceil */
		static double Ceil(double f){ return ceil(f); }
		/** mathematic function wrapper for meta programming for floor */
		static float Floor(float f){ return floorf(f); }
		/** mathematic function wrapper for meta programming for floor */
		static double Floor(double f){ return floor(f); }

		/** returns the linear interpolation between 2 values */
		template<typename T> 
		static T Lerp(T t, T a, T b)
		{
			return ((static_cast<T>(1) - t) * a) + (t * b);
		}

		/** returns the cos interpolation between 2 values */
		template<typename T> 
		static T Coserp(T t, T a, T b)
		{
			T f = (static_cast<T>(1) - Cos(t * static_cast<T>(M_PI)) / static_cast<T>(2));

			//
			//               t   -->  [ 0 ..  1]  
			//         (PI * t)  -->  [ 0 .. PI]
			//     cos (PI * t)  -->  [-1 ..  1]
			// 1 - cos (PI * t)  -->  [ 0 ..  2]
			//   
			// ----> f --> [0..1] 
			//

			return Lerp(f, a , b);
		}

		/** an Ease-In, Ease-out function */
		template<typename T>  
		static T Ease(T x)
		{
			T x2 = x * x;
			return static_cast<T>(3) * x2 - static_cast<T>(2) * x2 * x;
		}

		/** a linear function that is y(x) = a.x + b, but with a ease In (the derivative of    y(x) = x * x    as a value of 1 when x = 0.5) */
		template<typename T>  
		static T EaseInIdentity(T x)
		{
			return static_cast<T>(x) - (static_cast<T>(1.0 - 0.5 * 0.5));
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
		static Polynome2Solution<T> SolvePolynome2(T a, T b, T c)
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
				result.solutions[0] = (- b - sqrtdelta) / _2a;
				result.solutions[1] = (- b + sqrtdelta) / _2a;
				result.solution_count = 2;
			}

			return result;
		}

		/** a function to reset rand() function */
		static void ResetRandSeed()
		{
			srand((unsigned int)time(nullptr));
		}
		/** returns a random float between [0..1] */
		static float RandFloat()
		{
			return CastAndDiv<float>(rand(), RAND_MAX);
		}

		/** returns a random float between X and Y */
		static float RandFloat(float min_value, float max_value)
		{
			return min_value + RandFloat() * (max_value - min_value);
		}

		/** a function to detect if an entry is power of 2 */
		template<typename T>
		static bool IsPowerOf2(T src)
		{
			return ((src != 0) && (src & (src - 1)) == 0);
		}

		/** a function to find the smallest power of 2 greater or equal than src */
		template<typename T>
		static T GetNearestPowerOf2(T src)
		{
			if (src == 0)
				return 1;
			if (IsPowerOf2(src))
				return src;
			return 1 << (bsr(src) + 1);
		}


#if _WIN32

		/** Bit Scan Forward method */
		static int16_t bsf(int16_t i)
		{
			assert(i != 0);
			unsigned long result = 0;
			_BitScanForward(&result, (unsigned long)i);
			return (int16_t)result;
		}

		/** Bit Scan Forward method */
		static int32_t bsf(int32_t i)
		{
			assert(i != 0);
			unsigned long result = 0;
			_BitScanForward(&result, (unsigned long)i);
			return (int32_t)result;
		}

		/** Bit Scan Forward method */
		static uint16_t bsf(uint16_t i)
		{
			assert(i != 0);
			unsigned long result = 0;
			_BitScanForward(&result, (unsigned long)i);
			return (uint16_t)result;
		}

		/** Bit Scan Forward method */
		static uint32_t bsf(uint32_t i)
		{
			assert(i != 0);
			unsigned long result = 0;
			_BitScanForward(&result, (unsigned long)i);
			return (uint32_t)result;
		}

		/** Bit Scan Reverse methods */
		static int16_t bsr(int16_t i)
		{
			assert(i != 0);
			unsigned long result = 0;
			_BitScanReverse(&result, (unsigned long)i);
			return (int16_t)result;
		}

		/** Bit Scan Reverse methods */
		static int32_t bsr(int32_t i)
		{
			assert(i != 0);
			unsigned long result = 0;
			_BitScanReverse(&result, (unsigned long)i);
			return (int32_t)result;
		}

		/** Bit Scan Reverse methods */
		static uint16_t bsr(uint16_t i)
		{
			assert(i != 0);
			unsigned long result = 0;
			_BitScanReverse(&result, (unsigned long)i);
			return (uint16_t)result;
		}

		/** Bit Scan Reverse methods */
		static uint32_t bsr(uint32_t i)
		{
			assert(i != 0);
			unsigned long result = 0;
			_BitScanReverse(&result, (unsigned long)i);
			return (uint16_t)result;  
		}

#endif // _WIN32


#if _WIN64

		/** Bit Scan Forward methods */
		static int64_t bsf(int64_t i)
		{
			assert(i != 0);
			unsigned long result = 0;
			_BitScanForward64(&result, (__int64)i);
			return (uint16_t)result;
		}

		/** Bit Scan Forward methods */
		static uint64_t bsf(uint64_t i)
		{
			assert(i != 0);
			unsigned long result = 0;
			_BitScanForward64(&result, (unsigned __int64)i);
			return (uint64_t)result;
		}

		/** Bit Scan Reverse methods */
		static int64_t bsr(int64_t i)
		{
			assert(i != 0);
			unsigned long result = 0;
			_BitScanReverse64(&result, (__int64)i);
			return (uint16_t)result;
		}

		/** Bit Scan Reverse methods */
		static uint64_t bsr(uint64_t i)
		{
			assert(i != 0);
			unsigned long result = 0;
			_BitScanReverse64(&result, (unsigned __int64)i);
			return (uint64_t)result;
		}

#endif // _WIN64

#if _LINUX

		/*

		/** Bit Scan Forward method */
		static int16_t bsf(int16_t i)
		{
			assert(i != 0);
			__asm mov ax, i
			__asm bsf ax, ax
			__asm mov i, ax
			return i;
		}

		/** Bit Scan Forward method */
		static int32_t bsf(int32_t i)
		{
			assert(i != 0);
			__asm mov eax, i
			__asm bsf eax, eax
			__asm mov i, eax
			return i;
		}

		/** Bit Scan Forward method */
		static uint16_t bsf(uint16_t i)
		{
			assert(i != 0);
			__asm mov ax, i
			__asm bsf ax, ax
			__asm mov i, ax
			return i;
		}

		/** Bit Scan Forward method */
		static uint32_t bsf(uint32_t i)
		{
			assert(i != 0);
			__asm mov eax, i
			__asm bsf eax, eax
			__asm mov i, eax
			return i;
		}

		/** Bit Scan Reverse methods */
		static int16_t bsr(int16_t i)
		{
			assert(i != 0);
			__asm mov ax, i
			__asm bsr ax, ax
			__asm mov i, ax
			return i;
		}

		/** Bit Scan Reverse methods */
		static int32_t bsr(int32_t i)
		{
			assert(i != 0);
			__asm mov eax, i
			__asm bsr eax, eax
			__asm mov i, eax
			return i;
		}

		/** Bit Scan Reverse methods */
		static uint16_t bsr(uint16_t i)
		{
			assert(i != 0);
			__asm mov ax, i
			__asm bsr ax, ax
			__asm mov i, ax
			return i;
		}

		/** Bit Scan Reverse methods */
		static uint32_t bsr(uint32_t i)
		{
			assert(i != 0);
			__asm mov eax, i
			__asm bsr eax, eax
			__asm mov i, eax
			return i;
			*/
		}



		asm(

			"bsr %1,%0"

			: "=r" (result)
			: "r"  (i)

		);
		*/

#endif // _LINUX


	};

}; // namespace chaos


