#include "chaos/ChaosPCH.h"
#include "chaos/ChaosInternals.h"

namespace chaos
{

	int GPUVertexDeclarationEntry::GetComponentCount() const
	{
		int component_count = int(type) & 7;
		return component_count; 
	}

	GLenum GPUVertexDeclarationEntry::GetComponentType() const
	{
		VertexAttributeComponentType component_type = VertexAttributeComponentType((int(type) >> 3) & 7);

		if (component_type == VertexAttributeComponentType::FLOAT)
			return GL_FLOAT;
		else if (component_type == VertexAttributeComponentType::DOUBLE)
			return GL_DOUBLE;
		else if (component_type == VertexAttributeComponentType::HALF)
			return GL_HALF_FLOAT;
		else if (component_type == VertexAttributeComponentType::BYTE)
			return GL_BYTE;
		else if (component_type == VertexAttributeComponentType::INT)
			return GL_INT;
		return GL_INVALID_ENUM;
	}

	int GPUVertexDeclarationEntry::GetEntrySize() const
	{
		VertexAttributeComponentType component_type = VertexAttributeComponentType((int(type) >> 3) & 7);

		int component_size = 0;
		if (component_type == VertexAttributeComponentType::FLOAT)
			component_size = sizeof(float);
		else if (component_type == VertexAttributeComponentType::DOUBLE)
			component_size = sizeof(double);
		else if (component_type == VertexAttributeComponentType::HALF)
			component_size = sizeof(float) / 2;
		else if (component_type == VertexAttributeComponentType::BYTE)
			component_size = sizeof(unsigned char);
		else if (component_type == VertexAttributeComponentType::INT)
			component_size = sizeof(int32_t);

		int component_count = int(type) & 7;

		return component_count * component_size; 
	}

	void GPUVertexDeclaration::Push(VertexAttributeSemantic semantic, int semantic_index, VertexAttributeType type, char const * name)
	{
		int offset = 0;
		if (entries.size() > 0)
		{
			GPUVertexDeclarationEntry const & previous_entry = entries.back();
			offset = previous_entry.offset + previous_entry.GetEntrySize();
		}

		GPUVertexDeclarationEntry entry;
		entry.semantic       = semantic;
		entry.semantic_index = semantic_index;
		entry.type           = type;
		entry.offset         = offset;
		if (name != nullptr)
			entry.name = name;
		entries.push_back(entry);
	}

	GPUVertexDeclarationEntry* GPUVertexDeclaration::GetEntry(char const * name)
	{
		assert(name != nullptr);
		for (auto& entry : entries)
			if (StringTools::Stricmp(entry.name, name) == 0)
				return &entry;
		return nullptr;
	}

	GPUVertexDeclarationEntry const* GPUVertexDeclaration::GetEntry(char const* name) const
	{
		assert(name != nullptr);
		for (auto const& entry : entries)
			if (StringTools::Stricmp(entry.name, name) == 0)
				return &entry;
		return nullptr;
	}

	GPUVertexDeclarationEntry * GPUVertexDeclaration::GetEntry(VertexAttributeSemantic semantic, int semantic_index)
	{
		for (auto & entry : entries)
			if (entry.semantic == semantic && (entry.semantic_index < 0 || entry.semantic_index == semantic_index))
				return &entry;
		return nullptr;
	}

	GPUVertexDeclarationEntry const * GPUVertexDeclaration::GetEntry(VertexAttributeSemantic semantic, int semantic_index) const
	{
		for (auto const & entry : entries)
			if (entry.semantic == semantic && (entry.semantic_index < 0 || entry.semantic_index == semantic_index))
				return &entry;
		return nullptr;
	}

	int GPUVertexDeclaration::GetVertexSize() const
	{
		// return the effective size
		if (effective_size > 0)
			return effective_size;
		// fallback
		int result = 0;
		for (auto const & entry : entries)
			result += entry.GetEntrySize();
		return result;
	}

	void GPUVertexDeclaration::SetEffectiveVertexSize(int in_effective_size)
	{
		effective_size = in_effective_size;
	}

	int GPUVertexDeclaration::GetSemanticCount(VertexAttributeSemantic semantic) const
	{
		int result = -1;
		for (auto const & entry : entries)
			if (entry.semantic == semantic)
				result = std::max(result, entry.semantic_index);
		return result + 1;
	}

	int GPUVertexDeclaration::GetPositionCount() const
	{
		return GetSemanticCount(VertexAttributeSemantic::POSITION);
	}

	int GPUVertexDeclaration::GetColorCount() const
	{
		return GetSemanticCount(VertexAttributeSemantic::COLOR);
	}

	int GPUVertexDeclaration::GetTextureCount() const
	{
		return GetSemanticCount(VertexAttributeSemantic::TEXCOORD);
	}

	int GPUVertexDeclaration::GetBoneCount() const
	{
		return std::min(GetSemanticCount(VertexAttributeSemantic::BONEINDEX), GetSemanticCount(VertexAttributeSemantic::BONEWEIGHT));
	}


}; // namespace chaos
