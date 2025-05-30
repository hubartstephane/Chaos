namespace chaos
{
#ifdef CHAOS_FORWARD_DECLARATION

	class GPUFence;

#elif !defined CHAOS_TEMPLATE_IMPLEMENTATION

	// XXX : whenever an OpenGL fence is created it is in the same time pushed inside the command queue
	//       for our usage, some object will ask for a END OF FRAME FENCE
	//       it is an issue because by requiring the new Fence, this fence would be pushed in the command queue before the object push its own commands
	//         -that why there is a constructor that DOESNOT create the OpenGL resource
	//         -there is a CreateGPUFence function to push the fence in the queue

	class CHAOS_API GPUFence : public GPUResource
	{
	public:

		/** constructor (with fence copy) */
		GPUFence(GLsync in_fence);
		/** constructor (with fence creation => pushed in command queue) */
		GPUFence();
		/** destructor */
		virtual ~GPUFence();

		/** create GPU object and push it in command queue */
		bool CreateGPUFence();
		/** wait until the fence is signaled */
		bool WaitForCompletion(float timeout);
		/** returns true whether the resource is valid */
		bool IsValid() const { return (fence != nullptr) && glIsSync(fence); }

		/** returns openGL resource itself */
		GLsync GetGLFence() const { return fence; }

		/** override */
		virtual void Release() override;

	protected:

		/** the fence object */
		GLsync fence = nullptr;
		/** cached result for completion */
		bool completed = false;
	};

#endif

}; // namespace chaos