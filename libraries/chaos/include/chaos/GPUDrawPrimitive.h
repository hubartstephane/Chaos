#pragma once

#include <chaos/StandardHeaders.h>

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

}; // namespace chaos
