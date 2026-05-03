namespace chaos
{
#ifdef CHAOS_FORWARD_DECLARATION

	class GPURenderContext;

#elif !defined CHAOS_TEMPLATE_IMPLEMENTATION

	CHAOS_DEFINE_LOG(GPURenderContextLog, "GPURenderContextLog")

	/**
	 * GPURenderContext: interface for rendering
	 */

	class CHAOS_API GPURenderContext : public Tickable, public GPUDeviceResourceInterface
	{
		friend class Window;
		friend class GPUDevice;

	public:

		/** override */
		virtual void SubReference() override;

		/** render a frame (into the window directly) */
		bool RenderFrame(LightweightFunction<bool()> render_func);
		/** render a frame into a framebuffer */
		bool RenderIntoFramebuffer(GPUFramebuffer* framebuffer, bool generate_mipmaps, LightweightFunction<bool()> render_func);

		/** prepare vertex array */
		bool BindVertexArray(GPUVertexArrayBindingInfo const & binding_info);
		/** end the vertex array usage */
		void UnbindVertexArray();

		/** draw a primitive */
		void Draw(GPUDrawPrimitive const& primitive, GPUInstancingInfo const& instancing = {});
		/** render a full screen quad */
		void DrawFullscreenQuad(GPURenderMaterial const* material, GPUProgramProviderInterface const* uniform_provider, GPURenderParams const& render_params);

		/** get the rendering statistics */
		GPURenderContextStats const & GetStats() const { return stats; }

		/** get the owning window */
		Window* GetWindow() const { return window.get(); }

		/** check whether the context is current */
		bool IsRenderContextCurrent() const;

	protected:

		/** constructor */
		GPURenderContext(GPUDevice * in_gpu_device, Window* in_window);

		/** override */
		virtual bool DoTick(float delta_time) override;

		/** destroy the context */
		void Destroy();

		/** called at the start of a new frame */
		void BeginRenderingFrame();
		/** called at the end of a new frame */
		void EndRenderingFrame();

		/** called whenever a buffer resource is destroyed */
		void OnBufferDestroyed(GLuint in_buffer_id);
		/** called whenever a program resource is destroyed */
		void OnProgramDestroyed(GLuint in_program_id);

	protected:

		/** the owning window */
		weak_ptr<Window> window;
		/** the vertex array cache */
		GPUVertexArrayCache vertex_array_cache;

		/** whether a rendering is in progress */
		bool rendering_started = false;
		/** offscreen rendering count */
		int offscreen_rendering_count = 0;

		/** the rendering statistics */
		GPURenderContextStats stats;
	};

#endif

}; // namespace chaos