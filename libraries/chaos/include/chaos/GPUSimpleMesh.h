#pragma once

#include <chaos/StandardHeaders.h>
#include <chaos/GPUClasses.h>
#include <chaos/GPUResource.h>

#include <chaos/GPUVertexArrayCache.h>
#include <chaos/GPUVertexDeclaration.h>
#include <chaos/GPUDrawPrimitive.h>
#include <chaos/GPUBuffer.h>

namespace chaos
{

	/**
	* GPUSimpleMesh : how it says
	*/

	class GPUSimpleMesh : public GPUResource
	{
	public:

		/** constructor */
		GPUSimpleMesh() = default;
		/** destructor */
		~GPUSimpleMesh();

		/** render the primitive (base_instance is an offset applyed to gl_InstanceID) */
		void Render(GPURenderer * renderer, GPUProgram const * program, GPUProgramProviderBase const * uniform_provider, GPURenderParams const & render_params);
		/** render the primitive (base_instance is an offset applyed to gl_InstanceID) */
		void Render(GPURenderer * renderer, GPURenderMaterial const * material, GPUProgramProviderBase const * uniform_provider, GPURenderParams const & render_params);

		/** should bind index buffer and vertex buffer, as musch as for the vertex declaration */
		void SetVertexBufferOffset(GLintptr vertex_buffer_offset);
		/** offset the index or vertex position */
		void ShiftPrimitivesIndexAndVertexPosition(int vb_offset, int ib_offset);

	protected:

		/** cleaning the object */
		virtual bool DoRelease() override;
		/** rendering internal method */
		void DoRender(GPURenderer * renderer, GPUProgram const * program, GPURenderParams const & render_params);

	public:

		/** self descriptive */
		GPUVertexDeclaration vertex_declaration;
		/** the primitives to render */
		std::vector<GPUDrawPrimitive> primitives;

		/** self descriptive */
		shared_ptr<GPUBuffer> vertex_buffer;
		/** self descriptive */
		shared_ptr<GPUBuffer> index_buffer;
		/** in the vertex buffer (that may be shared by other simpled mesh), the offset from the begining given to it */
		GLintptr vertex_buffer_offset = 0;

	protected:

		/** the cache for all vertex arrays */
		GPUVertexArrayCache vertex_array_cache;
	};

}; // namespace chaos
