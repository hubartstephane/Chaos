#pragma once

#include <chaos/StandardHeaders.h>
#include <chaos/ReferencedObject.h>
#include <chaos/VertexArray.h>
#include <chaos/GPUProgram.h>
#include <chaos/GPUResource.h>
#include <chaos/VertexBuffer.h>
#include <chaos/VertexDeclaration.h>
#include <chaos/IndexBuffer.h>


namespace chaos
{
	// ==================================================================
	// VertexArrayCacheEntry : an entry in the cache vertex array
	// ==================================================================

	class VertexArrayCacheEntry : public ReferencedObject
	{
		friend class GPUProgramToVertexArrayCacheCallbacks;
		friend class VertexArrayCache;

	public:

		/** destructor */
		virtual ~VertexArrayCacheEntry();

	public:

		/** the cache containing the entry */
		VertexArrayCache * cache = nullptr;
		/** the id of the program concerned */
		GPUProgram * program = nullptr;
		/** a pointer on the destruction callback associated */
		GPUProgramToVertexArrayCacheCallbacks * program_destruction_callback = nullptr;	

		/** the vertex array */
		boost::intrusive_ptr<VertexArray> vertex_array;
	};

	// =================================================================================================
	// VertexArrayCache : a binding between GPUProgram/VertexArray that support destruction of both side
	// =================================================================================================

	class VertexArrayCache : public ReferencedObject
	{
		friend class GPUProgramToVertexArrayCacheCallbacks;

	public:

		/** find vertex array for the program */
		VertexArray * FindVertexArray(GPUProgram * program);
		/** create or return exisiting vertex array for a given program */
		VertexArray * FindOrCreateVertexArray(GPUProgram * program, VertexBuffer * vertex_buffer, IndexBuffer * index_buffer, VertexDeclaration & declaration);

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

		friend class VertexArrayCache;
		friend class VertexArrayCacheEntry;

	protected:

		/** called whenever the object is being released (returns true whether the callback must be removed from the list) */
		virtual bool OnResourceReleased(GPUResource * object, bool destruction) override;

	protected:

		/** a raw pointer on the program */
		GPUProgram * program = nullptr;
		/** a raw pointer on a cache entry */
		VertexArrayCacheEntry * cache_entry = nullptr;	
	};

}; // namespace chaos
