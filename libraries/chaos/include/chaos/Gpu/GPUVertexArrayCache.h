namespace chaos
{
#ifdef CHAOS_FORWARD_DECLARATION

	class GPUVertexArrayCacheEntry;
	class GPUVertexArrayCache;

#elif !defined CHAOS_TEMPLATE_IMPLEMENTATION

	// ==================================================================
	// GPUVertexArrayCacheEntry : an entry in the cache vertex array
	// ==================================================================

	// XXX : OpenGL contexts do not share 'CONTAINER OBJECTS' has VertexArrays or FrameBuffers
	//       That's why we use a Window entry in the cache system

	class CHAOS_API GPUVertexArrayCacheEntry
	{
		friend class GPUVertexArrayCache;

	public:

		/** whether the entry is still valid (whether one of the pointed element has been destroyed) */
		bool IsValid() const;

	public:

		/** the program concerned */
		weak_ptr<GPUProgram const> program;
		/** the vertex buffer concerned */
		weak_ptr<GPUBuffer const> vertex_buffer;
		/** the index buffer concerned */
		weak_ptr<GPUBuffer const> index_buffer;
		/** context window */
		weak_ptr<Window> context_window; // VertexArrays are not shared between different contexts

		/** the index of the program */
		GLuint program_id = 0;
		/** the vertex buffer */
		GLuint vertex_buffer_id = 0;
		/** the index buffer */
		GLuint index_buffer_id = 0;
		/** the offset for the vertex buffer */
		GLintptr vertex_buffer_offset = 0;

		/** the context */
		GLFWwindow* context = nullptr;

		/** the vertex array */
		shared_ptr<GPUVertexArray> vertex_array;
	};

	// =================================================================================================
	// GPUVertexArrayCache : a binding between GPUProgram/GPUVertexArray that support destruction of both side
	// =================================================================================================

	class CHAOS_API GPUVertexArrayCache : public Object
	{

	public:

		/** find vertex array for the program */
		GPUVertexArray const* FindVertexArray(GPURenderer* renderer, GPUProgram const* program, GPUBuffer const* vertex_buffer, GPUBuffer const* index_buffer, GLintptr offset) const;
		/** create or return exisiting vertex array for a given program */
		GPUVertexArray const* FindOrCreateVertexArray(GPURenderer* renderer, GPUProgram const* program, GPUBuffer const* vertex_buffer, GPUBuffer const* index_buffer, GPUVertexDeclaration const* declaration, GLintptr offset = 0);
		/** reset the whole object */
		void Clear();

	protected:

		/** a counter to know whether to purge the elements */
		mutable double last_purge_time = 0;
		/** the cache content */
		mutable std::vector<GPUVertexArrayCacheEntry> entries;
	};

#endif

}; // namespace chaos