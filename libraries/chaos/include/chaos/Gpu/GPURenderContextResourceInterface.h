namespace chaos
{
#ifdef CHAOS_FORWARD_DECLARATION

	class GPURenderContextResourceInterface;

#elif !defined CHAOS_TEMPLATE_IMPLEMENTATION

	/**
	 * GPURenderContextResourceInterface: for all objects owned by a GPURenderContext
	 */

	class CHAOS_API GPURenderContextResourceInterface
	{
	public:

		/** constructor */
		GPURenderContextResourceInterface(GPURenderContext* in_gpu_render_context);

		/** getter on the render context  */
		GPURenderContext* GetGPURenderContext() const { return gpu_render_context.get(); }
		/** getter on the device */
		GPUDevice * GetGPUDevice() const;

		/** check whether the context is current */
		bool IsRenderContextCurrent() const;

	protected:

		/** the render context owning this object */
		weak_ptr<GPURenderContext> gpu_render_context;
	};

#endif

}; // namespace chaos