namespace chaos
{
#ifdef CHAOS_FORWARD_DECLARATION

	class GPUDrawPrimitive;
	class GPUIndirectDrawArraysCommand;
	class GPUIndirectDrawElementsCommand;

#elif !defined CHAOS_TEMPLATE_IMPLEMENTATION

	/**
	* GPUDrawPrimitive : a draw call
	*/

	class CHAOS_API GPUDrawPrimitive
	{

	public:

		/** self descriptive */
		GLenum primitive_type = GL_TRIANGLES;
		/** is this an indexed rendering */
		bool   indexed = false;
		/** number of vertex or index to use */
		int    count = 0;
		/** beginning in vertex or index buffer (if indexed rendering or not is used) */
		int    start = 0;
		/** for indexed rendering, this is an offset applied to each index */
		int    base_vertex_index = 0;
	};

	// For indirect rendering.
	//
	// https://www.khronos.org/opengl/wiki/Vertex_Rendering

	class CHAOS_API GPUIndirectDrawArraysCommand
	{
	public:

		GLuint  Count;
		GLuint  InstanceCount;
		GLuint  First;
		GLuint  BaseInstance;
	};

	class CHAOS_API GPUIndirectDrawElementsCommand
	{
	public:

		GLuint  Count;
		GLuint  InstanceCount;
		GLuint  FirstIndex;
		GLuint  BaseVertex;
		GLuint  BaseInstance;
	};

#endif

}; // namespace chaos