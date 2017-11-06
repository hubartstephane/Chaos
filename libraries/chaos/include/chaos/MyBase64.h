#pragma once

#include <chaos/StandardHeaders.h>
#include <chaos/Buffer.h>

namespace chaos
{
	//
	// Implementation is coming from : https://stackoverflow.com/questions/180947/base64-decode-snippet-in-c
	//
	class MyBase64
	{
	public:

		/** encoding method */
		std::string Encode(Buffer<char> const & src);
		/** decoding method */
		Buffer<char> Decode(char const * src);

	protected:

		/** utility function to encode 3 bytes into 4 bytes */
		static void EncodeBuffer(unsigned char const * char_array_3, unsigned char * char_array_4);
    /** utility function to decode 4 bytes into 3 bytes */
    static void DecodeBuffer(unsigned char const * char_array_4, unsigned char * char_array_3);
    /** returns true whether the input is a valid character */
		static bool IsBase64(unsigned char c);

	protected:

		/** utility buffer for encodage */
		static char const * base64_chars;
	};

}; // namespace chaos
