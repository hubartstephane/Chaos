#include <chaos/VertexDeclaration.h>

namespace chaos
{

int VertexDeclarationEntry::GetComponentCount() const
{
  int component_count = (type & 7);
  return component_count; 
}

GLenum VertexDeclarationEntry::GetComponentType() const
{
  int component_type = (type >> 3) & 7;
  if (component_type == TYPE_FLOAT)
    return GL_FLOAT;
  else if (component_type == TYPE_DOUBLE)
    return GL_DOUBLE;
  else if (component_type == TYPE_HALF)
    return GL_HALF_FLOAT;
  else if (component_type == TYPE_BYTE)
    return GL_BYTE;
  else if (component_type == TYPE_INT)
    return GL_INT;
  return GL_INVALID_ENUM;
}

int VertexDeclarationEntry::GetEntrySize() const
{
  int component_type = (type >> 3) & 7;

  int component_size = 0;
  if (component_type == TYPE_FLOAT)
    component_size = sizeof(float);
  else if (component_type == TYPE_DOUBLE)
    component_size = sizeof(double);
  else if (component_type == TYPE_HALF)
    component_size = sizeof(float) / 2;
  else if (component_type == TYPE_BYTE)
    component_size = sizeof(unsigned char);
  else if (component_type == TYPE_INT)
    component_size = sizeof(int32_t);

  int component_count = (type & 7);

  return component_count * component_size; 
}

void VertexDeclaration::Push(int semantic, int semantic_index, int type)
{
  int offset = 0;
  if (entries.size() > 0)
  {
    VertexDeclarationEntry const & previous_entry = entries[entries.size() - 1];
    offset = previous_entry.offset + previous_entry.GetEntrySize();
  }

  VertexDeclarationEntry entry;
  entry.semantic       = semantic;
  entry.semantic_index = semantic_index;
  entry.type           = type;
  entry.offset         = offset;
  entries.push_back(entry);
}

VertexDeclarationEntry * VertexDeclaration::GetEntry(int semantic, int semantic_index)
{
  for (auto & entry : entries)
    if (entry.semantic == semantic && (entry.semantic_index < 0 || entry.semantic_index == semantic_index))
      return &entry;
  return nullptr;
}

VertexDeclarationEntry const * VertexDeclaration::GetEntry(int semantic, int semantic_index) const
{
  for (auto const & entry : entries)
    if (entry.semantic == semantic && (entry.semantic_index < 0 || entry.semantic_index == semantic_index))
      return &entry;
  return nullptr;
}

int VertexDeclaration::GetVertexSize() const
{
  int result = 0;
  for (auto const & entry : entries)
	  result += entry.GetEntrySize();
  return result;
}

int VertexDeclaration::GetSemanticCount(int semantic) const
{
  int result = -1;
  for (auto const & entry : entries)
    if (entry.semantic == semantic)
      result = max(result, entry.semantic_index);
  return result + 1;
}

int VertexDeclaration::GetPositionCount() const
{
  return GetSemanticCount(SEMANTIC_POSITION);
}

int VertexDeclaration::GetColorCount() const
{
  return GetSemanticCount(SEMANTIC_COLOR);
}

int VertexDeclaration::GetTextureCount() const
{
  return GetSemanticCount(SEMANTIC_TEXCOORD);
}

int VertexDeclaration::GetBoneCount() const
{
  return min(GetSemanticCount(SEMANTIC_BONEINDEX), GetSemanticCount(SEMANTIC_BONEWEIGHT));
}


}; // namespace chaos
