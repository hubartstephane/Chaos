#pragma once

#include <chaos/StandardHeaders.h>
#include <chaos/GPUClasses.h>

#include <chaos/Tickable.h>
#include <chaos/GPUFramebuffer.h>
#include <chaos/GPUFence.h>
#include <chaos/TimedAccumulator.h>
#include <chaos/GPUSimpleMesh.h>
#include <chaos/GPUInstancingInfo.h>

namespace chaos
{

	class GPUFramebufferRenderData
	{
	public:

		/** the concerned framebuffer */
		shared_ptr<GPUFramebuffer> framebuffer;	
		/** whether mipmaps should be generated at the end of rendering */
		bool generate_mipmaps = false;
		/** the viewport to restore */
		GLint viewport_to_restore[4];
	};

	class GPURenderer : public Tickable
	{
	public:

		/** initialization of the internal resources */
		virtual bool Initialize();

		/** render a full screen quad */
		void DrawFullscreenQuad(GPURenderMaterial const * material, GPUProgramProviderBase const * uniform_provider, GPURenderParams const & render_params);

		/** draw a primitive */
		void Draw(GPUDrawPrimitive const& primitive, GPUInstancingInfo const& instancing = {});

		/** called at the start of a new frame */
		void BeginRenderingFrame();
		/** called at the end of a new frame */
		void EndRenderingFrame();

		/** called to start of rendering on a new Framebuffer */
		bool PushFramebufferRenderContext(GPUFramebuffer * framebuffer, bool generate_mipmaps);
		/** called at the start of rendering on a framebuffer */ 
		bool PopFramebufferRenderContext();

		/** get the current frame rate */
		float GetFrameRate() const;
		/** get the rendering timestamp */
		uint64_t GetTimestamp() const;
		/** get  a fence for the end of this frame */
		GPUFence * GetCurrentFrameFence();

        /** get an index buffer for quad rendering (returns the number of quad that can be renderer with this buffer) */
        GPUBuffer* GetQuadIndexBuffer(size_t* result_quad_count);

	protected:

		/** override */
		virtual bool DoTick(float delta_time) override;

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

		/** the fullscreen quad mesh */
		shared_ptr<GPUSimpleMesh> quad_mesh;

        /** the quad to triangle_pair index rendering */
        shared_ptr<GPUBuffer> quad_index_buffer;


		/** whether a rendering is in progress */
#if _DEBUG
		bool rendering_started = false;
#endif
	};

}; // namespace chaos
