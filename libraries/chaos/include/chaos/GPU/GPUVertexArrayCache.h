namespace chaos
{
#ifdef CHAOS_FORWARD_DECLARATION

	class GPUVertexArrayBindingInfo;
	class GPUVertexArrayCacheEntry;
	class GPUVertexArrayCache;

#elif !defined CHAOS_TEMPLATE_IMPLEMENTATION

	// ==================================================================
	// GPUVertexArrayBindingInfo: this describes how vertex array must be build or searched
	// ==================================================================

	class CHAOS_API GPUVertexArrayBindingInfo
	{
	public:

		/** the program to use */
		GPUProgram const* program = nullptr;
		/** the vertex buffer to use */
		GPUBuffer const* vertex_buffer = nullptr;
		/** the index buffer to use */
		GPUBuffer const* index_buffer = nullptr;
		/** the vertex declaration to use */
		GPUVertexDeclaration const* vertex_declaration = nullptr;
		/** the offset in the vertex buffer */
		GLintptr vertex_buffer_offset = 0;
	};

	// ==================================================================
	// GPUVertexArrayCacheEntry: an entry in the cache vertex array
	// ==================================================================

	class CHAOS_API GPUVertexArrayCacheEntry
	{
		friend class GPUVertexArrayCache;

	public:

		/** mark the entry to be destroyed (when the request occurs, for buffer destruction for example, the context may not be the proper one. do not delete immediatly */
		void MarkedForDestruction();
		/** check whether the entry is marked for destruction */
		bool IsMarkedForDestruction() const;

		/** whether the entry is still valid (whether one of the pointed element has been destroyed) */
		bool IsValid() const;
		/** returns true whether the vertex_array is using the buffer */
		bool IsUsingBuffer(GLuint in_buffer_id) const;
		/** returns true whether the vertex_array is using the program */
		bool IsUsingProgram(GLuint in_program_id) const;

	public:

		/** the vertex array */
		shared_ptr<GPUVertexArray> vertex_array;

		/** the index of the program */
		GLuint program_id = 0;
		/** the vertex buffer */
		GLuint vertex_buffer_id = 0;
		/** the index buffer */
		GLuint index_buffer_id = 0;
		/** the offset for the vertex buffer */
		GLintptr vertex_buffer_offset = 0;
		/** the hash for a given declaration */
		size_t vertex_declaration_hash = 0;

		/** whether this entry should be removed (and the underlying vertex array should be destroyed) */
		bool marked_for_destruction = false;
	};

	// =================================================================================================
	// GPUVertexArrayCache : a binding between GPUProgram/GPUVertexArray that support destruction of both side
	// =================================================================================================

	class CHAOS_API GPUVertexArrayCache : public Tickable, public GPURenderContextResourceInterface
	{
		friend class GPURenderContext;

	public:

		/** destructor */
		virtual ~GPUVertexArrayCache();

		/** create or return exisiting vertex array for a given program */
		GPUVertexArray const* FindOrCreateVertexArray(GPUVertexArrayBindingInfo const& binding_info);

	protected:

		/** constructor */
		GPUVertexArrayCache(GPURenderContext * in_gpu_render_context);

		/** find vertex array for the program */
		GPUVertexArray const* FindVertexArray(GPUVertexArrayBindingInfo const& binding_info) const;
		/** create vertex array for the program */
		GPUVertexArray const* CreateVertexArray(GPUVertexArrayBindingInfo const& binding_info);
		/** purge the cache */
		void PurgeCache();
		/** destroy the whole content */
		void Destroy();
		/** free vertex array resource */
		void ReleaseVertexArray(GPUVertexArray * vertex_array);
		/** called whenever a buffer resource is destroyed */
		void OnBufferDestroyed(GLuint in_buffer_id);
		/** called whenever a program resource is destroyed */
		void OnProgramDestroyed(GLuint in_program_id);

		/** override */
		virtual bool DoTick(float delta_time) override;

	protected:

		/** max delay between successive purges */
		static constexpr float delay_between_purge = 10.0f;

		/** the cache content */
		std::vector<GPUVertexArrayCacheEntry> entries;
		/** time remaining until next purge */
		float purge_timer = 0.0f;
	};

#endif

}; // namespace chaos