#pragma once

#include <chaos/StandardHeaders.h>
#include <chaos/ReferencedObject.h>
#include <chaos/DrawPrimitive.h>
#include <chaos/TimedAccumulator.h>
#include <chaos/Tickable.h>

namespace chaos
{

	class Renderer : public Tickable
	{
	public:

		/** draw a primitive */
		void Draw(DrawPrimitive const & primitive, InstancingInfo const & instancing = InstancingInfo());

		/** called at the start of a new frame */
		virtual void BeginRenderingFrame();
		/** called at the end of a new frame */
		virtual void EndRenderingFrame();

		/** get the current frame rate */
		float GetFrameRate() const;
		/** get the rendering timestamp */
		uint64_t GetTimestamp() const;

	protected:

		/** override */
		virtual bool DoTick(double delta_time) override;

	protected:

		/** a time stamp for rendering */
		uint64_t rendering_timestamp = 0;
		/** for counting frame per seconds */
		TimedAccumulator<float> framerate_counter;
		/** for counting drawcall per seconds */
		TimedAccumulator<float> drawcall_counter;
		/** for counting drawcall per seconds */
		TimedAccumulator<float> vertices_counter;
	};

}; // namespace chaos
