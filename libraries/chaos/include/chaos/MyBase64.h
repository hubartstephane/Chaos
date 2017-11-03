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
		Buffer<char> Encode(Buffer<char> const & src);
		/** decoding method */
		Buffer<char> Decode(Buffer<char> const & src);

	protected:

		/** utility function to encode 3 bytes into 4 bytes */
		static void EncodeBuffer(unsigned char const * char_array_3, unsigned char * char_array_4);

	protected:

		/** utility buffer for encodage */
		static char const * base64_chars;
	};

}; // namespace chaos
