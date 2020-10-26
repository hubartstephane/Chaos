#ifdef CHAOS_FORWARD_DECLARATION

namespace chaos
{
	class GPUDrawPrimitive;
	class GPUIndirectDrawArraysCommand;
	class GPUIndirectDrawElementsCommand;

}; // namespace chaos

#else 

namespace chaos
{

	/**
	* GPUDrawPrimitive : a draw call
	*/

	class GPUDrawPrimitive
	{
	public:

		/** offset the index or vertex position */
		void ShiftIndexAndVertexPosition(int vb_offset, int ib_offset);

	public:

		/** self descriptive */
		GLenum primitive_type = GL_TRIANGLES;
		/** is this an indexed rendering */
		bool   indexed = false;
		/** number of vertex or index to use */
		int    count = 0;
		/** beginning in vertex or index buffer (if indexed rendering or not is used) */
		int    start = 0;
		/** for indexed rendering, this is an offset applyed to each index */
		int    base_vertex_index = 0;
	};

    // For indirect rendering. 
    //
    // https://www.khronos.org/opengl/wiki/Vertex_Rendering

    class GPUIndirectDrawArraysCommand
    {
    public:

        GLuint  Count;
        GLuint  InstanceCount;
        GLuint  First;
        GLuint  BaseInstance;
    };

    class GPUIndirectDrawElementsCommand
    {
    public:

        GLuint  Count;
        GLuint  InstanceCount;
        GLuint  FirstIndex;
        GLuint  BaseVertex;
        GLuint  BaseInstance;
    };

}; // namespace chaos

#endif // CHAOS_FORWARD_DECLARATION



