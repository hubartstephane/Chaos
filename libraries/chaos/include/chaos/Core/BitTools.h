#ifdef CHAOS_FORWARD_DECLARATION


#else 

namespace chaos
{

	/**
	* BitTools is namespace for methods to handle ASM/BIT functions
	*/

	namespace BitTools
	{

#if _WIN32

		/** Bit Scan Forward method */
		int16_t bsf(int16_t i);
		/** Bit Scan Forward method */
		int32_t bsf(int32_t i);
		/** Bit Scan Forward method */
		uint16_t bsf(uint16_t i);
		/** Bit Scan Forward method */
		uint32_t bsf(uint32_t i);
		/** Bit Scan Reverse methods */
		int16_t bsr(int16_t i);
		/** Bit Scan Reverse methods */
		int32_t bsr(int32_t i);
		/** Bit Scan Reverse methods */
		uint16_t bsr(uint16_t i);
		/** Bit Scan Reverse methods */
		uint32_t bsr(uint32_t i);

#endif // _WIN32

#if _WIN64

		/** Bit Scan Forward methods */
		int64_t bsf(int64_t i);
		/** Bit Scan Forward methods */
		uint64_t bsf(uint64_t i);
		/** Bit Scan Reverse methods */
		int64_t bsr(int64_t i);
		/** Bit Scan Reverse methods */
		uint64_t bsr(uint64_t i);

#endif // _WIN64

	}; // namespace BitTools

}; // namespace chaos

#endif // CHAOS_FORWARD_DECLARATION




