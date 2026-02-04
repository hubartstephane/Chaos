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
	enum class VertexAttributeSemantic : int
	{
		None = -1,
		Position = 0,
		Color = 1,
		Normal = 2,
		Binormal = 3,
		Tangent = 4,
		Texcoord = 5,
		BoneIndex = 6,
		BoneWeight = 7,
		UserData = 8
	};

	/** the possible component types */
	enum class VertexAttributeComponentType : int
	{
		Float = 1,
		Double = 2,
		Half = 3,
		Byte = 4,
		Int = 5
	};

	/** the possible vector types */
	enum class VertexAttributeType : int
	{
		Float1 = (int(VertexAttributeComponentType::Float) << 3) | 1,
		Float2 = (int(VertexAttributeComponentType::Float) << 3) | 2,
		Float3 = (int(VertexAttributeComponentType::Float) << 3) | 3,
		Float4 = (int(VertexAttributeComponentType::Float) << 3) | 4,

		Double1 = (int(VertexAttributeComponentType::Double) << 3) | 1,
		Double2 = (int(VertexAttributeComponentType::Double) << 3) | 2,
		Double3 = (int(VertexAttributeComponentType::Double) << 3) | 3,
		Double4 = (int(VertexAttributeComponentType::Double) << 3) | 4,

		Half1 = (int(VertexAttributeComponentType::Half) << 3) | 1,
		Half2 = (int(VertexAttributeComponentType::Half) << 3) | 2,
		Half3 = (int(VertexAttributeComponentType::Half) << 3) | 3,
		Half4 = (int(VertexAttributeComponentType::Half) << 3) | 4,

		Byte1 = (int(VertexAttributeComponentType::Byte) << 3) | 1,
		Byte2 = (int(VertexAttributeComponentType::Byte) << 3) | 2,
		Byte3 = (int(VertexAttributeComponentType::Byte) << 3) | 3,
		Byte4 = (int(VertexAttributeComponentType::Byte) << 3) | 4,

		Int1 = (int(VertexAttributeComponentType::Int) << 3) | 1,
		Int2 = (int(VertexAttributeComponentType::Int) << 3) | 2,
		Int3 = (int(VertexAttributeComponentType::Int) << 3) | 3,
		Int4 = (int(VertexAttributeComponentType::Int) << 3) | 4,
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
		VertexAttributeSemantic semantic = VertexAttributeSemantic::None;
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