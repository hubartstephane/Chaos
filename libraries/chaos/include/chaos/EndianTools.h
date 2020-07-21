#pragma once

#include <chaos/StandardHeaders.h>

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
		bool IsHostLittleEndian()
		{
			uint16_t const tmp = 0x00FF;

			unsigned char lsb = ((unsigned char const *)&tmp)[0];

			return (lsb == 0xFF);	
		}

		/** whether the host is Big Endian */
		bool IsHostBigEndian(){ return !IsHostLittleEndian();}

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
		uint16_t EndianSwap(uint16_t src)
		{
			uint16_t result = 0;
			((unsigned char *)&result)[0] = ((unsigned char const *)&src)[1];
			((unsigned char *)&result)[1] = ((unsigned char const *)&src)[0];	
			return result;
		}
		/** conversion method */
		int16_t EndianSwap(int16_t src)
		{
			int16_t result = 0;
			((unsigned char *)&result)[0] = ((unsigned char const *)&src)[1];
			((unsigned char *)&result)[1] = ((unsigned char const *)&src)[0];	
			return result;
		}
		/** conversion method */
		uint32_t EndianSwap(uint32_t src)
		{
			uint32_t result = 0;
			((unsigned char *)&result)[0] = ((unsigned char const *)&src)[3];
			((unsigned char *)&result)[1] = ((unsigned char const *)&src)[2];
			((unsigned char *)&result)[2] = ((unsigned char const *)&src)[1];
			((unsigned char *)&result)[3] = ((unsigned char const *)&src)[0];	
			return result;
		}
		/** conversion method */
		int32_t EndianSwap(int32_t src)
		{
			int32_t result = 0;
			((unsigned char *)&result)[0] = ((unsigned char const *)&src)[3];
			((unsigned char *)&result)[1] = ((unsigned char const *)&src)[2];
			((unsigned char *)&result)[2] = ((unsigned char const *)&src)[1];
			((unsigned char *)&result)[3] = ((unsigned char const *)&src)[0];	
			return result;
		}
		/** conversion method */
		uint64_t EndianSwap(uint64_t src)
		{
			uint64_t result = 0;
			((unsigned char *)&result)[0] = ((unsigned char const *)&src)[7];
			((unsigned char *)&result)[1] = ((unsigned char const *)&src)[6];
			((unsigned char *)&result)[2] = ((unsigned char const *)&src)[5];
			((unsigned char *)&result)[3] = ((unsigned char const *)&src)[4];	
			((unsigned char *)&result)[4] = ((unsigned char const *)&src)[3];
			((unsigned char *)&result)[5] = ((unsigned char const *)&src)[2];
			((unsigned char *)&result)[6] = ((unsigned char const *)&src)[1];
			((unsigned char *)&result)[7] = ((unsigned char const *)&src)[0];
			return result;
		}
		/** conversion method */
		int64_t EndianSwap(int64_t src)
		{
			int64_t result = 0;
			((unsigned char *)&result)[0] = ((unsigned char const *)&src)[7];
			((unsigned char *)&result)[1] = ((unsigned char const *)&src)[6];
			((unsigned char *)&result)[2] = ((unsigned char const *)&src)[5];
			((unsigned char *)&result)[3] = ((unsigned char const *)&src)[4];	
			((unsigned char *)&result)[4] = ((unsigned char const *)&src)[3];
			((unsigned char *)&result)[5] = ((unsigned char const *)&src)[2];
			((unsigned char *)&result)[6] = ((unsigned char const *)&src)[1];
			((unsigned char *)&result)[7] = ((unsigned char const *)&src)[0];
			return result;
		}
		/** conversion method */
		float EndianSwap(float src)
		{
			float result = 0;
			((unsigned char *)&result)[0] = ((unsigned char const *)&src)[3];
			((unsigned char *)&result)[1] = ((unsigned char const *)&src)[2];
			((unsigned char *)&result)[2] = ((unsigned char const *)&src)[1];
			((unsigned char *)&result)[3] = ((unsigned char const *)&src)[0];	
			return result;
		}
		/** conversion method */
		double EndianSwap(double src)
		{
			double result = 0;
			((unsigned char *)&result)[0] = ((unsigned char const *)&src)[7];
			((unsigned char *)&result)[1] = ((unsigned char const *)&src)[6];
			((unsigned char *)&result)[2] = ((unsigned char const *)&src)[5];
			((unsigned char *)&result)[3] = ((unsigned char const *)&src)[4];	
			((unsigned char *)&result)[4] = ((unsigned char const *)&src)[3];
			((unsigned char *)&result)[5] = ((unsigned char const *)&src)[2];
			((unsigned char *)&result)[6] = ((unsigned char const *)&src)[1];
			((unsigned char *)&result)[7] = ((unsigned char const *)&src)[0];
			return result;
		}
	}; // namespace EndianTools

}; // namespace chaos

