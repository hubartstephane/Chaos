#pragma once

#include <chaos/StandardHeaders.h>
#include <chaos/ReferencedObject.h>
#include <chaos/VertexDeclaration.h>
#include <chaos/GPUProgramData.h>
#include <chaos/GPUProgram.h>
#include <chaos/GPUProgramProvider.h>
#include <chaos/VertexArray.h>
#include <chaos/VertexBuffer.h>
#include <chaos/IndexBuffer.h>
#include <chaos/RenderMaterial.h>
#include <chaos/GPUResource.h>
#include <chaos/DrawPrimitive.h>
#include <chaos/VertexArrayCache.h>

namespace chaos
{

	/**
	* SimpleMesh : how it says
	*/

	class SimpleMesh : public GPUResource
	{
	public:

		/** constructor */
		SimpleMesh() = default;
		/** destructor */
		~SimpleMesh();

		/** render the primitive (base_instance is an offset applyed to gl_InstanceID) */
		void Render(GPUProgram const * program, GPUProgramProviderBase const * uniform_provider = nullptr, InstancingInfo const & instancing = InstancingInfo()) const;
		/** render the primitive (base_instance is an offset applyed to gl_InstanceID) */
		void Render(RenderMaterial const * material, GPUProgramProviderBase const * uniform_provider = nullptr, InstancingInfo const & instancing = InstancingInfo()) const;

		/** should bind index buffer and vertex buffer, as musch as for the vertex declaration */
		void SetVertexBufferOffset(GLintptr vertex_buffer_offset);
		/** offset the index or vertex position */
		void ShiftPrimitivesIndexAndVertexPosition(int vb_offset, int ib_offset);

	protected:

		/** cleaning the object */
		virtual bool DoRelease() override;
		/** find or create a vertex array for a given program */
		VertexArray const * GetOrCreateVertexArrayForProgram(GPUProgram const * program) const;
		/** rendering internal method */
		void DoRender(GPUProgram const * program, GPUProgramProviderBase const * uniform_provider, InstancingInfo const & instancing) const;

	public:

		/** self descriptive */
		VertexDeclaration vertex_declaration;
		/** the primitives to render */
		std::vector<DrawPrimitive> primitives;

		/** self descriptive */
		boost::intrusive_ptr<VertexBuffer> vertex_buffer;
		/** self descriptive */
		boost::intrusive_ptr<IndexBuffer> index_buffer;
		/** in the vertex buffer (that may be shared by other simpled mesh), the offset from the begining given to it */
		GLintptr vertex_buffer_offset = 0;

	protected:

		/** the cache for all vertex arrays */
		mutable VertexArrayCache vertex_array_cache;
	};

}; // namespace chaos
