namespace chaos
{
#ifdef CHAOS_FORWARD_DECLARATION

	class GPUDevice;

#elif !defined CHAOS_TEMPLATE_IMPLEMENTATION

	/**
	 * GPUDevice: an object used to send commands and create resources from the GPU hardware
	 */

	class CHAOS_API GPUDevice : public Tickable, public ConfigurationUserInterface
	{
		friend class GPUBuffer;
		friend class GPUTexture;
		friend class GPUBufferPool;
		friend class GPUTexturePool;
		friend class GPURenderContext;

	public:

		/** constructor */
		GPUDevice();
		/** destructor */
		virtual ~GPUDevice();

		/** initialize the device */
		virtual bool Initialize(JSONReadConfiguration config);
		/** finalize the device */
		virtual void Finalize();

		/** create a render context for a given window */
		GPURenderContext * CreateRenderContext(Window* in_window);
		/** create a texture */
		GPUTexture * CreateTexture(TextureDescription const& in_texture_description = {});
		/** create a buffer */
		GPUBuffer * CreateBuffer(size_t in_buffer_size, GPUBufferFlags in_flags);

	protected:

		/** override */
		virtual bool DoTick(float delta_time) override;

		/** internal method for initialization */
		virtual bool OnInitialize(JSONReadConfiguration config);

		/** called whenever a texture resource is destroyed */
		void OnTextureDestroyed(GLuint in_texture_id);
		/** called whenever a buffer resource is destroyed */
		void OnBufferDestroyed(GLuint in_buffer_id);
		/** called whenever a program resource is destroyed */
		void OnProgramDestroyed(GLuint in_program_id);
		/** called whenever a render context is about to be destroyed */
		void OnRenderContextDestroyed(GPURenderContext * in_render_context);

		/** called whenever a buffer is not used anymore (but there is still a reference from device) */
		void OnBufferUnused(GPUBuffer * in_buffer);
		/** called whenever a texture is not used anymore (but there is still a reference from device) */
		void OnTextureUnused(GPUTexture * in_texture);

	protected:

		/** the pool for buffers */
		GPUBufferPool buffer_pool;
		/** the pool for textures */
		GPUTexturePool texture_pool;
		/** the render contexts created by this */
		std::vector<shared_ptr<GPURenderContext>> render_contexts;
	};

#endif

}; // namespace chaos