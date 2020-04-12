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

	}; // namespace MathTools

}; // namespace chaos
