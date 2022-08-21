#include "chaos/Chaos.h"

#include <typeinfo>
#include <boost/convert/detail/has_member.hpp>

static bool destroy_all_particles = false;

// ==============================================================
// Particles
// ==============================================================

class ParticleExample
{
public:

	chaos::box2 box;
	glm::vec2 velocity;
	chaos::ParticleTexcoords texcoords;
	float lifetime;
	float remaining_time;
};

CHAOS_REGISTER_CLASS(ParticleExample);

using VertexExample = chaos::VertexDefault;

class AllocationData
{
public:

	int xyz = 666;

};

class ParticleExampleLayerTrait : public chaos::ParticleLayerTrait<ParticleExample, VertexExample, AllocationData>
{
public:


	int BeginParticlesToPrimitives(chaos::ParticleConstAccessor<ParticleExample> & accessor, AllocationData const & data) const
	{
		return 123;
	}

	bool UpdateParticle(float delta_time, ParticleExample & particle, AllocationData const & data) const
	{
		particle.box.position += particle.velocity * delta_time;
		particle.remaining_time -= delta_time;

		return (particle.remaining_time <= 0.0f);
	}

    void ParticleToPrimitives(ParticleExample const & particle, chaos::PrimitiveOutput<VertexExample> & output, int b, AllocationData const & trait) const
    {
		if (rand() % 5 == 0) // flickering particles (not always rendered)
			return;

		chaos::QuadPrimitive<VertexExample> primitive = output.AddQuads();

		glm::vec2 vertex_positions[4];
		chaos::GenerateVertexPositionAttributes(particle.box, 0.0f, vertex_positions);

		glm::vec3 vertex_texcoords[4];
		chaos::GenerateVertexTextureAttributes(particle.texcoords, 0, vertex_texcoords);

		float alpha = particle.remaining_time / particle.lifetime;
		for (size_t i = 0; i < primitive.GetVerticesCount(); ++i)
		{
			primitive[i].position = vertex_positions[i];
			primitive[i].texcoord = vertex_texcoords[i];
			primitive[i].color = glm::vec4(1.0f, 0.5f, 0.25f, alpha);
			primitive[i].position.y += 50 * std::cos(time);
		}
    }

public:

	float time = 0.0f;
};

// ==============================================================
// Application
// ==============================================================

class WindowOpenGLTest : public chaos::Window
{
	int const LAYER_COUNT = 5;
	int const MATERIAL_COUNT = 3;

	float VIEWPORT_WANTED_ASPECT = (16.0f / 9.0f);
	float WORLD_X = 1000.0f;

	CHAOS_DECLARE_OBJECT_CLASS(WindowOpenGLTest, chaos::Window);

protected:

	virtual bool OnDraw(chaos::GPURenderer * renderer, chaos::WindowDrawParams const& draw_params, chaos::GPUProgramProviderInterface const * uniform_provider) override
	{
		bool bbb1 = chaos::has_AllocationTrait_v< ParticleExampleLayerTrait>;
		bool bbb2 = chaos::has_AllocationTrait_v< WindowOpenGLTest>;



		// clear the buffers
		glClearColor(0.4f, 0.4f, 0.4f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		float far_plane = 1000.0f;
		glClearBufferfi(GL_DEPTH_STENCIL, 0, far_plane, 0);

		// change  the viewport
		chaos::GPUProgramProviderChain main_uniform_provider(uniform_provider);

		glm::vec2 world_size     = glm::vec2(WORLD_X, WORLD_X / VIEWPORT_WANTED_ASPECT);
		glm::vec2 world_position = glm::vec2(0.0f, 0.0f);

		glm::vec3 scale = glm::vec3(2.0f / world_size.x, 2.0f / world_size.y, 1.0f);
		glm::vec3 tr    = glm::vec3(-world_position.x, -world_position.y, 0.0f);

		glm::mat4 local_to_cam =  glm::scale(scale) * glm::translate(tr);

		main_uniform_provider.AddVariable("local_to_cam", local_to_cam);

		// draw
		chaos::GPURenderParams render_params;
		particle_manager->Display(renderer, &main_uniform_provider, render_params);

		return true;
	}

	virtual void Finalize() override
	{
		particle_manager = nullptr;
		particle_layers.clear();
		particle_allocations.clear();
		chaos::Window::Finalize();
	}

	virtual bool Tick(float delta_time) override
	{
		particle_manager->Tick(delta_time);

		return true; // no redraw
	}

	virtual bool OnMouseButtonImpl(int button, int action, int modifier) override
	{
		double mouse_x = 0.0;
		double mouse_y = 0.0;
		glfwGetCursorPos(glfw_window, &mouse_x, &mouse_y);

		if (button == 0 && action == GLFW_PRESS)
		{
			int layer_index    = rand() % LAYER_COUNT;
			int material_index = rand() % MATERIAL_COUNT;

			chaos::ParticleLayerBase * particle_layer = particle_manager->FindLayer(material_index + layer_index * MATERIAL_COUNT);
			if (particle_layer != nullptr)
			{
				int particle_count = rand() % 50 + 5;
				chaos::ParticleAllocationBase * allocation = particle_layer->SpawnParticles(particle_count);
				allocation->SetDestroyWhenEmpty(true);

				//particle_allocations.push_back(allocation);

                chaos::ParticleAccessor<ParticleExample> particle_accessor = allocation->GetParticleAccessor();
				if (particle_accessor.GetDataCount())
				{
					glm::vec2 center =
						(2.0f * (chaos::GLMTools::RandVec2() - glm::vec2(0.5f, 0.5f))) * 0.5f * glm::vec2(WORLD_X, WORLD_X / VIEWPORT_WANTED_ASPECT);

				//	glm::vec2 center;
				//	center.x = (float)mouse_x;
				//	center.y = (float)mouse_y;

                    for (int i = 0 ; i < particle_accessor.GetDataCount() ; ++i)
					    InitializeParticles(particle_accessor[i], center);
				}
			}
			return true;
		}
		else if (button == 1 && action == GLFW_PRESS)
		{
			size_t count = particle_allocations.size();
			if (count > 0)
			{
				size_t r = (rand() % count);

				if (button == 1)
				{
					particle_allocations[r] = particle_allocations.back();
					particle_allocations.pop_back();
				}
				else
				{
					particle_allocations[r]->Show(!particle_allocations[r]->IsVisible());
				//	particle_allocations[r]->Pause(!particle_allocations[r]->IsPaused());
				}
			}
			return true;
		}
		else if (button == 2)
		{
			if (action == GLFW_PRESS)
				destroy_all_particles = true;
			else if (action == GLFW_RELEASE)
				destroy_all_particles = false;
		}

		return false;
	}

	virtual bool InitializeFromConfiguration(nlohmann::json const & config) override
	{
		if (!chaos::Window::InitializeFromConfiguration(config))
			return false;

		chaos::GPUResourceManager * gpu_manager = chaos::WindowApplication::GetGPUResourceManagerInstance();
		if (gpu_manager == nullptr)
			return false;

		chaos::GPURenderMaterial * RM1 = gpu_manager->FindRenderMaterial("mat1");
		chaos::GPURenderMaterial * RM2 = gpu_manager->FindRenderMaterial("mat2");
		chaos::GPURenderMaterial * RM3 = gpu_manager->FindRenderMaterial("mat3");

		chaos::GPURenderMaterial * materials[] = { RM1, RM2, RM3 };
		assert(MATERIAL_COUNT == 3);

		particle_manager = new chaos::ParticleManager;

		for (int i = 0; i < LAYER_COUNT; ++i)
		{
			for (int j = 0; j < MATERIAL_COUNT; ++j)
			{
				int render_order = i;
				int layer_id = j + i * MATERIAL_COUNT;
				particle_manager->AddLayer<ParticleExampleLayerTrait>(render_order, layer_id, materials[j]);
			}
		}
		return true;
	}

	void InitializeParticles(ParticleExample & particle, glm::vec2 const & center)
	{
		float WORLD_HEIGHT = 0.5f * WORLD_X / VIEWPORT_WANTED_ASPECT;

        float size = WORLD_HEIGHT * chaos::MathTools::RandFloat() * 0.04f;
        float alpha = chaos::MathTools::RandFloat() * 6.28f;
        float speed = WORLD_HEIGHT * chaos::MathTools::RandFloat() * 0.1f;
        float lifetime = 4.0f + chaos::MathTools::RandFloat() * 2.0f;

        particle.box.position = center;
        particle.box.half_size = 0.5f * glm::vec2(size, size);
        particle.velocity = glm::vec2(
            speed * std::cos(alpha),
            speed * std::sin(alpha));
        particle.lifetime = lifetime;
        particle.remaining_time = lifetime;
	}

	virtual void TweakHints(chaos::WindowHints & hints, GLFWmonitor * monitor, bool pseudo_fullscreen) const override
	{
		chaos::Window::TweakHints(hints, monitor, pseudo_fullscreen);
		hints.toplevel = 0;
		hints.decorated = 1;
	}

protected:

	chaos::shared_ptr<chaos::ParticleManager> particle_manager;

	std::vector<chaos::shared_ptr<chaos::ParticleLayerBase>> particle_layers;
	std::vector<chaos::shared_ptr<chaos::ParticleAllocationBase>> particle_allocations;
};


int main(int argc, char ** argv, char ** env)
{
	chaos::WindowParams params;
	params.monitor = nullptr;
	params.width = 500;
	params.height = 500;
	params.monitor_index = 0;

	chaos::WindowHints hints;

	return chaos::RunWindowApplication<WindowOpenGLTest>(argc, argv, env, params, hints);
}


