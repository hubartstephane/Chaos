namespace chaos
{
#ifdef CHAOS_FORWARD_DECLARATION

	class GPUVertexAndIndexMappedBuffers;

#elif !defined CHAOS_TEMPLATE_IMPLEMENTATION

	/**
	 * GPUVertexAndIndexMappedBuffers: creating and mapping both an vertex and an index buffers is a common paradigm. this class is here to do all that at once
	 **/

	class GPUVertexAndIndexMappedBuffers
	{
	public:

		/** create all required resources */
		static GPUVertexAndIndexMappedBuffers CreateMappedBuffers(size_t vertex_buffer_size, size_t index_buffer_size);
		/** check for validity */
		bool IsValid() const;
		/** destroy all resources (usefull to ensure everything is unmapped properly) */
		void CleanResources();

	public:

		/** the vertex buffer */
		shared_ptr<GPUBuffer> vertex_buffer;
		/** the index buffer */
		shared_ptr<GPUBuffer> index_buffer;
		/** the mapped vertex buffer */
		char* mapped_vertex_buffer = nullptr;
		/** the mapped index buffer */
		char* mapped_index_buffer = nullptr;
	};

#endif

}; // namespace chaos