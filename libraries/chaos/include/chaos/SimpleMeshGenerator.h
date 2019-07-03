#pragma once

#include <chaos/StandardHeaders.h>
#include <chaos/GeometryFramework.h>
#include <chaos/SimpleMesh.h>
#include <chaos/ReferencedObject.h>
#include <chaos/MemoryBufferWriter.h>
#include <chaos/DrawPrimitive.h>

namespace chaos
{

	/**
	* A class to describe requirement for a mesh
	*/

	class MeshGenerationRequirement
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
	* SimpleMeshGenerator : an object that is responsible for generating the mesh data
	*/

	class SimpleMeshGenerator : public ReferencedObject
	{
	public:

		/** the destructor */
		virtual ~SimpleMeshGenerator() = default;

		/** get requirement */
		virtual MeshGenerationRequirement GetRequirement() const = 0;
		/** get the vertex declaration */
		virtual void GenerateVertexDeclaration(GPUVertexDeclaration & declaration) const = 0;
		/** get the mesh data */
		virtual void GenerateMeshData(std::vector<DrawPrimitive> & primitives, MemoryBufferWriter & vertices_writer, MemoryBufferWriter & indices_writer) const = 0;

		/** generation function */
		shared_ptr<SimpleMesh> GenerateMesh() const;
		/** population function */
		bool FillMeshData(SimpleMesh * mesh) const;
	};

	/**
	* PrimitiveMeshGenerator : a templated base class for primitives
	*/

	template<typename T>
	class PrimitiveMeshGenerator : public SimpleMeshGenerator
	{
	public:

		using primitive_type = T;

		/** constructor */
		PrimitiveMeshGenerator(primitive_type const & in_primitive, glm::mat4x4 const & in_transform = glm::scale(glm::vec3(1.0f, 1.0f, 1.0f))):
			primitive(in_primitive),
			transform(in_transform){}

	protected:

		/** the primitive that is been generated */
		primitive_type primitive;
		/** the transformation to apply to vertices */
		glm::mat4x4 transform;
	};

	/**
	* QuadMeshGenerator : help defines mesh as simple quad
	*/

	class QuadMeshGenerator : public PrimitiveMeshGenerator<box2>
	{

	public:

		using PrimitiveMeshGenerator::PrimitiveMeshGenerator;

		/** get requirement */
		virtual MeshGenerationRequirement GetRequirement() const override;
		/** get the vertex declaration */
		virtual void GenerateVertexDeclaration(GPUVertexDeclaration & declaration) const override;
		/** get the mesh data */
		virtual void GenerateMeshData(std::vector<DrawPrimitive> & primitives, MemoryBufferWriter & vertices_writer, MemoryBufferWriter & indices_writer) const override;

	protected:

		/** the vertices defining a face facing planes inside [-1, +1] */
		static glm::vec2 const vertices[4];
		/** the triangles indices defining a face facing planes */
		static GLuint const triangles[6];
	};

	/**
	* TriangleMeshGenerator : help defines mesh as simple traingle
	*/

	class TriangleMeshGenerator : public PrimitiveMeshGenerator<triangle3>
	{

	public:

		using PrimitiveMeshGenerator::PrimitiveMeshGenerator;

		/** get requirement */
		virtual MeshGenerationRequirement GetRequirement() const override;
		/** get the vertex declaration */
		virtual void GenerateVertexDeclaration(GPUVertexDeclaration & declaration) const override;
		/** get the mesh data */
		virtual void GenerateMeshData(std::vector<DrawPrimitive> & primitives, MemoryBufferWriter & vertices_writer, MemoryBufferWriter & indices_writer) const override;
	};

	/**
	* CubeMeshGenerator : help defines cube mesh
	*/

	class CubeMeshGenerator : public PrimitiveMeshGenerator<box3>
	{

	public:

		using PrimitiveMeshGenerator::PrimitiveMeshGenerator;

		/** get requirement */
		virtual MeshGenerationRequirement GetRequirement() const override;
		/** get the vertex declaration */
		virtual void GenerateVertexDeclaration(GPUVertexDeclaration & declaration) const override;
		/** get the mesh data */
		virtual void GenerateMeshData(std::vector<DrawPrimitive> & primitives, MemoryBufferWriter & vertices_writer, MemoryBufferWriter & indices_writer) const override;

	protected:

		/** the vertices defining a cube */
		static glm::vec3 const vertices[24 * 2];
		/** the triangles defining a cube */
		static GLuint const triangles[36];
	};

	/**
	* CircleMeshGenerator : help defines mesh as simple 2D circle
	*/

	class CircleMeshGenerator : public PrimitiveMeshGenerator<sphere2>
	{

	public:

		/** constructor */
		CircleMeshGenerator(sphere2 const & in_primitive, glm::mat4x4 const & in_transform = glm::scale(glm::vec3(1.0f, 1.0f, 1.0f)), int in_subdivisions = 10) :
			PrimitiveMeshGenerator<sphere2>(in_primitive, in_transform),
			subdivisions(in_subdivisions) {}

		/** get requirement */
		virtual MeshGenerationRequirement GetRequirement() const override;
		/** get the vertex declaration */
		virtual void GenerateVertexDeclaration(GPUVertexDeclaration & declaration) const override;
		/** get the mesh data */
		virtual void GenerateMeshData(std::vector<DrawPrimitive> & primitives, MemoryBufferWriter & vertices_writer, MemoryBufferWriter & indices_writer) const override;

	protected:

		/** number of subdivisions */
		int subdivisions = 10;
	};


	/**
	* SphereMeshGenerator : help defines mesh as simple sphere
	*/

	class SphereMeshGenerator : public PrimitiveMeshGenerator<sphere3>
	{

	public:

		/** constructor */
		SphereMeshGenerator(sphere3 const & in_primitive, glm::mat4x4 const & in_transform = glm::scale(glm::vec3(1.0f, 1.0f, 1.0f)), int in_subdivisions = 10) :
			PrimitiveMeshGenerator<sphere3>(in_primitive, in_transform),
			subdivisions(in_subdivisions) {}

		/** get requirement */
		virtual MeshGenerationRequirement GetRequirement() const override;
		/** get the vertex declaration */
		virtual void GenerateVertexDeclaration(GPUVertexDeclaration & declaration) const override;
		/** get the mesh data */
		virtual void GenerateMeshData(std::vector<DrawPrimitive> & primitives, MemoryBufferWriter & vertices_writer, MemoryBufferWriter & indices_writer) const override;

	protected:

		/** get a vertex on the sphere from polar angle */
		void InsertVertex(MemoryBufferWriter & vertices_writer, float alpha, float beta) const;

	protected:

		/** number of subdivisions */
		int subdivisions = 10;
	};

}; // namespace chaos

