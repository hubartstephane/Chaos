#pragma once

#include <chaos/StandardHeaders.h>
#include <chaos/ReferencedObject.h>

namespace chaos
{

	class Renderer : public ReferencedObject
	{
	public:

		/** called at the start of a new frame */
		virtual void BeginRenderingFrame();
		/** called at the end of a new frame */
		virtual void EndRenderingFrame();

	};

}; // namespace chaos
