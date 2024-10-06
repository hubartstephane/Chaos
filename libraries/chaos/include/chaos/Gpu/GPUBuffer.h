namespace chaos
{
#ifdef CHAOS_FORWARD_DECLARATION

	class GPUBuffer;

#elif !defined CHAOS_TEMPLATE_IMPLEMENTATION

	/** GPUBuffer : self explaning */
	class CHAOS_API GPUBuffer : public GPUResource
	{
	public:

		/** constructor (create its own resource) */
		GPUBuffer(bool dynamic);
		/** constructor (reference a given resource). Call this function with 0 if you do not want to create resource at all */
		GPUBuffer(GLuint in_id, bool in_ownership);
		/** destructor */
		virtual ~GPUBuffer();

		/** returns the GL name of the resource */
		GLuint GetResourceID() const { return buffer_id; }
		/** returns true whether the resource is valid */
		bool IsValid() const;

		/** create an OpenGL resource */
		bool CreateResource(bool in_dynamic);
		/** Initialize from GL Resource */
		bool SetResource(GLuint in_id, bool in_ownership);

		/** update the data of the buffer */
		bool SetBufferData(char const* in_data, size_t in_size);
		/** get the size of the buffer */
		size_t GetBufferSize() const;

		/** map the buffer */
		char* MapBuffer(size_t start = 0, size_t count = 0, bool read = false, bool write = true);
		/** unmap the buffer */
		void UnMapBuffer();

		/** an indication of how many objects use this buffer (to avoid buffer to be given to a GPUBufferPool whereas it used elsewhere) */
		size_t GetUsageCount() const { return usage_count; }
		/** Increment usage count */
		size_t IncrementUsageCount() { return ++usage_count; }
		/** Decrement usage count */
		size_t DecrementUsageCount() { assert(usage_count > 0); return --usage_count; }

		/** returns whether the buffer has been mapped */
		bool IsMapped() const { return mapped; }

	protected:

		/** cleaning the object */
		virtual void Release() override;

	protected:

		/** the resource id */
		GLuint buffer_id = 0;
		/** the size of the buffer */
		size_t buffer_size = 0;
		/** whether the object has ownership of the GL resource */
		bool ownership = true;
		/** whether the data is dynamic or not (this is a restrict of GL_ STATIC/STREAM/DYNAMIC ... DRAW/COPY/READ => considere only GL_STATIC_DRAW and GL_DYNAMIC_DRAW */
		bool dynamic = false;
		/** whether the buffer is mapped */
		bool mapped = false;
		/** usage count */
		size_t usage_count = 0;
	};
#endif

}; // namespace chaos