#ifdef CHAOS_FORWARD_DECLARATION

namespace chaos
{
	class GPUVertexArrayCacheEntry;
	class GPUVertexArrayCache;

}; // namespace chaos

#else 

namespace chaos
{
	// ==================================================================
	// GPUVertexArrayCacheEntry : an entry in the cache vertex array
	// ==================================================================

	class GPUVertexArrayCacheEntry
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
		/** the index of the program */
		GLuint program_id = 0;
		/** the vertex buffer */
		GLuint vertex_buffer_id = 0;
		/** the index buffer */
		GLuint index_buffer_id = 0;

		/** the vertex array */
		shared_ptr<GPUVertexArray> vertex_array;
	};

	// =================================================================================================
	// GPUVertexArrayCache : a binding between GPUProgram/GPUVertexArray that support destruction of both side
	// =================================================================================================

	class GPUVertexArrayCache : public Object
	{

	public:

		/** find vertex array for the program */
		GPUVertexArray const * FindVertexArray(GPUProgram const * program, GPUBuffer const * vertex_buffer, GPUBuffer const * index_buffer) const;
		/** create or return exisiting vertex array for a given program */
		GPUVertexArray const * FindOrCreateVertexArray(GPUProgram const * program, GPUBuffer const * vertex_buffer, GPUBuffer const * index_buffer, GPUVertexDeclaration const * declaration, GLintptr offset = 0);
		/** reset the whole object */
		void Clear();

	protected:

		/** the cache content */
		mutable std::vector<GPUVertexArrayCacheEntry> entries;

	};

}; // namespace chaos

#endif // CHAOS_FORWARD_DECLARATION



