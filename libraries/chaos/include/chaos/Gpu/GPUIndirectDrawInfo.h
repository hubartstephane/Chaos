namespace chaos
{
#ifdef CHAOS_FORWARD_DECLARATION

	class GPUIndirectDrawArraysCommand;
	class GPUIndirectDrawElementsCommand;

#elif !defined CHAOS_TEMPLATE_IMPLEMENTATION

	// For indirect rendering.
	//
	// https://www.khronos.org/opengl/wiki/Vertex_Rendering

	/**
	* GPUIndirectDrawArraysInfo: structure to give the GPU to draw vertices
	*/

	class CHAOS_API GPUIndirectDrawArraysInfo
	{
	public:

		/** number of vertex to use */
		uint32_t   count = 0;
		/** number of instances for this drawcall */
		uint32_t   instance_count = 0;
		/** beginning in vertex buffer */
		uint32_t   start = 0;
		/** starting the ID of the instance */
		uint32_t   base_instance = 0;
	};

	/**
	* GPUIndirectDrawArraysInfo: structure to give the GPU for indexed drawing
	*/

	class CHAOS_API GPUIndirectDrawElementsInfo
	{
	public:

		/** number of index to use */
		uint32_t   count = 0;
		/** number of instances for this drawcall */
		uint32_t   instance_count = 0;
		/** beginning in index buffer */
		uint32_t   start = 0;
		/** this is an offset applied to each vertex index */
		uint32_t   base_vertex_index = 0;
		/** starting the ID of the instance */
		uint32_t   base_instance = 0;
	};

#endif

}; // namespace chaos