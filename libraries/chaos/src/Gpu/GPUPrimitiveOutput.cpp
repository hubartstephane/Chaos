#include "chaos/ChaosPCH.h"
#include "chaos/ChaosInternals.h"

namespace chaos
{
	// returns the OpenGL primitive type corresponding to the primitive
	constexpr GLenum PrimitiveTypeToGLType(PrimitiveType primitive_type)
	{
		if (primitive_type == PrimitiveType::POINT)
			return GL_POINTS;

		if (primitive_type == PrimitiveType::TRIANGLE)
			return GL_TRIANGLES;
		if (primitive_type == PrimitiveType::TRIANGLE_PAIR)
			return GL_TRIANGLES;
		if (primitive_type == PrimitiveType::QUAD)
			return GL_TRIANGLES;

		if (primitive_type == PrimitiveType::TRIANGLE_STRIP)
			return GL_TRIANGLE_STRIP;
		if (primitive_type == PrimitiveType::TRIANGLE_FAN)
			return GL_TRIANGLE_FAN;

		if (primitive_type == PrimitiveType::LINE)
			return GL_LINES;
		if (primitive_type == PrimitiveType::LINE_STRIP)
			return GL_LINE_STRIP;
		if (primitive_type == PrimitiveType::LINE_LOOP)
			return GL_LINE_LOOP;

		return GL_NONE;
	}

	GPUPrimitiveOutputBase::GPUPrimitiveOutputBase(
		GPUMesh* in_mesh,
		GPUVertexDeclaration* in_vertex_declaration,
		GPURenderMaterial* in_render_material,
		size_t in_vertex_requirement_evaluation
	):
		mesh(in_mesh),
		vertex_declaration(in_vertex_declaration),
		render_material(in_render_material),
		vertex_requirement_evaluation(in_vertex_requirement_evaluation)
	{
		assert(in_mesh != nullptr);

		// get the index buffer for quads
		GPUResourceManager* gpu_resource_manager = WindowApplication::GetGPUResourceManagerInstance();
		if (gpu_resource_manager != nullptr)
			quad_index_buffer = gpu_resource_manager->GetQuadIndexBuffer(&max_quad_count);
		assert((quad_index_buffer != nullptr) && (max_quad_count != 0));
	}

	GPUPrimitiveOutputBase::GPUPrimitiveOutputBase(
		GPUMesh* in_mesh,
		GPUVertexDeclaration* in_vertex_declaration,
		ObjectRequest in_render_material_request,
		size_t in_vertex_requirement_evaluation
	):
		GPUPrimitiveOutputBase(in_mesh, in_vertex_declaration, nullptr, in_vertex_requirement_evaluation)
	{
		GPUResourceManager* resource_manager = WindowApplication::GetGPUResourceManagerInstance();
		if (resource_manager != nullptr)
			render_material = resource_manager->FindRenderMaterial(in_render_material_request);
	}

	GPUPrimitiveOutputBase::~GPUPrimitiveOutputBase()
	{
		Flush();
	}

	AtlasBitmapInfo const* GPUPrimitiveOutputBase::FindBitmapInfo(ObjectRequest bitmap_request, ObjectRequest folder_request) const
	{
		// get the application
		WindowApplication const* window_application = Application::GetInstance();
		if (window_application == nullptr)
			return nullptr;
		// get the atlas
		GPUAtlas const * atlas = window_application->GetTextureAtlas();
		if (atlas == nullptr)
			return nullptr;
		// search the folder
		AtlasFolderInfo const* folder_info = atlas->GetFolderInfo(folder_request, true);
		if (folder_info == nullptr)
			return nullptr;
		return folder_info->GetBitmapInfo(bitmap_request);
	}

	GPUPrimitiveBufferCacheEntry * GPUPrimitiveOutputBase::GetInternalCachedBuffer(size_t required_size)
	{
		for (GPUPrimitiveBufferCacheEntry& cache_entry : internal_buffer_pool)
			if (cache_entry.buffer_end - cache_entry.buffer_position >= int(required_size))
				return &cache_entry;
		return nullptr;
	}

	void GPUPrimitiveOutputBase::GiveBufferToInternalCache(GPUBuffer* in_buffer, char* in_buffer_start, char* in_buffer_position, char* in_buffer_end)
	{
		assert(in_buffer != nullptr);
		assert(in_buffer_start != nullptr);
		assert(in_buffer_position != nullptr);
		assert(in_buffer_end != nullptr);

		for (GPUPrimitiveBufferCacheEntry& entry : internal_buffer_pool)
		{
			if (entry.buffer == in_buffer)
			{
				entry.buffer_start = in_buffer_start;
				entry.buffer_position = in_buffer_position;
				entry.buffer_end = in_buffer_end;
				return;
			}
		}
		internal_buffer_pool.push_back({ in_buffer, in_buffer_start, in_buffer_position, in_buffer_end });
	}

	void GPUPrimitiveOutputBase::Flush()
	{
		// finalize the mesh
		FlushMeshElement();
		// unmap all buffers in internal cache
		bool vertex_buffer_in_cache = false;
		for (GPUPrimitiveBufferCacheEntry& cache_entry : internal_buffer_pool)
		{
			if (cache_entry.buffer == vertex_buffer)
				vertex_buffer_in_cache = true;
			cache_entry.buffer->UnMapBuffer();
		}
		// unmap current buffer (that map not be in cache)
		if (!vertex_buffer_in_cache && vertex_buffer != nullptr)
			vertex_buffer->UnMapBuffer();

		vertex_buffer = nullptr;
		internal_buffer_pool.clear();

		buffer_start = buffer_unflushed = buffer_position = buffer_end = nullptr;
	}

	char* GPUPrimitiveOutputBase::AllocateBufferMemory(size_t in_size)
	{
		if (buffer_start == nullptr || (buffer_end - buffer_position) < int(in_size)) // not enough memory in current buffer ?
		{
			// give previous buffer (if any) to internal cache
			if (vertex_buffer != nullptr)
			{
				FlushMeshElement(); // changing buffer, must create a new mesh element
				GiveBufferToInternalCache(vertex_buffer.get(), buffer_start, buffer_position, buffer_end);
				buffer_start = buffer_unflushed = buffer_position = buffer_end = nullptr;
				vertex_buffer = nullptr;
			}
			// the current GPUBuffer is not big enough -> get a new buffer
			if (GPUPrimitiveBufferCacheEntry* cache_entry = GetInternalCachedBuffer(in_size)) // try in buffers that has been started and never fully filled
			{
				vertex_buffer = cache_entry->buffer;
				buffer_start = cache_entry->buffer_start;
				buffer_unflushed = buffer_position = cache_entry->buffer_position;
				buffer_end = cache_entry->buffer_end;
			}
			else // new GPUBuffer is required
			{
				// create the vertex buffer
				GPUDevice * gpu_device = mesh->GetGPUDevice();

				size_t min_vertex_count = std::max(size_t(MIN_VERTEX_ALLOCATION), vertex_requirement_evaluation); // the minimum number of vertex to allocate
				size_t reserve_size     = std::max(in_size, min_vertex_count * vertex_size); // ask for a minimum size

				vertex_buffer = gpu_device->CreateBuffer(reserve_size, GPUBufferFlags::None);
				if (vertex_buffer == nullptr)
					return nullptr;

				// map the buffer
				buffer_start = vertex_buffer->MapBuffer(0, 0, GPUBufferMapFlags::Write);
				if (buffer_start == nullptr)
					return nullptr;
				buffer_unflushed = buffer_position = buffer_start;
				buffer_end = buffer_start + vertex_buffer->GetBufferSize();
			}
		}
		// return the result and displace the position
		assert(buffer_position != nullptr);
		char* result = buffer_position;
		buffer_position += in_size;
		return result;
	}

	void GPUPrimitiveOutputBase::SetRenderMaterial(GPURenderMaterial* in_render_material)
	{
		assert(in_render_material != nullptr);
		if (render_material != in_render_material)
		{
			FlushMeshElement();
			render_material = in_render_material;
		}
	}

	void GPUPrimitiveOutputBase::SetRenderMaterial(ObjectRequest render_material_request)
	{
		GPUResourceManager* resource_manager = WindowApplication::GetGPUResourceManagerInstance();
		if (resource_manager != nullptr)
			SetRenderMaterial(resource_manager->FindRenderMaterial(render_material_request));
	}

	void GPUPrimitiveOutputBase::FlushMeshElement()
	{
		FlushDrawPrimitive();
		if (pending_primitives.size() > 0)
		{
			assert(current_primitive_type != PrimitiveType::NONE);

			GPUMeshElement& element = mesh->AddMeshElement(
				vertex_declaration,
				vertex_buffer.get(),
				(current_primitive_type == PrimitiveType::QUAD)? quad_index_buffer : nullptr);
			element.primitives = std::move(pending_primitives);
			element.render_material = render_material;
			current_primitive_type = PrimitiveType::NONE;
		}
	}

	void GPUPrimitiveOutputBase::FlushDrawPrimitive()
	{
		if (buffer_unflushed != buffer_position) // something to flush
		{
			assert(current_primitive_type != PrimitiveType::NONE);

			GPUDrawPrimitive primitive;
			primitive.primitive_type = PrimitiveTypeToGLType(current_primitive_type);

			// quads are indexed and uses a shared index_buffer. The flush may so produce multiple primitive (if there are not enougth indices in this buffer)
			if (current_primitive_type == PrimitiveType::QUAD)
			{
				// flush
				primitive.indexed = true;
				primitive.start = 0; // the start is relative to the index buffer here : always 0

				while (buffer_unflushed != buffer_position)
				{
					size_t remaining_quad_count = ((buffer_position - buffer_unflushed) / vertex_size) / 4; // remainding quads to flush
					size_t count = std::min(remaining_quad_count, max_quad_count);

					primitive.count = int(6 * count); // 6 indices per quad
					primitive.base_vertex_index = int((buffer_unflushed - buffer_start) / vertex_size);

					pending_primitives.push_back(primitive);
					buffer_unflushed += 4 * count * vertex_size;
				}
			}
			// other primitives than QUAD produces a single draw call
			else
			{
				primitive.count = int((buffer_position - buffer_unflushed) / vertex_size);
				primitive.indexed = false;
				primitive.start = int((buffer_unflushed - buffer_start) / vertex_size); // start relative to the vertex buffer
				primitive.base_vertex_index = 0;
				pending_primitives.push_back(primitive);

				buffer_unflushed = buffer_position;
			}
		}
	}

	char* GPUPrimitiveOutputBase::GeneratePrimitive(size_t requested_size, PrimitiveType primitive_type)
	{
		assert(requested_size != 0);
		assert(primitive_type != PrimitiveType::NONE);

		// 1/ must create a new mesh element if we were indexed and we are no more indexed (or the opposite)
		// 2/ must create a new primitive when starting a FAN/STRIP/LOOP

		// was indexed, becomes not indexed (or the opposite) (or current_primitive_type was none)
		if ((primitive_type == PrimitiveType::QUAD) ^ (current_primitive_type == PrimitiveType::QUAD)) // one or the other but not twice (this works if current_primitive_type == NONE)
		{
			FlushMeshElement();
		}
		// cannot concat the new primitive in the same draw call
		else if (primitive_type == PrimitiveType::TRIANGLE_FAN || primitive_type == PrimitiveType::TRIANGLE_STRIP || primitive_type == PrimitiveType::LINE_STRIP || primitive_type == PrimitiveType::LINE_LOOP)
		{
			FlushDrawPrimitive(); // flush pending primitives
		}
		else if (primitive_type != current_primitive_type)
		{
			FlushDrawPrimitive(); // flush pending primitives
		}

		char * result = AllocateBufferMemory(requested_size); // this may set current_primitive_type to NONE
		if (result != nullptr)
			current_primitive_type = primitive_type;
		return result;
	}

}; // namespace chaos
