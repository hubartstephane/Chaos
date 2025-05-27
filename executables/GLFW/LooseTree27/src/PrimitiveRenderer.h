#include "chaos/Chaos.h"

class PrimitiveRenderingContext
{
public:

	glm::mat4 local_to_world;
	glm::vec4 color;
};

class PrimitiveRenderer : public chaos::Object
{
public:

	bool Initialize();

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

	void BeginTranslucency() const;

	void EndTranslucency() const;

	void DrawPrimitive(chaos::triangle3 const & t, glm::vec4 const & color, bool is_translucent) const;

	void DrawPrimitive(chaos::triangle2 const & t, glm::vec4 const & color, bool is_translucent) const;

	void DrawPrimitive(chaos::sphere3 const & s, glm::vec4 const & color, bool is_translucent) const;

	void DrawPrimitive(chaos::sphere2 const & s, glm::vec4 const & color, bool is_translucent) const;

	void DrawPrimitive(chaos::box3 const & b, glm::vec4 const & color, bool is_translucent, bool is_wireframe) const;

	void DrawPrimitive(chaos::box2 const & b, glm::vec4 const & color, bool is_translucent) const;

	void DrawPrimitive(chaos::obox3 const & b, glm::vec4 const & color, bool is_translucent, bool is_wireframe) const;

	void DrawPrimitive(chaos::obox2 const & b, glm::vec4 const & color, bool is_translucent) const;

	void DrawPrimitive(glm::vec3 const & p, glm::vec4 const & color, bool is_translucent) const;

	void DrawPrimitive(glm::vec2 const & p, glm::vec4 const & color, bool is_translucent) const;

protected:

	chaos::shared_ptr<chaos::GPUProgram> LoadProgram(boost::filesystem::path const & resources_path, char const * ps_filename, char const * vs_filename);

	void DrawPrimitiveImpl(
		chaos::GPUMesh * mesh,
		chaos::GPUProgram  * program,
		glm::vec4 const & color,
		glm::mat4 const & local_to_world,
		bool is_translucent,
		chaos::GPUProgramProvider * next_provider = nullptr
	) const;

	void PrepareObjectProgram(chaos::GPUProgramProvider & uniform_provider, PrimitiveRenderingContext const & prim_ctx, chaos::GPUProgramProvider * next_provider = nullptr) const;

public:

	chaos::GPURenderContext * render_context = nullptr;

	glm::mat4 projection = glm::mat4(1.0f);
	glm::mat4 world_to_camera = glm::mat4(1.0f);

	// the meshes
	chaos::shared_ptr<chaos::GPUMesh> mesh_box;
	chaos::shared_ptr<chaos::GPUMesh> mesh_wireframe_box;
	chaos::shared_ptr<chaos::GPUMesh> mesh_triangle;
	chaos::shared_ptr<chaos::GPUMesh> mesh_sphere;
	chaos::shared_ptr<chaos::GPUMesh> mesh_circle;
	chaos::shared_ptr<chaos::GPUMesh> mesh_quad;

	// the programs
	chaos::shared_ptr<chaos::GPUProgram>  program_common;
	chaos::shared_ptr<chaos::GPUProgram>  program_triangle;
};



