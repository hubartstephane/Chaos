#include <chaos/GPUDrawPrimitive.h>

namespace chaos
{

	//
	// XXX : the vb_offset is not really usefull in mixed BUFFER (one buffer with 2 parts)
	//       
	//       x------------------x-------------------x
	//       | vertex size = 6  |  vertex_size = 18 |
	//       x------------------x-------------------x
	//
	//       because BASE_VERTEX_INDEX is a number of vertices 
	//        => the real offset depends of the size of ONE vertex
	//       
	//      to create a shift, use glVertexArrayVertexBuffer(...offset ...) that helps create a view of buffer
	//
	void GPUDrawPrimitive::ShiftIndexAndVertexPosition(int vb_offset, int ib_offset)
	{
		if (!indexed)
			start += vb_offset;
		else
		{
			start += ib_offset;
			base_vertex_index += vb_offset;
		}
	}

}; // namespace chaos
