#pragma once

#include <chaos/StandardHeaders.h>
#include <chaos/GPUClasses.h>

#include <chaos/ReferencedObject.h>

namespace chaos
{
	/** the possible semantics */
	enum class VertexAttributeSemantic : int
	{
		NONE = -1,
		POSITION = 0,
		COLOR = 1,
		NORMAL = 2,
		BINORMAL = 3,
		TANGENT = 4,
		TEXCOORD = 5,
		BONEINDEX = 6,
		BONEWEIGHT = 7,
		USERDATA = 8
	};

	/** the possible component types */
	enum class VertexAttributeComponentType : int
	{	
		FLOAT = 1,
		DOUBLE = 2,
		HALF = 3,
		BYTE = 4,
		INT = 5
	};

	/** the possible vector types */
	enum class VertexAttributeType : int
	{		
		FLOAT1 = ((int)VertexAttributeComponentType::FLOAT << 3) | 1,
		FLOAT2 = ((int)VertexAttributeComponentType::FLOAT << 3) | 2,
		FLOAT3 = ((int)VertexAttributeComponentType::FLOAT << 3) | 3,
		FLOAT4 = ((int)VertexAttributeComponentType::FLOAT << 3) | 4,

		DOUBLE1 = ((int)VertexAttributeComponentType::DOUBLE << 3) | 1,
		DOUBLE2 = ((int)VertexAttributeComponentType::DOUBLE << 3) | 2,
		DOUBLE3 = ((int)VertexAttributeComponentType::DOUBLE << 3) | 3,
		DOUBLE4 = ((int)VertexAttributeComponentType::DOUBLE << 3) | 4,

		HALF1 = ((int)VertexAttributeComponentType::HALF << 3) | 1,
		HALF2 = ((int)VertexAttributeComponentType::HALF << 3) | 2,
		HALF3 = ((int)VertexAttributeComponentType::HALF << 3) | 3,
		HALF4 = ((int)VertexAttributeComponentType::HALF << 3) | 4,

		BYTE1 = ((int)VertexAttributeComponentType::BYTE << 3) | 1,
		BYTE2 = ((int)VertexAttributeComponentType::BYTE << 3) | 2,
		BYTE3 = ((int)VertexAttributeComponentType::BYTE << 3) | 3,
		BYTE4 = ((int)VertexAttributeComponentType::BYTE << 3) | 4,

		INT1 = ((int)VertexAttributeComponentType::INT << 3) | 1,
		INT2 = ((int)VertexAttributeComponentType::INT << 3) | 2,
		INT3 = ((int)VertexAttributeComponentType::INT << 3) | 3,
		INT4 = ((int)VertexAttributeComponentType::INT << 3) | 4,
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
		VertexAttributeSemantic semantic = VertexAttributeSemantic::NONE;
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

