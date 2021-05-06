#ifdef CHAOS_FORWARD_DECLARATION

namespace chaos
{
	class GPUMeshGenerationRequirement;
	class GPUSimpleMeshGenerator;

	template<typename T>
	class GPUPrimitiveMeshGenerator;

	class GPUQuadMeshGenerator;
	class GPUTriangleMeshGenerator;
	class GPUCubeMeshGenerator;
	class GPUCircleMeshGenerator;
	class GPUSphereMeshGenerator;

}; // namespace chaos

#elif defined CHAOS_TEMPLATE_IMPLEMENTATION


#else 

namespace chaos
{

	/**
	* A class to describe requirement for a mesh
	*/

	class GPUMeshGenerationRequirement
	{
	public:

		/** test whether the requirement is valid */
		bool IsValid() const;

	public:

		/** size of a vertex */
		int vertex_size = 0;
		/** number of vertices required */
		int vertices_count = 0;
		/** number of indices required */
		int indices_count = 0;
	};

	/**
	* GPUSimpleMeshGenerator : an object that is responsible for generating the mesh data
	*/

	class GPUSimpleMeshGenerator : public Object
	{
	public:

		/** the destructor */
		virtual ~GPUSimpleMeshGenerator() = default;

		/** get requirement */
		virtual GPUMeshGenerationRequirement GetRequirement() const = 0;
		/** get the vertex declaration */
		virtual GPUVertexDeclaration * GenerateVertexDeclaration() const = 0;
		/** get the mesh data */
		virtual void GenerateMeshData(std::vector<GPUDrawPrimitive> & primitives, MemoryBufferWriter & vertices_writer, MemoryBufferWriter & indices_writer) const = 0;

		/** generation function */
		shared_ptr<GPUSimpleMesh> GenerateMesh() const;
		/** population function */
		bool FillMeshData(GPUSimpleMesh * mesh) const;
	};

	/**
	* GPUPrimitiveMeshGenerator : a templated base class for primitives
	*/

	template<typename T>
	class GPUPrimitiveMeshGenerator : public GPUSimpleMeshGenerator
	{
	public:

		using primitive_type = T;

		/** constructor */
		GPUPrimitiveMeshGenerator(primitive_type const & in_primitive, glm::mat4x4 const & in_transform = glm::mat4x4()):
			primitive(in_primitive),
			transform(in_transform){}

	protected:

		/** the primitive that is been generated */
		primitive_type primitive;
		/** the transformation to apply to vertices */
		glm::mat4x4 transform;
	};

	/**
	* GPUQuadMeshGenerator : help defines mesh as simple quad
	*/

	class GPUQuadMeshGenerator : public GPUPrimitiveMeshGenerator<box2>
	{

	public:

		using GPUPrimitiveMeshGenerator::GPUPrimitiveMeshGenerator;

		/** get requirement */
		virtual GPUMeshGenerationRequirement GetRequirement() const override;
		/** get the vertex declaration */
		virtual GPUVertexDeclaration * GenerateVertexDeclaration() const override;
		/** get the mesh data */
		virtual void GenerateMeshData(std::vector<GPUDrawPrimitive> & primitives, MemoryBufferWriter & vertices_writer, MemoryBufferWriter & indices_writer) const override;

	protected:

		/** the vertices defining a face facing planes inside [-1, +1] */
		static glm::vec2 const vertices[4];
		/** the triangles indices defining a face facing planes */
		static GLuint const triangles[6];
	};

	/**
	* GPUTriangleMeshGenerator : help defines mesh as simple traingle
	*/

	class GPUTriangleMeshGenerator : public GPUPrimitiveMeshGenerator<triangle3>
	{

	public:

		using GPUPrimitiveMeshGenerator::GPUPrimitiveMeshGenerator;

		/** get requirement */
		virtual GPUMeshGenerationRequirement GetRequirement() const override;
		/** get the vertex declaration */
		virtual GPUVertexDeclaration * GenerateVertexDeclaration() const override;
		/** get the mesh data */
		virtual void GenerateMeshData(std::vector<GPUDrawPrimitive> & primitives, MemoryBufferWriter & vertices_writer, MemoryBufferWriter & indices_writer) const override;
	};

	/**
	* GPUCubeMeshGenerator : help defines cube mesh
	*/

	class GPUCubeMeshGenerator : public GPUPrimitiveMeshGenerator<box3>
	{

	public:

		using GPUPrimitiveMeshGenerator::GPUPrimitiveMeshGenerator;

		/** get requirement */
		virtual GPUMeshGenerationRequirement GetRequirement() const override;
		/** get the vertex declaration */
		virtual GPUVertexDeclaration * GenerateVertexDeclaration() const override;
		/** get the mesh data */
		virtual void GenerateMeshData(std::vector<GPUDrawPrimitive> & primitives, MemoryBufferWriter & vertices_writer, MemoryBufferWriter & indices_writer) const override;

	protected:

		/** the vertices defining a cube */
		static glm::vec3 const vertices[24 * 2];
		/** the triangles defining a cube */
		static GLuint const triangles[36];
	};

	/**
	* GPUCircleMeshGenerator : help defines mesh as simple 2D circle
	*/

	class GPUCircleMeshGenerator : public GPUPrimitiveMeshGenerator<sphere2>
	{

	public:

		/** constructor */
		GPUCircleMeshGenerator(sphere2 const & in_primitive, glm::mat4x4 const & in_transform = glm::mat4x4(), int in_subdivisions = 10) :
			GPUPrimitiveMeshGenerator<sphere2>(in_primitive, in_transform),
			subdivisions(in_subdivisions) {}

		/** get requirement */
		virtual GPUMeshGenerationRequirement GetRequirement() const override;
		/** get the vertex declaration */
		virtual GPUVertexDeclaration * GenerateVertexDeclaration() const override;
		/** get the mesh data */
		virtual void GenerateMeshData(std::vector<GPUDrawPrimitive> & primitives, MemoryBufferWriter & vertices_writer, MemoryBufferWriter & indices_writer) const override;

	protected:

		/** number of subdivisions */
		int subdivisions = 10;
	};


	/**
	* GPUSphereMeshGenerator : help defines mesh as simple sphere
	*/

	class GPUSphereMeshGenerator : public GPUPrimitiveMeshGenerator<sphere3>
	{

	public:

		/** constructor */
		GPUSphereMeshGenerator(sphere3 const & in_primitive, glm::mat4x4 const & in_transform = glm::mat4x4(), int in_subdivisions = 10) :
			GPUPrimitiveMeshGenerator<sphere3>(in_primitive, in_transform),
			subdivisions(in_subdivisions) {}

		/** get requirement */
		virtual GPUMeshGenerationRequirement GetRequirement() const override;
		/** get the vertex declaration */
		virtual GPUVertexDeclaration * GenerateVertexDeclaration() const override;
		/** get the mesh data */
		virtual void GenerateMeshData(std::vector<GPUDrawPrimitive> & primitives, MemoryBufferWriter & vertices_writer, MemoryBufferWriter & indices_writer) const override;

	protected:

		/** get a vertex on the sphere from polar angle */
		void InsertVertex(MemoryBufferWriter & vertices_writer, float alpha, float beta) const;

	protected:

		/** number of subdivisions */
		int subdivisions = 10;
	};

}; // namespace chaos

#endif // CHAOS_FORWARD_DECLARATION


