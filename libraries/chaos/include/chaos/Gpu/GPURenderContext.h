namespace chaos
{
#ifdef CHAOS_FORWARD_DECLARATION

	class GPURenderContextFrameStats;
	class GPURenderContext;

#elif !defined CHAOS_TEMPLATE_IMPLEMENTATION

	CHAOS_DEFINE_LOG(GPURenderContextLog, "GPURenderContextLog")

	/**
	 * GPURenderContextFrameStats: statistic for a single frame
	 */

	class GPURenderContextFrameStats
	{
	public:

		uint64_t rendering_timestamp = 0;
		int      drawcall_counter    = 0;
		int      vertices_counter    = 0;
		float    frame_start_time    = 0.0f;
		float    frame_end_time      = 0.0f;
	};

	/**
	 * GPURenderContext: interface for rendering
	 */

	class CHAOS_API GPURenderContext : public Tickable, public GPUDeviceResourceInterface
	{
	public:

		/** constructor */
		GPURenderContext(GPUDevice * in_gpu_device, Window* in_window);

		/** render a frame (into the window directly) */
		bool RenderFrame(LightweightFunction<bool()> render_func);
		/** render a frame into a framebuffer */
		bool RenderIntoFramebuffer(GPUFramebuffer* framebuffer, bool generate_mipmaps, LightweightFunction<bool()> render_func);

		/** draw a primitive */
		void Draw(GPUDrawPrimitive const& primitive, GPUInstancingInfo const& instancing = {});
		/** render a full screen quad */
		void DrawFullscreenQuad(GPURenderMaterial const* material, GPUProgramProviderInterface const* uniform_provider, GPURenderParams const& render_params);

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

		/** get the stats among time */
		boost::circular_buffer<GPURenderContextFrameStats> const & GetStats() const { return stats; }

		/** get the owning window */
		Window* GetWindow() const { return window.get(); }

	protected:

		/** override */
		virtual bool DoTick(float delta_time) override;

		/** called at the start of a new frame */
		void BeginRenderingFrame();
		/** called at the end of a new frame */
		void EndRenderingFrame();

	protected:

		/** the owning window */
		weak_ptr<Window> window;
		/** a time stamp for rendering */
		uint64_t rendering_timestamp = 0;
		/** the duration of the last tick */
		float last_delta_time = 0.0f;
		/** for counting frame per seconds */
		TimedAccumulator<float> framerate_counter;
		/** for counting drawcall per seconds */
		TimedAccumulator<int> drawcall_counter;
		/** for counting drawcall per seconds */
		TimedAccumulator<int> vertices_counter;
		/** the stats over time */
		boost::circular_buffer<GPURenderContextFrameStats> stats;
		/** the stat for current frame */
		GPURenderContextFrameStats current_frame_stat;
		/** whether a rendering is in progress */
		bool rendering_started = false;
		/** offscreen rendering count */
		int offscreen_rendering_count = 0;
	};

#endif

}; // namespace chaos