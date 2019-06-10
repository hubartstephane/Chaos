#pragma once

#include <chaos/StandardHeaders.h>
#include <chaos/ReferencedObject.h>
#include <chaos/GPUVertexDeclaration.h>
#include <chaos/GPUProgramData.h>
#include <chaos/GPUProgram.h>
#include <chaos/GPUProgramProvider.h>
#include <chaos/GPUVertexArray.h>
#include <chaos/GPUBuffer.h>
#include <chaos/GPURenderMaterial.h>
#include <chaos/GPUResource.h>
#include <chaos/DrawPrimitive.h>
#include <chaos/GPUVertexArrayCache.h>
#include <chaos/Renderer.h>

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
		void Render(Renderer * renderer, GPUProgram const * program, GPUProgramProviderBase const * uniform_provider, RenderParams const & render_params) const;
		/** render the primitive (base_instance is an offset applyed to gl_InstanceID) */
		void Render(Renderer * renderer, GPURenderMaterial const * material, GPUProgramProviderBase const * uniform_provider, RenderParams const & render_params) const;

		/** should bind index buffer and vertex buffer, as musch as for the vertex declaration */
		void SetVertexBufferOffset(GLintptr vertex_buffer_offset);
		/** offset the index or vertex position */
		void ShiftPrimitivesIndexAndVertexPosition(int vb_offset, int ib_offset);

	protected:

		/** cleaning the object */
		virtual bool DoRelease() override;
		/** rendering internal method */
		void DoRender(Renderer * renderer, GPUProgram const * program, RenderParams const & render_params) const;

	public:

		/** self descriptive */
		GPUVertexDeclaration vertex_declaration;
		/** the primitives to render */
		std::vector<DrawPrimitive> primitives;

		/** self descriptive */
		shared_ptr<GPUBuffer> vertex_buffer;
		/** self descriptive */
		shared_ptr<GPUBuffer> index_buffer;
		/** in the vertex buffer (that may be shared by other simpled mesh), the offset from the begining given to it */
		GLintptr vertex_buffer_offset = 0;

	protected:

		/** the cache for all vertex arrays */
		mutable GPUVertexArrayCache vertex_array_cache;
	};

}; // namespace chaos
