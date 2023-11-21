namespace chaos
{
	//
	// Little Endian :
	//
	//    lsb        msb
	//   --+----------+-->
	//
	// Big Endian :
	//
	//    msb        lsb
	//   --+----------+-->
	//
	// 0x86   : Little Endian
	// TCP/IP : Big Endian
	// MIDI File : BigEndian
	//

	namespace EndianTools
	{
#if !defined CHAOS_FORWARD_DECLARATION && !defined CHAOS_TEMPLATE_IMPLEMENTATION

		/** conversion method */
		CHAOS_API uint16_t EndianSwap(uint16_t src);
		/** conversion method */
		CHAOS_API int16_t EndianSwap(int16_t src);
		/** conversion method */
		CHAOS_API uint32_t EndianSwap(uint32_t src);
		/** conversion method */
		CHAOS_API int32_t EndianSwap(int32_t src);
		/** conversion method */
		CHAOS_API uint64_t EndianSwap(uint64_t src);
		/** conversion method */
		int64_t EndianSwap(int64_t src);
		/** conversion method */
		CHAOS_API float EndianSwap(float src);
		/** conversion method */
		CHAOS_API double EndianSwap(double src);

		/** whether the host is Little Endian */
		CHAOS_API bool IsHostLittleEndian();
		/** whether the host is Big Endian */
		CHAOS_API bool IsHostBigEndian();

		/** convert endianness */
		template<typename T>
		T HostToLittleEndian(T src)
		{
			if (IsHostBigEndian())
				return EndianSwap(src);
			return src;
		}

		/** convert endianness */
		template<typename T>
		T LittleEndianToHost(T src)
		{
			return HostToLittleEndian(src);
		}

		/** convert endianness */
		template<typename T>
		T HostToBigEndian(T src)
		{
			if (IsHostLittleEndian())
				return EndianSwap(src);
			return src;
		}
		/** convert endianness */
		template<typename T>
		T BigEndianToHost(T src)
		{
			return HostToBigEndian(src);
		}

#endif

	}; // namespace EndianTools

}; // namespace chaos