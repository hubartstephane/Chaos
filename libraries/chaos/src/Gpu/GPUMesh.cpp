#include "chaos/ChaosPCH.h"
#include "chaos/ChaosInternals.h"

namespace chaos
{
	GPUMeshElement::GPUMeshElement(GPUMeshElement const& src):
		vertex_declaration(src.vertex_declaration),
		render_material(src.render_material),
		vertex_buffer(src.vertex_buffer),
		index_buffer(src.index_buffer),
		primitives(src.primitives),
		vertex_buffer_offset(src.vertex_buffer_offset)
	{
		if (vertex_buffer != nullptr)
			vertex_buffer->IncrementUsageCount();
		if (index_buffer != nullptr)
			index_buffer->IncrementUsageCount();
	}

	GPUMeshElement::~GPUMeshElement()
	{
		if (vertex_buffer != nullptr)
			vertex_buffer->DecrementUsageCount();
		if (index_buffer != nullptr)
			index_buffer->DecrementUsageCount();
	}

	void GPUMesh::SetVertexArrayCache(GPUVertexArrayCache* in_vertex_array_cache)
	{
		vertex_array_cache = in_vertex_array_cache;
	}

	GPUMeshElement & GPUMesh::AddMeshElement(GPUVertexDeclaration* vertex_declaration, GPUBuffer * vertex_buffer, GPUBuffer * index_buffer)
	{
		if (vertex_buffer != nullptr)
			vertex_buffer->IncrementUsageCount();
		if (index_buffer != nullptr)
			index_buffer->IncrementUsageCount();

		GPUMeshElement & result = elements.emplace_back();
		result.vertex_declaration = vertex_declaration;
		result.vertex_buffer      = vertex_buffer;
		result.index_buffer       = index_buffer;
		return result;
	}

	void GPUMesh::Clear(GPUBufferPool* buffer_pool)
	{
		if (buffer_pool != nullptr) // give buffers to pool if we want that
		{
			for (GPUMeshElement& element : elements)
			{
				if (GPUBuffer * vb = element.vertex_buffer.get())
				{
					assert(!vb->IsMapped());
					if (vb->DecrementUsageCount() == 0)
						buffer_pool->GiveBuffer(vb, nullptr);
					element.vertex_buffer = nullptr; // prevent GPUMeshElement destructor do decrement usage one more time (call this only after given to avoid destruction)
				}
				if (GPUBuffer * ib = element.index_buffer.get())
				{
					assert(!ib->IsMapped());
					if (ib->DecrementUsageCount() == 0)
						buffer_pool->GiveBuffer(ib, nullptr);
					element.index_buffer = nullptr;  // prevent GPUMeshElement destructor do decrement usage one more time (call this only after given to avoid destruction)
				}
			}
		}
		elements.clear();
		bounding_box = {};
	}

	int GPUMesh::DoDisplay(GPURenderContext* render_context, GPUProgramProviderInterface const * uniform_provider, GPURenderParams const& render_params)
	{
		// create a vertex array cache if necessary
		if (vertex_array_cache == nullptr)
			vertex_array_cache = new GPUVertexArrayCache;
		// display the elements
		std::optional<GPURenderMaterial const*> previous_element_material;
		std::optional<GPURenderMaterial const*> previous_effective_material;
		std::optional<GPUProgram const*> previous_program;

		int result = 0;
		for (GPUMeshElement & element : elements)
		{
			// early skip
			if (element.primitives.size() == 0)
				continue;

			// gets the effective_material
			GPURenderMaterial const* effective_material = nullptr;

			GPURenderMaterial const* element_material = element.render_material.get();
			if (previous_element_material.has_value() && element_material == *previous_element_material)
				effective_material = *previous_effective_material;
			else
				effective_material = render_params.GetMaterial(this, element_material);
			if (effective_material == nullptr)
				continue;

			// the the program
			GPUProgram const* program = nullptr;

			if (previous_effective_material.has_value() && effective_material == *previous_effective_material)
				program = *previous_program;
			else
				program = effective_material->UseMaterial(uniform_provider, render_params); // can be costly due to uniform binding
			if (program == nullptr)
				continue;

			// cache data
			previous_element_material = element_material;
			previous_effective_material = effective_material;
			previous_program = program;

			// gets the vertex array
			GPUVertexArray const* vertex_array = vertex_array_cache->FindOrCreateVertexArray(render_context, program, element.vertex_buffer.get(), element.index_buffer.get(), element.vertex_declaration.get(), element.vertex_buffer_offset);
			if (vertex_array == nullptr)
				continue;

			GLuint vertex_array_id = vertex_array->GetResourceID();
			glBindVertexArray(vertex_array_id);

			// draw all primitives
			for (GPUDrawPrimitive const& primitive : element.primitives)
			{
				if (primitive.count <= 0)
					continue;
				render_context->Draw(primitive, render_params.instancing);
				++result;
			}
		}

		// restore an 'empty' state
		glUseProgram(0);
		glBindVertexArray(0);
		return result;
	}

	int GPUMesh::DisplayWithMaterial(GPURenderMaterial const* material, GPURenderContext* render_context, GPUProgramProviderInterface const * uniform_provider, GPURenderParams const& render_params)
	{
		DisableReferenceCount<GPUConstantMaterialProvider> material_provider(material);  // while on stack, use DisableReferenceCount<...>
		GPURenderParams other_render_params = render_params;
		other_render_params.material_provider = &material_provider;
		return Display(render_context, uniform_provider, other_render_params);
	}

	int GPUMesh::DisplayWithProgram(GPUProgram const* program, GPURenderContext* render_context, GPUProgramProviderInterface const * uniform_provider, GPURenderParams const& render_params)
	{
		return DisplayWithMaterial(program->GetDefaultMaterial(), render_context, uniform_provider, render_params);
	}

	void GPUMesh::SetBoundingBox(std::optional<box3> const& in_bounding_box)
	{
		bounding_box = in_bounding_box;
	}
	/** get the bounding box of the mesh */
	std::optional<box3> const& GPUMesh::GetBoundingBox() const
	{
		return bounding_box;
	}

}; // namespace chaos