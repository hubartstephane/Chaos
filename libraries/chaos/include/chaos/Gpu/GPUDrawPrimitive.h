namespace chaos
{
#ifdef CHAOS_FORWARD_DECLARATION

	class GPUDrawPrimitive;

#elif !defined CHAOS_TEMPLATE_IMPLEMENTATION

	/**
	* GPUDrawPrimitive: a draw call
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
		/** beginning in vertex or index buffer */
		int    start = 0;
		/** for indexed rendering, this is an offset applied to each index */
		int    base_vertex_index = 0;
	};

#endif

}; // namespace chaos