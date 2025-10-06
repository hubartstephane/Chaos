namespace chaos
{
#ifdef CHAOS_FORWARD_DECLARATION

	enum class VertexAttributeSemantic;
	enum class VertexAttributeComponentType;
	enum class VertexAttributeType;

	class GPUVertexDeclarationEntry;
	class GPUVertexDeclaration;

#elif !defined CHAOS_TEMPLATE_IMPLEMENTATION

	/** the possible semantics */
	enum class CHAOS_API VertexAttributeSemantic : int
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
	enum class CHAOS_API VertexAttributeComponentType : int
	{
		FLOAT = 1,
		DOUBLE = 2,
		HALF = 3,
		BYTE = 4,
		INT = 5
	};

	/** the possible vector types */
	enum class CHAOS_API VertexAttributeType : int
	{
		FLOAT1 = (int(VertexAttributeComponentType::FLOAT) << 3) | 1,
		FLOAT2 = (int(VertexAttributeComponentType::FLOAT) << 3) | 2,
		FLOAT3 = (int(VertexAttributeComponentType::FLOAT) << 3) | 3,
		FLOAT4 = (int(VertexAttributeComponentType::FLOAT) << 3) | 4,

		DOUBLE1 = (int(VertexAttributeComponentType::DOUBLE) << 3) | 1,
		DOUBLE2 = (int(VertexAttributeComponentType::DOUBLE) << 3) | 2,
		DOUBLE3 = (int(VertexAttributeComponentType::DOUBLE) << 3) | 3,
		DOUBLE4 = (int(VertexAttributeComponentType::DOUBLE) << 3) | 4,

		HALF1 = (int(VertexAttributeComponentType::HALF) << 3) | 1,
		HALF2 = (int(VertexAttributeComponentType::HALF) << 3) | 2,
		HALF3 = (int(VertexAttributeComponentType::HALF) << 3) | 3,
		HALF4 = (int(VertexAttributeComponentType::HALF) << 3) | 4,

		BYTE1 = (int(VertexAttributeComponentType::BYTE) << 3) | 1,
		BYTE2 = (int(VertexAttributeComponentType::BYTE) << 3) | 2,
		BYTE3 = (int(VertexAttributeComponentType::BYTE) << 3) | 3,
		BYTE4 = (int(VertexAttributeComponentType::BYTE) << 3) | 4,

		INT1 = (int(VertexAttributeComponentType::INT) << 3) | 1,
		INT2 = (int(VertexAttributeComponentType::INT) << 3) | 2,
		INT3 = (int(VertexAttributeComponentType::INT) << 3) | 3,
		INT4 = (int(VertexAttributeComponentType::INT) << 3) | 4,
	};

	/**
	* Declaration of one component of a vertex
	*/
	class CHAOS_API GPUVertexDeclarationEntry
	{
	public:

		/** compute the size in memory required for this component */
		size_t GetEntrySize() const;
		/** returns the number of component x, y, z, w = 1 .. 4 */
		size_t GetComponentCount() const;
		/** returns the type for component */
		GLenum GetComponentType() const;
		/** get a hash */
		size_t GetHash() const;

	public:

		/** the semantic of the vertex component */
		VertexAttributeSemantic semantic = VertexAttributeSemantic::NONE;
		/** the type of the vertex component */
		VertexAttributeType type;
		/** for repetition of the same semantic */
		int semantic_index = 0;
		/** offset of this entry from the beginning of the vertex */
		size_t offset = 0;
		/** a name for the component */
		std::string name;
	};

	/**
	* Declaration of a full vertex
	*/
	class CHAOS_API GPUVertexDeclaration : public Object
	{
	public:

		/** compute the size in memory required for this vertex */
		size_t GetVertexSize() const;
		/** set the effective vertex size (for data that would not be declared) */
		void SetEffectiveVertexSize(size_t in_effective_size);

		/** returns the number of elements for a given semantic */
		size_t GetSemanticCount(VertexAttributeSemantic semantic) const;
		/** returns the number of position */
		size_t GetPositionCount() const;
		/** returns the number of color */
		size_t GetColorCount() const;
		/** returns the number of texture */
		size_t GetTextureCount() const;
		/** returns the number of bones */
		size_t GetBoneCount() const;

		/** get a hash for the declaration */
		size_t GetHash() const;

	public:

		/** insert an entry into the declaration */
		void Push(VertexAttributeSemantic semantic, int semantic_index, VertexAttributeType type, char const* name = nullptr);
		/** reset the object */
		void Clear() { entries.clear(); }

		/** gets an entry from its name */
		GPUVertexDeclarationEntry const* GetEntry(char const* name) const;
		/** gets an entry from its semantic (ignore semantic_index if negative) */
		GPUVertexDeclarationEntry const* GetEntry(VertexAttributeSemantic semantic, int semantic_index) const;

	protected:

		/** all the entries of the declaration */
		std::vector<GPUVertexDeclarationEntry> entries;
		/** the effective size of the vertex */
		mutable std::optional<size_t> effective_size;
		/** the vertex size */
		mutable std::optional<size_t> size;
		/** the hash for the declaration */
		mutable std::optional<size_t> hash;
	};

#endif

}; // namespace chaos