#include "chaos/ChaosPCH.h"
#include "chaos/ChaosInternals.h"

namespace chaos
{
	namespace BitTools
	{
		/** Bit Scan Forward methods */
		int8_t bsf(int8_t i)
		{
			assert(i != 0);
			return std::countr_zero(static_cast<uint8_t>(i));
		}

		int16_t bsf(int16_t i)
		{
			assert(i != 0);
			return std::countr_zero(static_cast<uint16_t>(i));
		}

		int32_t bsf(int32_t i)
		{
			assert(i != 0);
			return std::countr_zero(static_cast<uint32_t>(i));
		}

		int64_t bsf(int64_t i)
		{
			assert(i != 0);
			return std::countr_zero(static_cast<uint64_t>(i));
		}

		uint8_t bsf(uint8_t i)
		{
			assert(i != 0);
			return std::countr_zero(i);
		}

		uint16_t bsf(uint16_t i)
		{
			assert(i != 0);
			return std::countr_zero(i);
		}

		uint32_t bsf(uint32_t i)
		{
			assert(i != 0);
			return std::countr_zero(i);
		}

		uint64_t bsf(uint64_t i)
		{
			assert(i != 0);
			return std::countr_zero(i);
		}

		/** Bit Scan Reverse methods */
		int8_t bsr(int8_t i)
		{
			assert(i != 0);
			return std::bit_width(static_cast<uint8_t>(i)) - 1;
		}

		int16_t bsr(int16_t i)
		{
			assert(i != 0);
			return std::bit_width(static_cast<uint16_t>(i)) - 1;
		}

		int32_t bsr(int32_t i)
		{
			assert(i != 0);
			return std::bit_width(static_cast<uint32_t>(i)) - 1;
		}

		int64_t bsr(int64_t i)
		{
			assert(i != 0);
			return std::bit_width(static_cast<uint64_t>(i)) - 1;
		}

		uint8_t bsr(uint8_t i)
		{
			assert(i != 0);
			return std::bit_width(i) - 1;
		}

		uint16_t bsr(uint16_t i)
		{
			assert(i != 0);
			return std::bit_width(i) - 1;
		}

		uint32_t bsr(uint32_t i)
		{
			assert(i != 0);
			return std::bit_width(i) - 1;
		}

		uint64_t bsr(uint64_t i)
		{
			assert(i != 0);
			return std::bit_width(i) - 1;
		}

		/** Popcount methods */
		int8_t popcount(int8_t i)
		{
			assert(i != 0);
			return std::popcount(static_cast<uint8_t>(i));
		}

		int16_t popcount(int16_t i)
		{
			assert(i != 0);
			return std::popcount(static_cast<uint16_t>(i));
		}

		int32_t popcount(int32_t i)
		{
			assert(i != 0);
			return std::popcount(static_cast<uint32_t>(i));
		}

		int64_t popcount(int64_t i)
		{
			assert(i != 0);
			return std::popcount(static_cast<uint64_t>(i));
		}

		uint8_t popcount(uint8_t i)
		{
			assert(i != 0);
			return std::popcount(i);
		}

		uint16_t popcount(uint16_t i)
		{
			assert(i != 0);
			return std::popcount(i);
		}

		uint32_t popcount(uint32_t i)
		{
			assert(i != 0);
			return std::popcount(i);
		}

		uint64_t popcount(uint64_t i)
		{
			assert(i != 0);
			return std::popcount(i);
		}

	}; // namespace BitTools

}; // namespace chaos
