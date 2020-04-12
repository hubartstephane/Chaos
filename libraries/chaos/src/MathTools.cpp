#include <chaos/MathTools.h>

namespace chaos
{
	namespace MathTools
	{

		/** structure used to decompose floating point in bit fields */
		union FloatingPointStruct 
		{
			float value;
			struct {
				unsigned int mantissa : 23;  // to get exponent, we decompose value to the form 1.xxx E exponent    
				unsigned int exponent : 8;   // beware : should add 127 to have the correct value
				unsigned int sign : 1;
			};
		} ;

		unsigned int GetMantissa(float f)
		{
			FloatingPointStruct r;
			r.value = f;
			return r.mantissa;
		}

		int GetExponent(float f)
		{
			FloatingPointStruct r;
			r.value = f;
			return r.exponent;
		}

		unsigned int GetSign(float f)
		{
			FloatingPointStruct r;
			r.value = f;
			return r.sign;
		}

		void ResetRandSeed()
		{
			srand((unsigned int)time(nullptr));
		}

		float RandFloat()
		{
			return CastAndDiv<float>(rand(), RAND_MAX);
		}

		float RandFloat(float min_value, float max_value)
		{
			return min_value + RandFloat() * (max_value - min_value);
		}









#if _WIN32

		/** Bit Scan Forward method */
		int16_t bsf(int16_t i)
		{
			assert(i != 0);
			unsigned long result = 0;
			_BitScanForward(&result, (unsigned long)i);
			return (int16_t)result;
		}

		/** Bit Scan Forward method */
		int32_t bsf(int32_t i)
		{
			assert(i != 0);
			unsigned long result = 0;
			_BitScanForward(&result, (unsigned long)i);
			return (int32_t)result;
		}

		/** Bit Scan Forward method */
		uint16_t bsf(uint16_t i)
		{
			assert(i != 0);
			unsigned long result = 0;
			_BitScanForward(&result, (unsigned long)i);
			return (uint16_t)result;
		}

		/** Bit Scan Forward method */
		uint32_t bsf(uint32_t i)
		{
			assert(i != 0);
			unsigned long result = 0;
			_BitScanForward(&result, (unsigned long)i);
			return (uint32_t)result;
		}

		/** Bit Scan Reverse methods */
		int16_t bsr(int16_t i)
		{
			assert(i != 0);
			unsigned long result = 0;
			_BitScanReverse(&result, (unsigned long)i);
			return (int16_t)result;
		}

		/** Bit Scan Reverse methods */
		int32_t bsr(int32_t i)
		{
			assert(i != 0);
			unsigned long result = 0;
			_BitScanReverse(&result, (unsigned long)i);
			return (int32_t)result;
		}

		/** Bit Scan Reverse methods */
		uint16_t bsr(uint16_t i)
		{
			assert(i != 0);
			unsigned long result = 0;
			_BitScanReverse(&result, (unsigned long)i);
			return (uint16_t)result;
		}

		/** Bit Scan Reverse methods */
		uint32_t bsr(uint32_t i)
		{
			assert(i != 0);
			unsigned long result = 0;
			_BitScanReverse(&result, (unsigned long)i);
			return (uint16_t)result;
		}

#endif // _WIN32


#if _WIN64

		/** Bit Scan Forward methods */
		int64_t bsf(int64_t i)
		{
			assert(i != 0);
			unsigned long result = 0;
			_BitScanForward64(&result, (__int64)i);
			return (uint16_t)result;
		}

		/** Bit Scan Forward methods */
		uint64_t bsf(uint64_t i)
		{
			assert(i != 0);
			unsigned long result = 0;
			_BitScanForward64(&result, (unsigned __int64)i);
			return (uint64_t)result;
		}

		/** Bit Scan Reverse methods */
		int64_t bsr(int64_t i)
		{
			assert(i != 0);
			unsigned long result = 0;
			_BitScanReverse64(&result, (__int64)i);
			return (uint16_t)result;
		}

		/** Bit Scan Reverse methods */
		uint64_t bsr(uint64_t i)
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
		int16_t bsf(int16_t i)
		{
			assert(i != 0);
			__asm mov ax, i
			__asm bsf ax, ax
			__asm mov i, ax
			return i;
		}

		/** Bit Scan Forward method */
		int32_t bsf(int32_t i)
		{
			assert(i != 0);
			__asm mov eax, i
			__asm bsf eax, eax
			__asm mov i, eax
			return i;
		}

		/** Bit Scan Forward method */
		uint16_t bsf(uint16_t i)
		{
			assert(i != 0);
			__asm mov ax, i
			__asm bsf ax, ax
			__asm mov i, ax
			return i;
		}

		/** Bit Scan Forward method */
		uint32_t bsf(uint32_t i)
		{
			assert(i != 0);
			__asm mov eax, i
			__asm bsf eax, eax
			__asm mov i, eax
			return i;
		}

		/** Bit Scan Reverse methods */
		int16_t bsr(int16_t i)
		{
			assert(i != 0);
			__asm mov ax, i
			__asm bsr ax, ax
			__asm mov i, ax
			return i;
		}

		/** Bit Scan Reverse methods */
		int32_t bsr(int32_t i)
		{
			assert(i != 0);
			__asm mov eax, i
			__asm bsr eax, eax
			__asm mov i, eax
			return i;
		}

		/** Bit Scan Reverse methods */
		uint16_t bsr(uint16_t i)
		{
			assert(i != 0);
			__asm mov ax, i
			__asm bsr ax, ax
			__asm mov i, ax
			return i;
		}

		/** Bit Scan Reverse methods */
		uint32_t bsr(uint32_t i)
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








	}; // namespace MathTools

}; // namespace chaos
