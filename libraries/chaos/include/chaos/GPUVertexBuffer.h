#pragma once

#include <chaos/StandardHeaders.h>
#include <chaos/GPUBuffer.h>

namespace chaos
{
	class GPUVertexBuffer : public GPUBuffer
	{
	public:

		using GPUBuffer::GPUBuffer; // forwarding constructors
	};

}; // namespace chaos
