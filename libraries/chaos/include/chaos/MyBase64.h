#pragma once

#include <chaos/StandardHeaders.h>
#include <chaos/Buffer.h>

namespace chaos
{
	class MyBase64
	{
	public:

		/** encoding method */
		Buffer<char> Encode(Buffer<char> const & src);
		/** decoding method */
		Buffer<char> Decode(Buffer<char> const & src);
	};

}; // namespace chaos
