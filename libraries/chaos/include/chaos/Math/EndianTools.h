#ifdef CHAOS_FORWARD_DECLARATION



#else

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

		/** whether the host is Little Endian */
		bool IsHostLittleEndian();
		/** whether the host is Big Endian */
		bool IsHostBigEndian();

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
		/** conversion method */
		uint16_t EndianSwap(uint16_t src);
		/** conversion method */
		int16_t EndianSwap(int16_t src);
		/** conversion method */
		uint32_t EndianSwap(uint32_t src);
		/** conversion method */
		int32_t EndianSwap(int32_t src);
		/** conversion method */
		uint64_t EndianSwap(uint64_t src);
		/** conversion method */
		int64_t EndianSwap(int64_t src);
		/** conversion method */
		float EndianSwap(float src);
		/** conversion method */
		double EndianSwap(double src);

	}; // namespace EndianTools

}; // namespace chaos

#endif // CHAOS_FORWARD_DECLARATION
