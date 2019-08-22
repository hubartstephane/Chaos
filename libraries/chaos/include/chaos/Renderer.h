#pragma once

#include <chaos/StandardHeaders.h>
#include <chaos/ReferencedObject.h>
#include <chaos/DrawPrimitive.h>
#include <chaos/GPUFramebuffer.h>
#include <chaos/TimedAccumulator.h>
#include <chaos/GPUFence.h>
#include <chaos/Tickable.h>

namespace chaos
{


	class GPUFramebufferRenderData
	{
	public:

		/** the concerned framebuffer */
		shared_ptr<GPUFramebuffer> framebuffer;	
		/** whether mipmaps should be generated at the end of rendering */
		bool generate_mipmaps = false;
	};

	class Renderer : public Tickable
	{
	public:

		/** draw a primitive */
		void Draw(DrawPrimitive const & primitive, InstancingInfo const & instancing = InstancingInfo());

		/** called at the start of a new frame */
		virtual void BeginRenderingFrame();
		/** called at the end of a new frame */
		virtual void EndRenderingFrame();


		/** called to start of rendering on a new Framebuffer */
		virtual bool PushFramebufferRenderContext(GPUFramebuffer * framebuffer, bool generate_mipmaps);
		/** called at the start of rendering on a framebuffer */ 
		virtual bool PopFramebufferRenderContext();

		/** get the current frame rate */
		float GetFrameRate() const;
		/** get the rendering timestamp */
		uint64_t GetTimestamp() const;
		/** get  a fence for the end of this frame */
		GPUFence * GetCurrentFrameFence();

	protected:

		/** override */
		virtual bool DoTick(double delta_time) override;

	protected:

		/** a time stamp for rendering */
		uint64_t rendering_timestamp = 0;
		/** the fence for current frame */
		shared_ptr<GPUFence> rendering_fence;
		/** for counting frame per seconds */
		TimedAccumulator<float> framerate_counter;
		/** for counting drawcall per seconds */
		TimedAccumulator<float> drawcall_counter;
		/** for counting drawcall per seconds */
		TimedAccumulator<float> vertices_counter;

		/** the stack of framebuffer */
		std::vector<GPUFramebufferRenderData> framebuffer_stack;

		/** whether a rendering is in progress */
#if _DEBUG
		bool rendering_started = false;
#endif
	};

}; // namespace chaos
