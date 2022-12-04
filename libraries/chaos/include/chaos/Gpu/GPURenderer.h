namespace chaos
{
#ifdef CHAOS_FORWARD_DECLARATION

	class GPUFramebufferRenderData;
	class GPURenderer;

#elif !defined CHAOS_TEMPLATE_IMPLEMENTATION

	class CHAOS_API GPUFramebufferRenderData
	{
	public:

		/** the concerned framebuffer */
		shared_ptr<GPUFramebuffer> framebuffer;
		/** whether mipmaps should be generated at the end of rendering */
		bool generate_mipmaps = false;
		/** the viewport to restore */
		aabox2 viewport_to_restore;
	};

	class CHAOS_API GPURenderer : public Tickable
	{
	public:

		/** constructor */
		GPURenderer(Window* in_window);

		/** draw a primitive */
		void Draw(GPUDrawPrimitive const& primitive, GPUInstancingInfo const& instancing = {});
		/** render a full screen quad */
		void DrawFullscreenQuad(GPURenderMaterial const* material, GPUProgramProviderInterface const * uniform_provider, GPURenderParams const& render_params);

		/** called at the start of a new frame */
		void BeginRenderingFrame();
		/** called at the end of a new frame */
		void EndRenderingFrame();

		/** called to start of rendering on a new Framebuffer */
		bool PushFramebufferRenderContext(GPUFramebuffer* framebuffer, bool generate_mipmaps);
		/** called at the start of rendering on a framebuffer */
		bool PopFramebufferRenderContext();

		/** get the last delta_time */
		float GetLastDeltaTime() const { return last_delta_time; }
		/** get the average frame rate */
		float GetAverageFrameRate() const;
		/** get the number of average draw calls */
		int GetAverageDrawCalls() const;
		/** get the number of average rendered vertices */
		int GetAverageVertices() const;

		/** get the rendering timestamp */
		uint64_t GetTimestamp() const;
		/** get  a fence for the end of this frame */
		GPUFence* GetCurrentFrameFence();

		/** get the owning window */
		Window* GetWindow() const { return window.get(); }

	protected:

		/** override */
		virtual bool DoTick(float delta_time) override;

	protected:

		/** a time stamp for rendering */
		uint64_t rendering_timestamp = 0;
		/** the fence for current frame */
		shared_ptr<GPUFence> rendering_fence;
		/** the duration of the last tick */
		float last_delta_time = 0.0f;
		/** for counting frame per seconds */
		TimedAccumulator<float> framerate_counter;
		/** for counting drawcall per seconds */
		TimedAccumulator<int> drawcall_counter;
		/** for counting drawcall per seconds */
		TimedAccumulator<int> vertices_counter;

		/** the owning window */
		weak_ptr<Window> window;

		/** the stack of framebuffer */
		std::vector<GPUFramebufferRenderData> framebuffer_stack;

		/** whether a rendering is in progress */
#if _DEBUG
		bool rendering_started = false;
#endif
	};

#endif

}; // namespace chaos