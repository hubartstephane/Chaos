#pragma once

#include <chaos/StandardHeaders.h>
#include <chaos/ReferencedObject.h>
#include <chaos/DrawPrimitive.h>

namespace chaos
{

	class Renderer : public ReferencedObject
	{
	public:

		/** draw a primitive */
		void Draw(DrawPrimitive const & primitive, InstancingInfo const & instancing = InstancingInfo());

		/** called at the start of a new frame */
		virtual void BeginRenderingFrame();
		/** called at the end of a new frame */
		virtual void EndRenderingFrame();

	};

}; // namespace chaos
