#pragma once

#include <chaos/StandardHeaders.h>
#include <chaos/GPUClasses.h>

#include <chaos/ReferencedObject.h>

namespace chaos
{
	/** the possible semantics */
	enum class VertexAttributeSemantic : int
	{
		SEMANTIC_NONE = -1,
		SEMANTIC_POSITION = 0,
		SEMANTIC_COLOR = 1,
		SEMANTIC_NORMAL = 2,
		SEMANTIC_BINORMAL = 3,
		SEMANTIC_TANGENT = 4,
		SEMANTIC_TEXCOORD = 5,
		SEMANTIC_BONEINDEX = 6,
		SEMANTIC_BONEWEIGHT = 7,
		SEMANTIC_USERDATA = 8
	};

	/** the possible component types */
	enum class VertexAttributeComponentType : int
	{	
		TYPE_FLOAT = 1,
		TYPE_DOUBLE = 2,
		TYPE_HALF = 3,
		TYPE_BYTE = 4,
		TYPE_INT = 5
	};

	/** the possible vector types */
	enum class VertexAttributeType : int
	{		
		TYPE_FLOAT1 = ((int)VertexAttributeComponentType::TYPE_FLOAT << 3) | 1,
		TYPE_FLOAT2 = ((int)VertexAttributeComponentType::TYPE_FLOAT << 3) | 2,
		TYPE_FLOAT3 = ((int)VertexAttributeComponentType::TYPE_FLOAT << 3) | 3,
		TYPE_FLOAT4 = ((int)VertexAttributeComponentType::TYPE_FLOAT << 3) | 4,

		TYPE_DOUBLE1 = ((int)VertexAttributeComponentType::TYPE_DOUBLE << 3) | 1,
		TYPE_DOUBLE2 = ((int)VertexAttributeComponentType::TYPE_DOUBLE << 3) | 2,
		TYPE_DOUBLE3 = ((int)VertexAttributeComponentType::TYPE_DOUBLE << 3) | 3,
		TYPE_DOUBLE4 = ((int)VertexAttributeComponentType::TYPE_DOUBLE << 3) | 4,

		TYPE_HALF1 = ((int)VertexAttributeComponentType::TYPE_HALF << 3) | 1,
		TYPE_HALF2 = ((int)VertexAttributeComponentType::TYPE_HALF << 3) | 2,
		TYPE_HALF3 = ((int)VertexAttributeComponentType::TYPE_HALF << 3) | 3,
		TYPE_HALF4 = ((int)VertexAttributeComponentType::TYPE_HALF << 3) | 4,

		TYPE_BYTE1 = ((int)VertexAttributeComponentType::TYPE_BYTE << 3) | 1,
		TYPE_BYTE2 = ((int)VertexAttributeComponentType::TYPE_BYTE << 3) | 2,
		TYPE_BYTE3 = ((int)VertexAttributeComponentType::TYPE_BYTE << 3) | 3,
		TYPE_BYTE4 = ((int)VertexAttributeComponentType::TYPE_BYTE << 3) | 4,

		TYPE_INT1 = ((int)VertexAttributeComponentType::TYPE_INT << 3) | 1,
		TYPE_INT2 = ((int)VertexAttributeComponentType::TYPE_INT << 3) | 2,
		TYPE_INT3 = ((int)VertexAttributeComponentType::TYPE_INT << 3) | 3,
		TYPE_INT4 = ((int)VertexAttributeComponentType::TYPE_INT << 3) | 4,
	};

	/**
	* Declaration of one component of a vertex
	*/
	class GPUVertexDeclarationEntry
	{
	public:

		/** compute the size in memory required for this component */
		int GetEntrySize() const;
		/** returns the number of component x, y, z, w = 1 .. 4 */
		int GetComponentCount() const;
		/** returns the type for component */
		GLenum GetComponentType() const;

	public:

		/** the semantic of the vertex component */
		VertexAttributeSemantic semantic = VertexAttributeSemantic::SEMANTIC_NONE;
		/** for repetition of the same semantic */
		int semantic_index = 0;
		/** the type of the vertex component */
		VertexAttributeType type;
		/** offset of this entry from the beginning of the vertex */
		int offset = 0;
	};

	/**
	* Declaration of a full vertex
	*/
	class GPUVertexDeclaration : public ReferencedObject
	{
	public:

		/** compute the size in memory required for this vertex */
		int GetVertexSize() const;

		/** returns the number of elements for a given semantic */
		int GetSemanticCount(VertexAttributeSemantic semantic) const;
		/** returns the number of position */
		int GetPositionCount() const;
		/** returns the number of color */
		int GetColorCount() const;
		/** returns the number of texture */
		int GetTextureCount() const;
		/** returns the number of bones */
		int GetBoneCount() const;

	public:

		/** insert an entry into the declaration */
		void Push(VertexAttributeSemantic semantic, int semantic_index, VertexAttributeType type);
		/** reset the object */
		void Clear(){ entries.clear(); }

		/** gets an entry from its semantic (ignore semantic_index if negative) */
		GPUVertexDeclarationEntry const * GetEntry(VertexAttributeSemantic semantic, int semantic_index) const;
		/** gets an entry from its semantic (ignore semantic_index if negative) */
		GPUVertexDeclarationEntry * GetEntry(VertexAttributeSemantic semantic, int semantic_index);

	public:

		/** all the entries of the declaration */
		std::vector<GPUVertexDeclarationEntry> entries;
	};
}; // namespace chaos

