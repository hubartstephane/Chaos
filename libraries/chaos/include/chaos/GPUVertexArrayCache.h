#pragma once

#include <chaos/StandardHeaders.h>
#include <chaos/ReferencedObject.h>
#include <chaos/GPUVertexArray.h>
#include <chaos/GPUProgram.h>
#include <chaos/GPUResource.h>
#include <chaos/GPUVertexBuffer.h>
#include <chaos/GPUVertexDeclaration.h>
#include <chaos/GPUIndexBuffer.h>


namespace chaos
{
	// ==================================================================
	// VertexArrayCacheEntry : an entry in the cache vertex array
	// ==================================================================

	class VertexArrayCacheEntry : public ReferencedObject
	{
		friend class GPUProgramToVertexArrayCacheCallbacks;
		friend class GPUVertexArrayCache;

	public:

		/** destructor */
		virtual ~VertexArrayCacheEntry();

	public:

		/** the cache containing the entry */
		GPUVertexArrayCache * cache = nullptr;
		/** the id of the program concerned */
		GPUProgram const * program = nullptr;
		/** a pointer on the destruction callback associated */
		GPUProgramToVertexArrayCacheCallbacks * program_destruction_callback = nullptr;	

		/** the vertex array */
		boost::intrusive_ptr<GPUVertexArray> vertex_array;
	};

	// =================================================================================================
	// GPUVertexArrayCache : a binding between GPUProgram/GPUVertexArray that support destruction of both side
	// =================================================================================================

	class GPUVertexArrayCache : public ReferencedObject
	{
		friend class GPUProgramToVertexArrayCacheCallbacks;

	public:

		/** find vertex array for the program */
		GPUVertexArray const * FindVertexArray(GPUProgram const * program) const;
		/** create or return exisiting vertex array for a given program */
		GPUVertexArray const * FindOrCreateVertexArray(GPUProgram const * program, GPUVertexBuffer const * vertex_buffer, GPUIndexBuffer const * index_buffer, GPUVertexDeclaration const & declaration, GLintptr offset = 0);
		/** reset the whole object */
		void Clear();

	protected:

		/** remove the entry from the cache */
		void RemoveEntry(VertexArrayCacheEntry * entry);

	protected:

		/** the cache content */
		std::vector<boost::intrusive_ptr<VertexArrayCacheEntry>> entries;
	
	};

	// ================================================================================================================
	// GPUProgramToVertexArrayCacheCallbacks : a callback for GPUProgram to notify the cache that it is being destroyed
	// ================================================================================================================

	class GPUProgramToVertexArrayCacheCallbacks : public GPUResourceCallbacks
	{

		friend class GPUVertexArrayCache;
		friend class VertexArrayCacheEntry;

	protected:

		/** called whenever the object is being released (returns true whether the callback must be removed from the list) */
		virtual bool OnResourceReleased(GPUResource const * object, bool destruction) override;

	protected:

		/** a raw pointer on the program */
		GPUProgram const * program = nullptr;
		/** a raw pointer on a cache entry */
		VertexArrayCacheEntry * cache_entry = nullptr;	
	};

}; // namespace chaos
