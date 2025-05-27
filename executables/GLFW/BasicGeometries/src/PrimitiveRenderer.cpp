#include "chaos/Chaos.h"
#include "PrimitiveRenderer.h"

static glm::vec4 const solid = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);
static glm::vec4 const translucent = glm::vec4(1.0f, 1.0f, 1.0f, 0.3f);

bool PrimitiveRenderer::Initialize()
{
	chaos::WindowApplication * window_application = chaos::Application::GetInstance();
	if (window_application == nullptr)
		return false;

	// compute resource path
	boost::filesystem::path resources_path = window_application->GetResourcesPath();

	// load programs
	program_common = LoadProgram(resources_path, "pixel_shader.txt", "vertex_shader_common.txt");
	if (program_common == nullptr)
		return false;

	program_triangle = LoadProgram(resources_path, "pixel_shader.txt", "vertex_shader_triangle.txt");
	if (program_triangle == nullptr)
		return false;

	// create meshes
	chaos::triangle3 t; // data will be initialized in vertex shader as uniform
	chaos::box2      b2 = chaos::box2(glm::vec2(0.0f, 0.0f), glm::vec2(1.0f, 1.0f));
	chaos::box3      b3 = chaos::box3(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(1.0f, 1.0f, 1.0f));
	chaos::sphere2   c = chaos::sphere2(glm::vec2(0.0f, 0.0f), 1.0f);
	chaos::sphere3   s = chaos::sphere3(glm::vec3(0.0f, 0.0f, 0.0f), 1.0f);

#if 1
	chaos::GPUMultiMeshGenerator generators;
	generators.AddGenerator(new chaos::GPUSphereMeshGenerator(s, glm::mat4x4(1.0f), 30), mesh_sphere);
	generators.AddGenerator(new chaos::GPUCircleMeshGenerator(c, glm::mat4x4(1.0f), 30), mesh_circle);
	generators.AddGenerator(new chaos::GPUQuadMeshGenerator(b2), mesh_quad);
	generators.AddGenerator(new chaos::GPUBoxMeshGenerator(b3), mesh_box);
	generators.AddGenerator(new chaos::GPUTriangleMeshGenerator(t), mesh_triangle);

	if (!generators.GenerateMeshes(window_application->GetGPUDevice()))
		return false;
#else

	mesh_sphere = (new chaos::GPUSphereMeshGenerator(s, glm::mat4x4(1.0f), 30))->GenerateMesh(window_application->GetGPUDevice());

	mesh_circle = (new chaos::GPUCircleMeshGenerator(c, glm::mat4x4(1.0f), 30))->GenerateMesh(window_application->GetGPUDevice());
	mesh_quad = (new chaos::GPUQuadMeshGenerator(b2))->GenerateMesh(window_application->GetGPUDevice());
	mesh_box = (new chaos::GPUBoxMeshGenerator(b3))->GenerateMesh(window_application->GetGPUDevice());
	mesh_triangle = (new chaos::GPUTriangleMeshGenerator(t))->GenerateMesh(window_application->GetGPUDevice());
#endif
	return true;
}

chaos::shared_ptr<chaos::GPUProgram> PrimitiveRenderer::LoadProgram(boost::filesystem::path const & resources_path, char const * ps_filename, char const * vs_filename)
{
	chaos::GPUProgramGenerator program_generator;
	program_generator.AddShaderSourceFile(chaos::ShaderType::FRAGMENT, resources_path / ps_filename);
	program_generator.AddShaderSourceFile(chaos::ShaderType::VERTEX, resources_path / vs_filename);

	return program_generator.GenProgramObject();
}

void PrimitiveRenderer::BeginTranslucency() const
{
	glEnable(GL_BLEND);
	glDepthMask(GL_FALSE);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
}

void PrimitiveRenderer::EndTranslucency() const
{
	glDepthMask(GL_TRUE);
	glDisable(GL_BLEND);
}

void PrimitiveRenderer::PrepareObjectProgram(chaos::GPUProgramProvider & uniform_provider, PrimitiveRenderingContext const & prim_ctx, chaos::GPUProgramProvider * next_provider) const
{
	uniform_provider.AddVariable("projection", projection);
	uniform_provider.AddVariable("world_to_camera", world_to_camera);
	uniform_provider.AddVariable("local_to_world", prim_ctx.local_to_world);
	uniform_provider.AddVariable("color", prim_ctx.color);

	uniform_provider.AddVariable("light_dir", glm::vec3(0.0f, 0.0f, 1.0f));

	if (next_provider != nullptr)
		uniform_provider.AddProvider(next_provider);
}

void PrimitiveRenderer::DrawPrimitiveImpl(
	chaos::GPUMesh * mesh,
	chaos::GPUProgram  * program,
	glm::vec4 const & color,
	glm::mat4 const & local_to_world,
	bool is_translucent,
	chaos::GPUProgramProvider * next_provider
) const
{
	glm::vec4 final_color = color;
	if (is_translucent)
	{
		BeginTranslucency();
		final_color *= translucent;
	}

	PrimitiveRenderingContext prim_ctx;
	prim_ctx.local_to_world = local_to_world;
	prim_ctx.color = final_color;

	chaos::GPUProgramProvider uniform_provider;
	PrepareObjectProgram(uniform_provider, prim_ctx, next_provider);

	chaos::GPURenderParams render_params;
	mesh->DisplayWithProgram(program, render_context, &uniform_provider, render_params);

	if (is_translucent)
		EndTranslucency();
}

void PrimitiveRenderer::DrawPrimitive(chaos::triangle3 const & t, glm::vec4 const & color, bool is_translucent) const
{
	glDisable(GL_CULL_FACE); // XXX : the quad generation, produce a bad oriented quad in our case, fix the rendering with a hack

	glm::mat4 local_to_world = glm::scale(glm::vec3(1.0f, 1.0f, 1.0f));

	// cannot be on the stack. due to reference count
	chaos::shared_ptr<chaos::GPUProgramProvider> uniform_provider = new chaos::GPUProgramProvider;
	uniform_provider->AddVariable("p1", t.a);
	uniform_provider->AddVariable("p2", t.b);
	uniform_provider->AddVariable("p3", t.c);

	DrawPrimitiveImpl(
		mesh_triangle.get(),
		program_triangle.get(),
		color,
		local_to_world,
		is_translucent,
		uniform_provider.get()
	);
	glEnable(GL_CULL_FACE);
}

void PrimitiveRenderer::DrawPrimitive(chaos::triangle2 const & t, glm::vec4 const & color, bool is_translucent) const
{
	chaos::triangle3 t3;
	t3.a = glm::vec3(t.a.x, t.a.y, 0.0f);
	t3.b = glm::vec3(t.b.x, t.b.y, 0.0f);
	t3.c = glm::vec3(t.c.x, t.c.y, 0.0f);
	DrawPrimitive(t3, color, is_translucent);
}

void PrimitiveRenderer::DrawPrimitive(chaos::sphere3 const & s, glm::vec4 const & color, bool is_translucent) const
{
	if (IsGeometryEmpty(s))
		return;

	glm::mat4 local_to_world =
		glm::translate(s.position) *
		glm::scale(glm::vec3(s.radius, s.radius, s.radius));

	DrawPrimitiveImpl(
		mesh_sphere.get(),
		program_common.get(),
		color,
		local_to_world,
		is_translucent
	);
}

void PrimitiveRenderer::DrawPrimitive(chaos::sphere2 const & s, glm::vec4 const & color, bool is_translucent) const
{
	if (IsGeometryEmpty(s))
		return;

	glm::mat4 local_to_world =
		glm::translate(glm::vec3(s.position.x, s.position.y, 0.0f)) *
		glm::scale(glm::vec3(s.radius, s.radius, s.radius));

	DrawPrimitiveImpl(
		mesh_circle.get(),
		program_common.get(),
		color,
		local_to_world,
		is_translucent
	);
}

void PrimitiveRenderer::DrawPrimitive(chaos::box3 const & b, glm::vec4 const & color, bool is_translucent) const
{
	if (IsGeometryEmpty(b))
		return;

	glm::mat4 local_to_world =
		glm::translate(b.position) *
		glm::scale(b.half_size);

	DrawPrimitiveImpl(
		mesh_box.get(),
		program_common.get(),
		color,
		local_to_world,
		is_translucent
	);
}

void PrimitiveRenderer::DrawPrimitive(chaos::box2 const & b, glm::vec4 const & color, bool is_translucent) const
{
	glDisable(GL_CULL_FACE); // XXX : the quad generation, produce a bad oriented quad in our case, fix the rendering with a hack

	if (IsGeometryEmpty(b))
		return;

	glm::mat4 local_to_world =
		glm::translate(glm::vec3(b.position.x, b.position.y, 0.0f)) *
		glm::scale(glm::vec3(b.half_size.x, b.half_size.y, 1.0f));

	DrawPrimitiveImpl(
		mesh_quad.get(),
		program_common.get(),
		color,
		local_to_world,
		is_translucent
	);

	glEnable(GL_CULL_FACE);
}

void PrimitiveRenderer::DrawPrimitive(chaos::obox3 const & b, glm::vec4 const & color, bool is_translucent) const
{
	if (IsGeometryEmpty(b))
		return;

	glm::mat4 local_to_world =
		glm::translate(b.position) *
		chaos::GetRotatorMatrix(b.rotator) *
		glm::scale(b.half_size);

	DrawPrimitiveImpl(
		mesh_box.get(),
		program_common.get(),
		color,
		local_to_world,
		is_translucent
	);
}

void PrimitiveRenderer::DrawPrimitive(chaos::obox2 const & b, glm::vec4 const & color, bool is_translucent) const
{
	glDisable(GL_CULL_FACE); // XXX : the quad generation, produce a bad oriented quad in our case, fix the rendering with a hack

	if (IsGeometryEmpty(b))
		return;

	glm::mat4 local_to_world =
		glm::translate(glm::vec3(b.position.x, b.position.y, 0.0f)) *
		chaos::GetRotatorMatrix(b.rotator) *
		glm::scale(glm::vec3(b.half_size.x, b.half_size.y, 1.0f));

	DrawPrimitiveImpl(
		mesh_quad.get(),
		program_common.get(),
		color,
		local_to_world,
		is_translucent
	);

	glEnable(GL_CULL_FACE);
}

void PrimitiveRenderer::DrawPrimitive(glm::vec3 const & p, glm::vec4 const & color, bool is_translucent) const
{
	glm::vec3 half_point_size(0.125f);
	DrawPrimitive(chaos::box3(p, half_point_size), color, is_translucent);
}

void PrimitiveRenderer::DrawPrimitive(glm::vec2 const & p, glm::vec4 const & color, bool is_translucent) const
{
	glm::vec2 half_point_size(0.125f);
	DrawPrimitive(chaos::box2(p, half_point_size), color, is_translucent);
}


