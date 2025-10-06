#include "chaos/ChaosPCH.h"
#include "chaos/ChaosInternals.h"

namespace chaos
{
	size_t GPUVertexDeclarationEntry::GetHash() const
	{
		size_t result = 0;
		boost::hash_combine(result, int(semantic));
		boost::hash_combine(result, int(type));
		boost::hash_combine(result, semantic_index);
		boost::hash_combine(result, offset);
		return result;
	}

	size_t GPUVertexDeclarationEntry::GetComponentCount() const
	{
		size_t component_count = size_t(type) & 7;
		return component_count;
	}

	GLenum GPUVertexDeclarationEntry::GetComponentType() const
	{
		VertexAttributeComponentType component_type = VertexAttributeComponentType((size_t(type) >> 3) & 7);

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

	size_t GPUVertexDeclarationEntry::GetEntrySize() const
	{
		VertexAttributeComponentType component_type = VertexAttributeComponentType((size_t(type) >> 3) & 7);

		size_t component_size = 0;
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

		size_t component_count = size_t(type) & 7;
		return component_count * component_size;
	}

	void GPUVertexDeclaration::Push(VertexAttributeSemantic semantic, int semantic_index, VertexAttributeType type, char const * name)
	{
		size.reset();
		hash.reset();

		size_t offset = 0;
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

	size_t GPUVertexDeclaration::GetVertexSize() const
	{
		// return the effective size
		if (effective_size.has_value())
			return effective_size.value();
		// get or compute the size
		if (!size.has_value())
		{
			size_t result = 0;
			for (auto const& entry : entries)
				result += entry.GetEntrySize();
			size = result;
		}
		return size.value();
	}

	void GPUVertexDeclaration::SetEffectiveVertexSize(size_t in_effective_size)
	{
		effective_size = in_effective_size;
	}

	size_t GPUVertexDeclaration::GetSemanticCount(VertexAttributeSemantic semantic) const
	{
		size_t result = 0;
		for (auto const& entry : entries)
			if (entry.semantic == semantic)
				++result;
		return result;
	}

	size_t GPUVertexDeclaration::GetPositionCount() const
	{
		return GetSemanticCount(VertexAttributeSemantic::POSITION);
	}

	size_t GPUVertexDeclaration::GetColorCount() const
	{
		return GetSemanticCount(VertexAttributeSemantic::COLOR);
	}

	size_t GPUVertexDeclaration::GetTextureCount() const
	{
		return GetSemanticCount(VertexAttributeSemantic::TEXCOORD);
	}

	size_t GPUVertexDeclaration::GetBoneCount() const
	{
		return std::min(GetSemanticCount(VertexAttributeSemantic::BONEINDEX), GetSemanticCount(VertexAttributeSemantic::BONEWEIGHT));
	}

	size_t GPUVertexDeclaration::GetHash() const
	{
		if (!hash.has_value())
		{
			std::size_t hash_value = 0;
			for (GPUVertexDeclarationEntry const& entry : entries)
				boost::hash_combine(hash_value, entry.GetHash());
			hash = hash_value;
		}
		return hash.value();
	}

}; // namespace chaos
