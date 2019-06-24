#include <chaos/StandardHeaders.h> 
#include <chaos/GLMTools.h> 
#include <chaos/GPUProgramGenerator.h>
#include <chaos/GeometryFramework.h>
#include <chaos/CollisionFramework.h>
#include <chaos/SimpleMeshGenerator.h>
#include <chaos/SkyBoxTools.h>
#include <chaos/SimpleMesh.h>
#include <chaos/MultiMeshGenerator.h>
#include <chaos/GPUProgramData.h>
#include <chaos/GPUProgram.h>
#include <chaos/GPUVertexDeclaration.h>
#include <chaos/GPUProgramProvider.h>

#include "DrawFunctions.h"

static glm::vec4 const solid = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);
static glm::vec4 const translucent = glm::vec4(1.0f, 1.0f, 1.0f, 0.3f);

void RenderingContext::BeginTranslucency() const
{
	glEnable(GL_BLEND);
	glDepthMask(GL_FALSE);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
}

void RenderingContext::EndTranslucency() const
{
	glDepthMask(GL_TRUE);
	glDisable(GL_BLEND);
}


void RenderingContext::PrepareObjectProgram(chaos::GPUProgramProvider & uniform_provider, PrimitiveRenderingContext const & prim_ctx, float Y_Scale, chaos::GPUProgramProvider * next_provider) const
{
	uniform_provider.AddVariableValue("projection", projection);
	uniform_provider.AddVariableValue("world_to_camera", world_to_camera);
	uniform_provider.AddVariableValue("local_to_world", prim_ctx.local_to_world);
	uniform_provider.AddVariableValue("color", prim_ctx.color);
	uniform_provider.AddVariableValue("Y_Scale", Y_Scale);

	if (next_provider != nullptr)
		uniform_provider.AddVariableProvider(next_provider);
}

void RenderingContext::DrawPrimitiveImpl(
	chaos::SimpleMesh * mesh,
	chaos::GPUProgram  * program,
	glm::vec4 const & color,
	glm::mat4 const & local_to_world,
	bool is_translucent,
	float Y_Scale,
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
	PrepareObjectProgram(uniform_provider, prim_ctx, Y_Scale, next_provider);

	chaos::RenderParams render_params;
	mesh->Render(renderer, program, &uniform_provider, render_params);

	if (is_translucent)
		EndTranslucency();
}

void RenderingContext::DrawPrimitive(chaos::triangle3 const & t, glm::vec4 const & color, bool is_translucent) const
{
	glm::mat4 local_to_world = glm::scale(glm::vec3(1.0f, 1.0f, 1.0f));

	// cannot be on the stack. due to reference count
	chaos::shared_ptr<chaos::GPUProgramProvider> uniform_provider = new chaos::GPUProgramProvider;
	uniform_provider->AddVariableValue("p1", t.a);
	uniform_provider->AddVariableValue("p2", t.b);
	uniform_provider->AddVariableValue("p3", t.c);

	DrawPrimitiveImpl(
		mesh_triangle.get(),
		program_triangle.get(),
		color,
		local_to_world,
		is_translucent,
		1.0f,
		uniform_provider.get()
	);
}
	
void RenderingContext::DrawPrimitive(chaos::triangle2 const & t, glm::vec4 const & color, bool is_translucent) const
{
	chaos::triangle3 t3;
	t3.a = glm::vec3(t.a.x, 0.0f, t.a.y);
	t3.b = glm::vec3(t.b.x, 0.0f, t.b.y);
	t3.c = glm::vec3(t.c.x, 0.0f, t.c.y);
	DrawPrimitive(t3, color, is_translucent);
}

void RenderingContext::DrawPrimitive(chaos::sphere3 const & s, glm::vec4 const & color, bool is_translucent) const
{
	if (IsGeometryEmpty(s))
		return;

	glm::mat4 local_to_world =
		glm::translate(s.position) *
		glm::scale(glm::vec3(s.radius, s.radius, s.radius));

	DrawPrimitiveImpl(

		mesh_sphere.get(),
		program_sphere.get(),
		color,
		local_to_world,
		is_translucent,
		1.0f
	);
}

void RenderingContext::DrawPrimitive(chaos::sphere2 const & s, glm::vec4 const & color, bool is_translucent) const
{
	if (IsGeometryEmpty(s))
		return;

	glm::mat4 local_to_world =
		glm::translate(glm::vec3(s.position.x, 0.0f, s.position.y)) *
		glm::scale(glm::vec3(s.radius, s.radius, s.radius));

	DrawPrimitiveImpl(
		mesh_sphere.get(),
		program_sphere.get(),
		color,
		local_to_world,
		is_translucent,
		0.0f
	);
}

void RenderingContext::DrawPrimitive(chaos::box3 const & b, glm::vec4 const & color, bool is_translucent) const
{
	if (IsGeometryEmpty(b))
		return;

	glm::mat4 local_to_world =
		glm::translate(b.position) *
		glm::scale(b.half_size);

	DrawPrimitiveImpl(
		mesh_box.get(),
		program_box.get(),
		color,
		local_to_world,
		is_translucent,
		1.0f
	);
}

void RenderingContext::DrawPrimitive(chaos::box2 const & b, glm::vec4 const & color, bool is_translucent) const
{
	if (IsGeometryEmpty(b))
		return;

	glm::mat4 local_to_world =
		glm::translate(glm::vec3(b.position.x, 0.0f, b.position.y)) *
		glm::scale(glm::vec3(b.half_size.x, 1.0f, b.half_size.y));

	DrawPrimitiveImpl(
		mesh_box.get(),
		program_box.get(),
		color,
		local_to_world,
		is_translucent,
		0.0f
	);
}

void RenderingContext::DrawPrimitive(chaos::obox3 const & b, glm::vec4 const & color, bool is_translucent) const
{
	if (IsGeometryEmpty(b))
		return;

	glm::mat4 local_to_world =
		glm::translate(b.position) *
		chaos::GetRotatorMatrix(b.rotator) *
		glm::scale(b.half_size);

	DrawPrimitiveImpl(
		mesh_box.get(),
		program_box.get(),
		color,
		local_to_world,
		is_translucent,
		1.0f
	);
}

void RenderingContext::DrawPrimitive(chaos::obox2 const & b, glm::vec4 const & color, bool is_translucent) const
{
	if (IsGeometryEmpty(b))
		return;

	glm::mat4 local_to_world =
		glm::translate(glm::vec3(b.position.x, 0.0f, b.position.y)) *
		chaos::GetRotatorMatrix(b.rotator) *
		glm::scale(glm::vec3(b.half_size.x, 1.0f, b.half_size.y));

	DrawPrimitiveImpl(
		mesh_box.get(),
		program_box.get(),
		color,
		local_to_world,
		is_translucent,
		0.0f
	);
}

void RenderingContext::DrawPrimitive(glm::vec3 const & p, glm::vec4 const & color, bool is_translucent) const
{
	glm::vec3 half_point_size(0.125f);
	DrawPrimitive(chaos::box3(p, half_point_size), color, is_translucent);
}

void RenderingContext::DrawPrimitive(glm::vec2 const & p, glm::vec4 const & color, bool is_translucent) const
{
	glm::vec2 half_point_size(0.125f);
	DrawPrimitive(chaos::box2(p, half_point_size), color, is_translucent);
}


