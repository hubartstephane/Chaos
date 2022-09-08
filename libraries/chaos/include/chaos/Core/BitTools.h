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

#endif

	}; // namespace BitTools

}; // namespace chaos






