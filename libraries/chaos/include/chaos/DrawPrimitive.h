#pragma once

#include <chaos/StandardHeaders.h>

namespace chaos
{

	/**
	* DrawPrimitive : a draw call
	*/

	class DrawPrimitive
	{
	public:

		/** render the primitive (base_instance is an offset applyed to gl_InstanceID) */
		void Render(int instance_count = 0, int base_instance = 0) const;

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
