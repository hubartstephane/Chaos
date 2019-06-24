#include <chaos/StandardHeaders.h> 
#include <chaos/GLTools.h> 
#include <chaos/GLMTools.h> 
#include <chaos/GeometryFramework.h>
#include <chaos/CollisionFramework.h>
#include <chaos/SimpleMeshGenerator.h>
#include <chaos/SimpleMesh.h>
#include <chaos/MultiMeshGenerator.h>
#include <chaos/GPUProgramData.h>
#include <chaos/GPUProgram.h>
#include <chaos/GPUVertexDeclaration.h>
#include <chaos/GPUProgramProvider.h>
#include <chaos/ConvexPolygonSplitter.h>


class PrimitiveRenderingContext
{
public:

	glm::mat4 local_to_world;
	glm::vec4 color;
};

class RenderingContext
{
public:

	template<typename T, int dimension>
	chaos::type_box<T, dimension> SlightIncreaseSize(chaos::type_box<T, dimension> src) const
	{
		src.half_size *= static_cast<T>(1.01);
		return src;
	}

	template<typename T, int dimension>
	chaos::type_obox<T, dimension> SlightIncreaseSize(chaos::type_obox<T, dimension> src) const
	{
		src.half_size *= static_cast<T>(1.01);
		return src;
	}

	template<typename T, int dimension>
	chaos::type_sphere<T, dimension> SlightIncreaseSize(chaos::type_sphere<T, dimension> src) const
	{
		src.radius *= static_cast<T>(1.01);
		return src;
	}


	template<typename T, int dimension>
	chaos::type_box<T, dimension> SlightDecreaseSize(chaos::type_box<T, dimension> src) const
	{
		src.half_size *= static_cast<T>(0.90);
		return src;
	}

	template<typename T, int dimension>
	chaos::type_obox<T, dimension> SlightDecreaseSize(chaos::type_obox<T, dimension> src) const
	{
		src.half_size *= static_cast<T>(0.90);
		return src;
	}

	template<typename T, int dimension>
	chaos::type_sphere<T, dimension> SlightDecreaseSize(chaos::type_sphere<T, dimension> src) const
	{
		src.radius *= static_cast<T>(0.90);
		return src;
	}

	void PrepareObjectProgram(chaos::GPUProgramProvider & uniform_provider, PrimitiveRenderingContext const & prim_ctx, float Y_Scale, chaos::GPUProgramProvider * next_provider = nullptr) const;

	void BeginTranslucency() const;

	void EndTranslucency() const;

	void DrawPrimitiveImpl(
		chaos::SimpleMesh * mesh,
		chaos::GPUProgram  * program,
		glm::vec4 const & color,
		glm::mat4 const & local_to_world,
		bool is_translucent,
		float Y_Scale,
		chaos::GPUProgramProvider * next_provider = nullptr
	) const;

	void DrawPrimitive(chaos::triangle3 const & t, glm::vec4 const & color, bool is_translucent) const;

	void DrawPrimitive(chaos::triangle2 const & t, glm::vec4 const & color, bool is_translucent) const;

	void DrawPrimitive(chaos::sphere3 const & s, glm::vec4 const & color, bool is_translucent) const;

	void DrawPrimitive(chaos::sphere2 const & s, glm::vec4 const & color, bool is_translucent) const;

	void DrawPrimitive(chaos::box3 const & b, glm::vec4 const & color, bool is_translucent) const;

	void DrawPrimitive(chaos::box2 const & b, glm::vec4 const & color, bool is_translucent) const;

	void DrawPrimitive(chaos::obox3 const & b, glm::vec4 const & color, bool is_translucent) const;

	void DrawPrimitive(chaos::obox2 const & b, glm::vec4 const & color, bool is_translucent) const;

	void DrawPrimitive(glm::vec3 const & p, glm::vec4 const & color, bool is_translucent) const;

	void DrawPrimitive(glm::vec2 const & p, glm::vec4 const & color, bool is_translucent) const;

public:

	chaos::Renderer * renderer = nullptr;

	glm::mat4 projection;
	glm::mat4 world_to_camera;

	// rendering for the box  
	chaos::shared_ptr<chaos::SimpleMesh> mesh_box;
	chaos::shared_ptr<chaos::GPUProgram>  program_box;

	// rendering for the triangle  
	chaos::shared_ptr<chaos::SimpleMesh> mesh_triangle;
	chaos::shared_ptr<chaos::GPUProgram>  program_triangle;

	// rendering for the rect
	chaos::shared_ptr<chaos::SimpleMesh> mesh_sphere;
	chaos::shared_ptr<chaos::GPUProgram>  program_sphere;
};



