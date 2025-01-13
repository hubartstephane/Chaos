namespace chaos
{
#ifdef CHAOS_FORWARD_DECLARATION

	class GPUMeshGenerationRequirement;
	class GPUMeshGenerator;

	template<typename T>
	class GPUPrimitiveMeshGenerator;

	class GPUQuadMeshGenerator;
	class GPUTriangleMeshGenerator;
	class GPUBoxMeshGenerator;
	class GPUWireframeBoxMeshGenerator;
	class GPUCircleMeshGenerator;
	class GPUSphereMeshGenerator;

	using GPUMeshGenerationElementCreationFunc = LightweightFunction<GPUMeshElement &()>;

#elif !defined CHAOS_TEMPLATE_IMPLEMENTATION

	/**
	* A class to describe requirement for a mesh
	*/

	class CHAOS_API GPUMeshGenerationRequirement
	{
	public:

		/** test whether the requirement is valid */
		bool IsValid() const;

	public:

		/** number of vertices required */
		size_t vertices_count = 0;
		/** number of indices required */
		size_t indices_count = 0;
	};

	/**
	* GPUMeshGenerator : an object that is responsible for generating the mesh data
	*/

	class CHAOS_API GPUMeshGenerator : public Object
	{
		friend class GPUMultiMeshGenerator;

	public:

		/** the destructor */
		virtual ~GPUMeshGenerator() = default;

		/** get requirement */
		virtual GPUMeshGenerationRequirement GetRequirement() const = 0;
		/** get the vertex declaration */
		virtual GPUVertexDeclaration* GenerateVertexDeclaration() const = 0;

		/** generation function */
		shared_ptr<GPUMesh> GenerateMesh() const;

	protected:

		/** get the mesh data */
		virtual void GenerateMeshData(GPUMeshGenerationElementCreationFunc elem_create_func, MemoryBufferWriter& vertices_writer, MemoryBufferWriter& indices_writer, box3& bounding_box) const = 0;
		/** population function */
		bool FillMeshData(GPUMesh* mesh) const;
	};

	/**
	* GPUPrimitiveMeshGenerator : a templated base class for primitives
	*/

	template<typename T>
	class GPUPrimitiveMeshGenerator : public GPUMeshGenerator
	{
	public:

		using primitive_type = T;

		/** constructor */
		GPUPrimitiveMeshGenerator(primitive_type const& in_primitive, glm::mat4x4 const& in_transform = glm::mat4x4(1.0f)) :
			primitive(in_primitive),
			transform(in_transform),
			transform_normal(glm::inverse(glm::transpose(in_transform))){}

	protected:

		/** transform a position */
		glm::vec3 TransformPosition(glm::vec3 const& position) const
		{
			return GLMTools::MultWithTranslation(transform, position);
		}

		/** transform a normal */
		glm::vec3 TransformNormal(glm::vec3 const& normal) const
		{
			return normalize(GLMTools::Mult(transform_normal, normal));
		}

	protected:

		/** the primitive that is been generated */
		primitive_type primitive;
		/** the transformation to apply to vertices */
		glm::mat4x4 transform;
		/** the transformation to apply to normals */
		glm::mat4x4 transform_normal;
	};

	/**
	* GPUQuadMeshGenerator : help defines mesh as simple quad
	*/

	class CHAOS_API GPUQuadMeshGenerator : public GPUPrimitiveMeshGenerator<box2>
	{

	public:

		using GPUPrimitiveMeshGenerator::GPUPrimitiveMeshGenerator;

		/** override */
		virtual GPUMeshGenerationRequirement GetRequirement() const override;
		/** override */
		virtual GPUVertexDeclaration* GenerateVertexDeclaration() const override;

	protected:

		/** override */
		virtual void GenerateMeshData(GPUMeshGenerationElementCreationFunc elem_create_func, MemoryBufferWriter& vertices_writer, MemoryBufferWriter& indices_writer, box3& bounding_box) const override;

	protected:

		/** the vertices defining a face facing planes inside [-1, +1] */
		static glm::vec3 const vertices[4];
		/** the triangles indices defining a face facing planes */
		static uint32_t const triangles[6];
	};

	/**
	* GPUTriangleMeshGenerator : help defines mesh as simple traingle
	*/

	class CHAOS_API GPUTriangleMeshGenerator : public GPUPrimitiveMeshGenerator<triangle3>
	{

	public:

		using GPUPrimitiveMeshGenerator::GPUPrimitiveMeshGenerator;

		/** override */
		virtual GPUMeshGenerationRequirement GetRequirement() const override;
		/** override */
		virtual GPUVertexDeclaration* GenerateVertexDeclaration() const override;

	protected:

		/** override */
		virtual void GenerateMeshData(GPUMeshGenerationElementCreationFunc elem_create_func, MemoryBufferWriter& vertices_writer, MemoryBufferWriter& indices_writer, box3& bounding_box) const override;
	};

	/**
	* GPUBoxMeshGenerator : help defines cube mesh
	*/

	class CHAOS_API GPUBoxMeshGenerator : public GPUPrimitiveMeshGenerator<box3>
	{

	public:

		using GPUPrimitiveMeshGenerator::GPUPrimitiveMeshGenerator;

		/** override */
		virtual GPUMeshGenerationRequirement GetRequirement() const override;
		/** override */
		virtual GPUVertexDeclaration* GenerateVertexDeclaration() const override;

	protected:

		/** override */
		virtual void GenerateMeshData(GPUMeshGenerationElementCreationFunc elem_create_func, MemoryBufferWriter& vertices_writer, MemoryBufferWriter& indices_writer, box3& bounding_box) const override;

	protected:

		/** the vertices defining a cube */
		static glm::vec3 const vertices[24 * 2];
		/** the triangles defining a cube */
		static uint32_t const triangles[36];
	};

	/**
	* GPUWireframeBoxMeshGenerator : help defines cube mesh
	*/

	class CHAOS_API GPUWireframeBoxMeshGenerator : public GPUPrimitiveMeshGenerator<box3>
	{

	public:

		using GPUPrimitiveMeshGenerator::GPUPrimitiveMeshGenerator;

		/** override */
		virtual GPUMeshGenerationRequirement GetRequirement() const override;
		/** override */
		virtual GPUVertexDeclaration* GenerateVertexDeclaration() const override;

	protected:

		/** override */
		virtual void GenerateMeshData(GPUMeshGenerationElementCreationFunc elem_create_func, MemoryBufferWriter& vertices_writer, MemoryBufferWriter& indices_writer, box3& bounding_box) const override;

	protected:

		/** the vertices defining a cube */
		static glm::vec3 const vertices[8];
		/** the indices defining a cube */
		static uint32_t const indices[24];
	};

	/**
	* GPUCircleMeshGenerator : help defines mesh as simple 2D circle
	*/

	class CHAOS_API GPUCircleMeshGenerator : public GPUPrimitiveMeshGenerator<sphere2>
	{

	public:

		/** constructor */
		GPUCircleMeshGenerator(sphere2 const& in_primitive, glm::mat4x4 const& in_transform = glm::mat4x4(1.0f), int in_subdivisions = 10) :
			GPUPrimitiveMeshGenerator<sphere2>(in_primitive, in_transform),
			subdivisions(in_subdivisions) {}

		/** override */
		virtual GPUMeshGenerationRequirement GetRequirement() const override;
		/** override */
		virtual GPUVertexDeclaration* GenerateVertexDeclaration() const override;

	protected:

		/** override */
		virtual void GenerateMeshData(GPUMeshGenerationElementCreationFunc elem_create_func, MemoryBufferWriter& vertices_writer, MemoryBufferWriter& indices_writer, box3& bounding_box) const override;

	protected:

		/** number of subdivisions */
		int subdivisions = 10;
	};


	/**
	* GPUSphereMeshGenerator : help defines mesh as simple sphere
	*/

	class CHAOS_API GPUSphereMeshGenerator : public GPUPrimitiveMeshGenerator<sphere3>
	{

	public:

		/** constructor */
		GPUSphereMeshGenerator(sphere3 const& in_primitive, glm::mat4x4 const& in_transform = glm::mat4x4(1.0f), int in_subdivisions = 10) :
			GPUPrimitiveMeshGenerator<sphere3>(in_primitive, in_transform),
			subdivisions(in_subdivisions) {}

		/** override */
		virtual GPUMeshGenerationRequirement GetRequirement() const override;
		/** override */
		virtual GPUVertexDeclaration* GenerateVertexDeclaration() const override;
		/** override */

	protected:

		/** override */
		virtual void GenerateMeshData(GPUMeshGenerationElementCreationFunc elem_create_func, MemoryBufferWriter& vertices_writer, MemoryBufferWriter& indices_writer, box3& bounding_box) const override;

	protected:

		/** number of subdivisions */
		int subdivisions = 10;
	};

#endif

}; // namespace chaos