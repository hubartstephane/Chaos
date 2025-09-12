namespace chaos
{
	/**
	* BitTools is namespace for methods to handle ASM/BIT functions
	*/

	namespace BitTools
	{

#if !defined CHAOS_FORWARD_DECLARATION && !defined CHAOS_TEMPLATE_IMPLEMENTATION

		/** Bit Scan Forward method */
		CHAOS_API int8_t bsf(int8_t i);
		/** Bit Scan Forward method */
		CHAOS_API int16_t bsf(int16_t i);
		/** Bit Scan Forward method */
		CHAOS_API int32_t bsf(int32_t i);
		/** Bit Scan Forward method */
		CHAOS_API int64_t bsf(int64_t i);

		/** Bit Scan Forward method */
		CHAOS_API uint8_t bsf(uint8_t i);
		/** Bit Scan Forward method */
		CHAOS_API uint16_t bsf(uint16_t i);
		/** Bit Scan Forward method */
		CHAOS_API uint32_t bsf(uint32_t i);
		/** Bit Scan Forward method */
		CHAOS_API uint64_t bsf(uint64_t i);



		/** Bit Scan Reverse method */
		CHAOS_API int8_t bsr(int8_t i);
		/** Bit Scan Reverse method */
		CHAOS_API int16_t bsr(int16_t i);
		/** Bit Scan Reverse method */
		CHAOS_API int32_t bsr(int32_t i);
		/** Bit Scan Reverse method */
		CHAOS_API int64_t bsr(int64_t i);

		/** Bit Scan Reverse method */
		CHAOS_API uint8_t bsr(uint8_t i);
		/** Bit Scan Reverse method */
		CHAOS_API uint16_t bsr(uint16_t i);
		/** Bit Scan Reverse method */
		CHAOS_API uint32_t bsr(uint32_t i);
		/** Bit Scan Reverse method */
		CHAOS_API uint64_t bsr(uint64_t i);



		/** PopCount method */
		CHAOS_API int8_t popcount(int8_t i);
		/** PopCount method */
		CHAOS_API int16_t popcount(int16_t i);
		/** PopCount method */
		CHAOS_API int32_t popcount(int32_t i);
		/** PopCount method */
		CHAOS_API int64_t popcount(int64_t i);

		/** PopCount method */
		CHAOS_API uint8_t popcount(uint8_t i);
		/** PopCount method */
		CHAOS_API uint16_t popcount(uint16_t i);
		/** PopCount method */
		CHAOS_API uint32_t popcount(uint32_t i);
		/** PopCount method */
		CHAOS_API uint64_t popcount(uint64_t i);



		/** set or unset some bit in an integer */
		template<typename T>
		T SetBit(T src, T bit_index, bool value)
		{
			return (value)?
				src |  (T(1) << bit_index):
				src & ~(T(1) << bit_index);
		}

		/** interate over each bit to 1 in an integer */
		template<bool FORWARD, std::integral T, typename FUNC >
		decltype(auto) ForEachBit(T bitfield, FUNC const& func)
		{
			using L = meta::LambdaInfo<FUNC, T>;

			while (bitfield != 0)
			{
				T bit;
				if constexpr (FORWARD)
					bit = BitTools::bsf(bitfield);
				else
					bit = BitTools::bsr(bitfield);

				if constexpr (L::convertible_to_bool)
				{
					if (decltype(auto) result = func(bit))
						return result;
				}
				else
				{
					func(bit);
				}
				bitfield &= ~(T(1) << bit);
			}

			if constexpr (L::convertible_to_bool)
				return typename L::result_type {};
		}

		/** interate over each bit to 1 in an integer (forward direction) */
		template<std::integral T, typename FUNC>
		decltype(auto) ForEachBitForward(T bitfield, FUNC const& func)
		{
			return ForEachBit<true>(bitfield, func);
		}

		/** interate over each bit to 1 in an integer (reverse direction) */
		template<std::integral T, typename FUNC>
		decltype(auto) ForEachBitReverse(T bitfield, FUNC const& func)
		{
			return ForEachBit<false>(bitfield, func);
		}

#endif

	}; // namespace BitTools

}; // namespace chaos