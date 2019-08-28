#pragma once

#include <chaos/StandardHeaders.h>

namespace chaos
{
	/** the possible semantics */
	static int const SEMANTIC_POSITION   = 0;
	static int const SEMANTIC_COLOR      = 1;
	static int const SEMANTIC_NORMAL     = 2;
	static int const SEMANTIC_BINORMAL   = 3;
	static int const SEMANTIC_TANGENT    = 4;
	static int const SEMANTIC_TEXCOORD   = 5;
	static int const SEMANTIC_BONEINDEX  = 6;
	static int const SEMANTIC_BONEWEIGHT = 7;
	static int const SEMANTIC_USERDATA   = 8;

	/** the possible basic types */
	static int const TYPE_FLOAT  = 1;
	static int const TYPE_DOUBLE = 2;
	static int const TYPE_HALF   = 3;
	static int const TYPE_BYTE   = 4;
	static int const TYPE_INT    = 5;

	/** the possible count for vectors */
	static int const TYPE_COUNT_1 = 1;
	static int const TYPE_COUNT_2 = 2;
	static int const TYPE_COUNT_3 = 3;
	static int const TYPE_COUNT_4 = 4;

	/** the possible vector types */
	static int const TYPE_FLOAT1 = (TYPE_FLOAT << 3) | TYPE_COUNT_1;
	static int const TYPE_FLOAT2 = (TYPE_FLOAT << 3) | TYPE_COUNT_2;
	static int const TYPE_FLOAT3 = (TYPE_FLOAT << 3) | TYPE_COUNT_3;
	static int const TYPE_FLOAT4 = (TYPE_FLOAT << 3) | TYPE_COUNT_4;

	static int const TYPE_DOUBLE1 = (TYPE_DOUBLE << 3) | TYPE_COUNT_1;
	static int const TYPE_DOUBLE2 = (TYPE_DOUBLE << 3) | TYPE_COUNT_2;
	static int const TYPE_DOUBLE3 = (TYPE_DOUBLE << 3) | TYPE_COUNT_3;
	static int const TYPE_DOUBLE4 = (TYPE_DOUBLE << 3) | TYPE_COUNT_4;

	static int const TYPE_HALF1 = (TYPE_HALF << 3) | TYPE_COUNT_1;
	static int const TYPE_HALF2 = (TYPE_HALF << 3) | TYPE_COUNT_2;
	static int const TYPE_HALF3 = (TYPE_HALF << 3) | TYPE_COUNT_3;
	static int const TYPE_HALF4 = (TYPE_HALF << 3) | TYPE_COUNT_4;

	static int const TYPE_BYTE1 = (TYPE_BYTE << 3) | TYPE_COUNT_1;
	static int const TYPE_BYTE2 = (TYPE_BYTE << 3) | TYPE_COUNT_2;
	static int const TYPE_BYTE3 = (TYPE_BYTE << 3) | TYPE_COUNT_3;
	static int const TYPE_BYTE4 = (TYPE_BYTE << 3) | TYPE_COUNT_4;

	static int const TYPE_INT1 = (TYPE_INT << 3) | TYPE_COUNT_1;
	static int const TYPE_INT2 = (TYPE_INT << 3) | TYPE_COUNT_2;
	static int const TYPE_INT3 = (TYPE_INT << 3) | TYPE_COUNT_3;
	static int const TYPE_INT4 = (TYPE_INT << 3) | TYPE_COUNT_4;

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
		int semantic;
		/** for repetition of the same semantic */
		int semantic_index;
		/** the type of the vertex component */
		int type;
		/** offset of this entry from the beginning of the vertex */
		int offset;
	};

	/**
	* Declaration of a full vertex
	*/
	class GPUVertexDeclaration
	{
	public:

		/** compute the size in memory required for this vertex */
		int GetVertexSize() const;

		/** returns the number of elements for a given semantic */
		int GetSemanticCount(int semantic) const;
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
		void Push(int semantic, int semantic_index, int type);
		/** reset the object */
		void Clear(){ entries.clear(); }

		/** gets an entry from its semantic (ignore semantic_index if negative) */
		GPUVertexDeclarationEntry const * GetEntry(int semantic, int semantic_index) const;
		/** gets an entry from its semantic (ignore semantic_index if negative) */
		GPUVertexDeclarationEntry * GetEntry(int semantic, int semantic_index);

	public:

		/** all the entries of the declaration */
		std::vector<GPUVertexDeclarationEntry> entries;
	};
}; // namespace chaos
