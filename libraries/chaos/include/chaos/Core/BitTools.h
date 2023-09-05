namespace chaos
{

	/**
	* BitTools is namespace for methods to handle ASM/BIT functions
	*/

	namespace BitTools
	{

#if !defined CHAOS_FORWARD_DECLARATION && !defined CHAOS_TEMPLATE_IMPLEMENTATION

#if _WIN32

		/** Bit Scan Forward method */
		CHAOS_API int16_t bsf(int16_t i);
		/** Bit Scan Forward method */
		CHAOS_API int32_t bsf(int32_t i);
		/** Bit Scan Forward method */
		CHAOS_API uint16_t bsf(uint16_t i);
		/** Bit Scan Forward method */
		CHAOS_API uint32_t bsf(uint32_t i);
		/** Bit Scan Reverse methods */
		CHAOS_API int16_t bsr(int16_t i);
		/** Bit Scan Reverse methods */
		CHAOS_API int32_t bsr(int32_t i);
		/** Bit Scan Reverse methods */
		CHAOS_API uint16_t bsr(uint16_t i);
		/** Bit Scan Reverse methods */
		CHAOS_API uint32_t bsr(uint32_t i);

#endif // _WIN32

#if _WIN64

		/** Bit Scan Forward methods */
		CHAOS_API int64_t bsf(int64_t i);
		/** Bit Scan Forward methods */
		CHAOS_API uint64_t bsf(uint64_t i);
		/** Bit Scan Reverse methods */
		CHAOS_API int64_t bsr(int64_t i);
		/** Bit Scan Reverse methods */
		CHAOS_API uint64_t bsr(uint64_t i);

#endif // _WIN64

		template<typename T>
		T SetBit(T src, T bit_index, bool value)
		{
			return (value)?
				src | (1 << bit_index):
				src & ~(1 << bit_index);
		}

		template<bool FORWARD = true, std::integral T, typename FUNC>
		decltype(auto) ForEachBit(T bitfield, FUNC const& func)
		{
			using result_type = decltype(func(0));

			constexpr bool convertible_to_bool = std::is_convertible_v<result_type, bool>;

			while (bitfield != 0)
			{
				T bit;
				if constexpr (FORWARD)
					bit = BitTools::bsf(bitfield);
				else
					bit = BitTools::bsr(bitfield);

				if constexpr (convertible_to_bool)
				{
					if (auto result = func(bit))
						return result;
				}
				else
				{
					func(bit);
				}
				bitfield &= ~(1 << bit);
			}

			if constexpr (convertible_to_bool)
				return result_type();
		}

		template<std::integral T, typename FUNC>
		decltype(auto) ForEachBitForward(T bitfield, FUNC const& func)
		{
			return ForEachBit<true>(bitfield, func);
		}

		template<std::integral T, typename FUNC>
		decltype(auto) ForEachBitReverse(T bitfield, FUNC const& func)
		{
			return ForEachBit<false>(bitfield, func);
		}

#endif

	}; // namespace BitTools

}; // namespace chaos