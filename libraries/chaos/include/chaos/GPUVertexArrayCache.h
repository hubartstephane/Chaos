#pragma once

#include <chaos/StandardHeaders.h>
#include <chaos/ReferencedObject.h>
#include <chaos/GPUVertexArray.h>
#include <chaos/GPUProgram.h>
#include <chaos/GPUResource.h>
#include <chaos/GPUBuffer.h>
#include <chaos/GPUVertexDeclaration.h>


namespace chaos
{
	// ==================================================================
	// VertexArrayCacheEntry : an entry in the cache vertex array
	// ==================================================================

	class VertexArrayCacheEntry
	{
		friend class GPUVertexArrayCache;

	public:

		/** whether the entry is still valid (whether one of the pointed element has been destroyed) */
		bool IsValid() const;

	public:

		/** the program concerned */
		weak_ptr<GPUProgram const> program;
		/** the vertex buffer concerned */
		weak_ptr<GPUBuffer const> vertex_buffer;
		/** the index buffer concerned */
		weak_ptr<GPUBuffer const> index_buffer;
		/** whether the initial vertex buffer was valid */
		bool has_vertex_buffer = false;
		/** whether the initial index buffer was valid */
		bool has_index_buffer = false;
		/** the vertex array */
		shared_ptr<GPUVertexArray> vertex_array;
	};

	// =================================================================================================
	// GPUVertexArrayCache : a binding between GPUProgram/GPUVertexArray that support destruction of both side
	// =================================================================================================

	class GPUVertexArrayCache : public ReferencedObject
	{

	public:

		/** find vertex array for the program */
		GPUVertexArray const * FindVertexArray(GPUProgram const * program) const;
		/** create or return exisiting vertex array for a given program */
		GPUVertexArray const * FindOrCreateVertexArray(GPUProgram const * program, GPUBuffer const * vertex_buffer, GPUBuffer const * index_buffer, GPUVertexDeclaration const & declaration, GLintptr offset = 0);
		/** reset the whole object */
		void Clear();

	protected:

		/** the cache content */
		mutable std::vector<VertexArrayCacheEntry> entries;

	};

}; // namespace chaos
