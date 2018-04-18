#include <chaos/SimpleMesh.h>

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
	void DrawPrimitive::ShiftIndexAndVertexPosition(int vb_offset, int ib_offset)
	{
		if (!indexed)
			start += vb_offset;
		else
		{
			start += ib_offset;
			base_vertex_index += vb_offset;
		}
	}

	void DrawPrimitive::Render(InstancingInfo const & instancing) const
	{
		// This function is able to render : 
		//   -normal primitives
		//   -indexed primitives
		//   -instanced primitives
		//
		// We do not use yet :
		//   -indirect primitive
		//   -multi draws possibilities

		if (count <= 0)
			return;

		if (!indexed)
		{
			if (instancing.instance_count <= 1)
			{
				glDrawArrays(primitive_type, start, count);
			}
			else
			{
				if (instancing.base_instance == 0)
					glDrawArraysInstanced(primitive_type, start, count, instancing.instance_count);
				else
					glDrawArraysInstancedBaseInstance(primitive_type, start, count, instancing.instance_count, instancing.base_instance);
			}
		}
		else
		{
			GLvoid * offset = ((int32_t*)nullptr) + start;
			if (instancing.instance_count <= 1)
			{
				if (base_vertex_index == 0)
					glDrawElements(primitive_type, count, GL_UNSIGNED_INT, offset);
				else
					glDrawElementsBaseVertex(primitive_type, count, GL_UNSIGNED_INT, offset, base_vertex_index);
			}
			else
			{
				if (base_vertex_index == 0)
					glDrawElementsInstanced(primitive_type, count, GL_UNSIGNED_INT, offset, instancing.instance_count);
				else
					glDrawElementsInstancedBaseVertexBaseInstance(primitive_type, count, GL_UNSIGNED_INT, offset, instancing.instance_count, base_vertex_index, instancing.base_instance);
			}
		}
	}

}; // namespace chaos
